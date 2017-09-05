//-----------------------------------------------------------------------------
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 2017 by Ian Burgmyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:  Heads-up displays
//
//-----------------------------------------------------------------------------

#include <ctype.h>
#include <SDL_stdinc.h>

#include "doomdef.h"

#include "z_zone.h"

#include "hu_stuff.h"
#include "hu_lib.h"
#include "w_wad.h"

#include "r_local.h"
#include "p_spec.h"
#include "p_local.h"
#include "p_tick.h"

#include "s_sound.h"

#include "doomstat.h"

// Data.
#include "dstrings.h"
#include "sounds.h"

#include "hu_debug.h"

//
// Locally used constants, shortcuts.
//
#define HU_TITLE	(mapnames[(gameepisode-1)*9+gamemap-1])
#define HU_TITLE2	(mapnames2[gamemap-1])
#define HU_TITLEP	(mapnamesp[gamemap-1])
#define HU_TITLET	(mapnamest[gamemap-1])
#define HU_TITLEFD1	(mapnamefd1[(gameepisode-1)*9+gamemap-1])
#define HU_TITLEFD2	(mapnamefd2[gamemap-1])
#define HU_TITLEHEIGHT	1
#define HU_TITLEX	0
#define HU_TITLEY	(167 - SDL_SwapLE16(hu_font[0]->height))
#define HU_DEBUGX	0
#define HU_DEBUGWIDTH 80

#define HU_INPUTTOGGLE	't'
#define HU_INPUTX	HU_MSGX
#define HU_INPUTY	(HU_MSGY + HU_MSGHEIGHT*(SDL_SwapLE16(hu_font[0]->height) +1))
#define HU_INPUTWIDTH	64
#define HU_INPUTHEIGHT	1

char* chat_macros[] = {
	HUSTR_CHATMACRO0,
	HUSTR_CHATMACRO1,
	HUSTR_CHATMACRO2,
	HUSTR_CHATMACRO3,
	HUSTR_CHATMACRO4,
	HUSTR_CHATMACRO5,
	HUSTR_CHATMACRO6,
	HUSTR_CHATMACRO7,
	HUSTR_CHATMACRO8,
	HUSTR_CHATMACRO9
};

char* player_names[] = {
	HUSTR_PLRGREEN,
	HUSTR_PLRINDIGO,
	HUSTR_PLRBROWN,
	HUSTR_PLRRED
};


char chat_char; // remove later.
static player_t* plr;
patch_t* hu_font[HU_FONTSIZE];
static hu_textline_t w_title;
SDL_bool chat_on;
static hu_itext_t w_chat;
static SDL_bool always_off = SDL_FALSE;
static char chat_dest[MAXPLAYERS];
static hu_itext_t w_inputbuffer[MAXPLAYERS];

static SDL_bool message_on;
SDL_bool message_dontfuckwithme;
static SDL_bool message_nottobefuckedwith;

static hu_stext_t w_message;
static int message_counter;

extern int showMessages;
extern SDL_bool automapactive;

static SDL_bool headsupactive = SDL_FALSE;

static hu_textline_t* w_debug;

//
// Builtin map names.
// The actual names can be found in DStrings.h.
//

char* mapnames[] = { // DOOM shareware/registered/retail (Ultimate) names.
	HUSTR_E1M1,
	HUSTR_E1M2,
	HUSTR_E1M3,
	HUSTR_E1M4,
	HUSTR_E1M5,
	HUSTR_E1M6,
	HUSTR_E1M7,
	HUSTR_E1M8,
	HUSTR_E1M9,

	HUSTR_E2M1,
	HUSTR_E2M2,
	HUSTR_E2M3,
	HUSTR_E2M4,
	HUSTR_E2M5,
	HUSTR_E2M6,
	HUSTR_E2M7,
	HUSTR_E2M8,
	HUSTR_E2M9,

	HUSTR_E3M1,
	HUSTR_E3M2,
	HUSTR_E3M3,
	HUSTR_E3M4,
	HUSTR_E3M5,
	HUSTR_E3M6,
	HUSTR_E3M7,
	HUSTR_E3M8,
	HUSTR_E3M9,

	HUSTR_E4M1,
	HUSTR_E4M2,
	HUSTR_E4M3,
	HUSTR_E4M4,
	HUSTR_E4M5,
	HUSTR_E4M6,
	HUSTR_E4M7,
	HUSTR_E4M8,
	HUSTR_E4M9,

	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL"
};

char* mapnames2[] = { // DOOM 2 map names.
	HUSTR_1,
	HUSTR_2,
	HUSTR_3,
	HUSTR_4,
	HUSTR_5,
	HUSTR_6,
	HUSTR_7,
	HUSTR_8,
	HUSTR_9,
	HUSTR_10,
	HUSTR_11,

	HUSTR_12,
	HUSTR_13,
	HUSTR_14,
	HUSTR_15,
	HUSTR_16,
	HUSTR_17,
	HUSTR_18,
	HUSTR_19,
	HUSTR_20,

	HUSTR_21,
	HUSTR_22,
	HUSTR_23,
	HUSTR_24,
	HUSTR_25,
	HUSTR_26,
	HUSTR_27,
	HUSTR_28,
	HUSTR_29,
	HUSTR_30,
	HUSTR_31,
	HUSTR_32
};


char* mapnamesp[] = { // Plutonia WAD map names.
	PHUSTR_1,
	PHUSTR_2,
	PHUSTR_3,
	PHUSTR_4,
	PHUSTR_5,
	PHUSTR_6,
	PHUSTR_7,
	PHUSTR_8,
	PHUSTR_9,
	PHUSTR_10,
	PHUSTR_11,

	PHUSTR_12,
	PHUSTR_13,
	PHUSTR_14,
	PHUSTR_15,
	PHUSTR_16,
	PHUSTR_17,
	PHUSTR_18,
	PHUSTR_19,
	PHUSTR_20,

	PHUSTR_21,
	PHUSTR_22,
	PHUSTR_23,
	PHUSTR_24,
	PHUSTR_25,
	PHUSTR_26,
	PHUSTR_27,
	PHUSTR_28,
	PHUSTR_29,
	PHUSTR_30,
	PHUSTR_31,
	PHUSTR_32
};


char* mapnamest[] = { // TNT WAD map names.
	THUSTR_1,
	THUSTR_2,
	THUSTR_3,
	THUSTR_4,
	THUSTR_5,
	THUSTR_6,
	THUSTR_7,
	THUSTR_8,
	THUSTR_9,
	THUSTR_10,
	THUSTR_11,

	THUSTR_12,
	THUSTR_13,
	THUSTR_14,
	THUSTR_15,
	THUSTR_16,
	THUSTR_17,
	THUSTR_18,
	THUSTR_19,
	THUSTR_20,

	THUSTR_21,
	THUSTR_22,
	THUSTR_23,
	THUSTR_24,
	THUSTR_25,
	THUSTR_26,
	THUSTR_27,
	THUSTR_28,
	THUSTR_29,
	THUSTR_30,
	THUSTR_31,
	THUSTR_32
};

char* mapnamefd1[] = { /* Freedoom - Phase 1 */
	FREEDM1_E1M1,
	FREEDM1_E1M2,
	FREEDM1_E1M3,
	FREEDM1_E1M4,
	FREEDM1_E1M5,
	FREEDM1_E1M6,
	FREEDM1_E1M7,
	FREEDM1_E1M8,
	FREEDM1_E1M9,

	FREEDM1_E2M1,
	FREEDM1_E2M2,
	FREEDM1_E2M3,
	FREEDM1_E2M4,
	FREEDM1_E2M5,
	FREEDM1_E2M6,
	FREEDM1_E2M7,
	FREEDM1_E2M8,
	FREEDM1_E2M9,

	FREEDM1_E3M1,
	FREEDM1_E3M2,
	FREEDM1_E3M3,
	FREEDM1_E3M4,
	FREEDM1_E3M5,
	FREEDM1_E3M6,
	FREEDM1_E3M7,
	FREEDM1_E3M8,
	FREEDM1_E3M9,

	FREEDM1_E4M1,
	FREEDM1_E4M2,
	FREEDM1_E4M3,
	FREEDM1_E4M4,
	FREEDM1_E4M5,
	FREEDM1_E4M6,
	FREEDM1_E4M7,
	FREEDM1_E4M8,
	FREEDM1_E4M9,

	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL",
	"NEWLEVEL"
};

char* mapnamefd2[] = { /* Freedoom - Phase 2 */
	FREEDM2_1,
	FREEDM2_2,
	FREEDM2_3,
	FREEDM2_4,
	FREEDM2_5,
	FREEDM2_6,
	FREEDM2_7,
	FREEDM2_8,
	FREEDM2_9,
	FREEDM2_10,
	FREEDM2_11,
	FREEDM2_12,
	FREEDM2_13,
	FREEDM2_14,
	FREEDM2_15,
	FREEDM2_16,
	FREEDM2_17,
	FREEDM2_18,
	FREEDM2_19,
	FREEDM2_20,
	FREEDM2_21,
	FREEDM2_22,
	FREEDM2_23,
	FREEDM2_24,
	FREEDM2_25,
	FREEDM2_26,
	FREEDM2_27,
	FREEDM2_28,
	FREEDM2_29,
	FREEDM2_30,
	FREEDM2_31,
	FREEDM2_32
};

const char* shiftxform;

const char french_shiftxform[] = {
	0,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	31,
	' ', '!', '"', '#', '$', '%', '&',
	'"', // shift-'
	'(', ')', '*', '+',
	'?', // shift-,
	'_', // shift--
	'>', // shift-.
	'?', // shift-/
	'0', // shift-0
	'1', // shift-1
	'2', // shift-2
	'3', // shift-3
	'4', // shift-4
	'5', // shift-5
	'6', // shift-6
	'7', // shift-7
	'8', // shift-8
	'9', // shift-9
	'/',
	'.', // shift-;
	'<',
	'+', // shift-=
	'>', '?', '@',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'[', // shift-[
	'!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
	']', // shift-]
	'"', '_',
	'\'', // shift-`
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'{', '|', '}', '~', 127

};

const char english_shiftxform[] = {
	0,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	31,
	' ', '!', '"', '#', '$', '%', '&',
	'"', // shift-'
	'(', ')', '*', '+',
	'<', // shift-,
	'_', // shift--
	'>', // shift-.
	'?', // shift-/
	')', // shift-0
	'!', // shift-1
	'@', // shift-2
	'#', // shift-3
	'$', // shift-4
	'%', // shift-5
	'^', // shift-6
	'&', // shift-7
	'*', // shift-8
	'(', // shift-9
	':',
	':', // shift-;
	'<',
	'+', // shift-=
	'>', '?', '@',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'[', // shift-[
	'!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
	']', // shift-]
	'"', '_',
	'\'', // shift-`
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'{', '|', '}', '~', 127
};

char frenchKeyMap[128] = {
	0,
	1,2,3,4,5,6,7,8,9,10,
	11,12,13,14,15,16,17,18,19,20,
	21,22,23,24,25,26,27,28,29,30,
	31,
	' ','!','"','#','$','%','&','%','(',')','*','+',';','-',':','!',
	'0','1','2','3','4','5','6','7','8','9',':','M','<','=','>','?',
	'@','Q','B','C','D','E','F','G','H','I','J','K','L',',','N','O',
	'P','A','R','S','T','U','V','Z','X','Y','W','^','\\','$','^','_',
	'@','Q','B','C','D','E','F','G','H','I','J','K','L',',','N','O',
	'P','A','R','S','T','U','V','Z','X','Y','W','^','\\','$','^',127
};

static inline int DebugY(int offset) {
	return 167 - (SDL_SwapLE16(hu_font[0]->height + 1)) * (offset + 2);
}

char ForeignTranslation(unsigned char ch) {
	return ch < 128 ? frenchKeyMap[ch] : ch;
}

#define HUD_DPRINT(name, fmt, count, max) \
	x = count; \
	if(max < x) max = x; \
	HUlib_clearTextLine(&name); \
	sprintf(buf, fmt, x, max); \
	i = 0; \
	while(buf[i]) HUlib_addCharToTextLine(&name, buf[i++])

void DebugPrint(void) {
	char buf[HU_DEBUGWIDTH];
	int i, j;

	HU_DebugUpdate();
	for(i = 0; i < debug_count; i++) {
		debug_stat_t dbg = debug_stat[i];

		HUlib_clearTextLine(&w_debug[i]);
		sprintf(buf, "%s: %i / %i", dbg.desc, dbg.cur, dbg.max);
		j = 0;
		while(buf[j])
			HUlib_addCharToTextLine(&w_debug[i], buf[j++]);
	}
}

void HU_Init(void) {

	int i;
	int j;
	char buffer[9];

	if(language == french)
		shiftxform = french_shiftxform;
	else
		shiftxform = english_shiftxform;

	// load the heads-up font
	j = HU_FONTSTART;
	for(i = 0; i < HU_FONTSIZE; i++) {
		sprintf(buffer, "STCFN%.3d", j++);
		hu_font[i] = (patch_t *) W_CacheLumpName(buffer, PU_STATIC);
	}

	HU_DebugInit();
	w_debug = malloc(sizeof(hu_textline_t) * debug_count);
}

void HU_Stop(void) {
	headsupactive = SDL_FALSE;
}

void HU_Start(void) {

	int i;
	char* s;

	if(headsupactive)
		HU_Stop();

	plr = &players[consoleplayer];
	message_on = SDL_FALSE;
	message_dontfuckwithme = SDL_FALSE;
	message_nottobefuckedwith = SDL_FALSE;
	chat_on = SDL_FALSE;

	// create the message widget
	HUlib_initSText(&w_message,
	                HU_MSGX, HU_MSGY, HU_MSGHEIGHT,
	                hu_font,
	                HU_FONTSTART, &message_on);

	// create the map title widget
	HUlib_initTextLine(&w_title,
	                   HU_TITLEX, HU_TITLEY,
	                   hu_font,
	                   HU_FONTSTART);

	/* SDP debugging information */
	for(i = 0; i < debug_count; i++) {
		HUlib_initTextLine(&w_debug[debug_count - 1 - i],
			HU_DEBUGX, DebugY(i), hu_font, HU_FONTSTART);
	}

	switch(gamemode) {
		case shareware:
		case registered:
		case retail:
			if(gamemission == freedoom)
				s = HU_TITLEFD1;
			else
				s = HU_TITLE;
			break;

		case commercial:
			switch(gamemission) {
				case pack_plut:
					s = HU_TITLEP;
					break;
				case pack_tnt:
					s = HU_TITLET;
					break;
				case freedoom2:
					s = HU_TITLEFD2;
					break;
				default:
					s = HU_TITLE2;
					break;
			}
			break;

		default:
			s = HU_TITLE2;
			break;
	}

	while(*s)
		HUlib_addCharToTextLine(&w_title, *(s++));

	// create the chat widget
	HUlib_initIText(&w_chat,
	                HU_INPUTX, HU_INPUTY,
	                hu_font,
	                HU_FONTSTART, &chat_on);

	// create the inputbuffer widgets
	for(i = 0; i < MAXPLAYERS; i++)
		HUlib_initIText(&w_inputbuffer[i], 0, 0, 0, 0, &always_off);

	headsupactive = SDL_TRUE;

	/* Reset the max stats for the developer overlay. */
	HU_DebugResetStats();
}

void HU_Drawer(void) {
	int i, j;

	HUlib_drawSText(&w_message);
	HUlib_drawIText(&w_chat);
	if(automapactive)
		HUlib_drawTextLine(&w_title, SDL_FALSE);
	
	for(i = 0; i < MAXPLAYERS; i++) {
		if(i != consoleplayer)
			break;
		if(!players[i].stat_display)
			break;
		
		for(j = 0; j < debug_count; j++)
			HUlib_drawTextLine(&w_debug[j], SDL_FALSE);
	}
}

void HU_Erase(void) {

	HUlib_eraseSText(&w_message);
	HUlib_eraseIText(&w_chat);
	HUlib_eraseTextLine(&w_title);

}

void HU_Ticker(void) {

	int i, rc;
	char c;

	// tick down message counter if message is up
	if(message_counter && !--message_counter) {
		message_on = SDL_FALSE;
		message_nottobefuckedwith = SDL_FALSE;
	}

	if(showMessages || message_dontfuckwithme) {

		// display message if necessary
		if((plr->message && !message_nottobefuckedwith)
		   || (plr->message && message_dontfuckwithme)) {
			HUlib_addMessageToSText(&w_message, 0, plr->message);
			plr->message = 0;
			message_on = SDL_TRUE;
			message_counter = HU_MSGTIMEOUT;
			message_nottobefuckedwith = message_dontfuckwithme;
			message_dontfuckwithme = 0;
		}

	} // else message_on = false;

	/* print SDP debug information (if enabled) */
	DebugPrint();

	// check for incoming chat characters
	if(netgame) {
		for(i = 0; i < MAXPLAYERS; i++) {
			if(!playeringame[i])
				continue;
			if(i != consoleplayer
			   && ((c = players[i].cmd.chatchar))) {
				if(c <= HU_BROADCAST)
					chat_dest[i] = c;
				else {
					if(c >= 'a' && c <= 'z')
						c = (char) shiftxform[(unsigned char) c];
					rc = HUlib_keyInIText(&w_inputbuffer[i], c);
					if(rc && c == KEY_ENTER) {
						if(w_inputbuffer[i].l.len
						   && (chat_dest[i] == consoleplayer + 1
						       || chat_dest[i] == HU_BROADCAST)) {
							HUlib_addMessageToSText(&w_message,
							                        player_names[i],
							                        w_inputbuffer[i].l.l);

							message_nottobefuckedwith = SDL_TRUE;
							message_on = SDL_TRUE;
							message_counter = HU_MSGTIMEOUT;
							if(gamemode == commercial)
								S_StartSound(0, sfx_radio);
							else
								S_StartSound(0, sfx_tink);
						}
						HUlib_resetIText(&w_inputbuffer[i]);
					}
				}
				players[i].cmd.chatchar = 0;
			}
		}
	}

}

#define QUEUESIZE		128

static char chatchars[QUEUESIZE];
static int head = 0;
static int tail = 0;


void HU_queueChatChar(char c) {
	if(((head + 1) & (QUEUESIZE - 1)) == tail) {
		plr->message = HUSTR_MSGU;
	} else {
		chatchars[head] = c;
		head = (head + 1) & (QUEUESIZE - 1);
	}
}

char HU_dequeueChatChar(void) {
	char c;

	if(head != tail) {
		c = chatchars[tail];
		tail = (tail + 1) & (QUEUESIZE - 1);
	} else {
		c = 0;
	}

	return c;
}

SDL_bool HU_Responder(event_t* ev) {

	static char lastmessage[HU_MAXLINELENGTH + 1];
	char* macromessage;
	SDL_bool eatkey = SDL_FALSE;
	static SDL_bool shiftdown = SDL_FALSE;
	static SDL_bool altdown = SDL_FALSE;
	unsigned char c;
	int i;
	int numplayers;

	static char destination_keys[MAXPLAYERS] =
	{
		HUSTR_KEYGREEN,
		HUSTR_KEYINDIGO,
		HUSTR_KEYBROWN,
		HUSTR_KEYRED
	};

	static int num_nobrainers = 0;

	numplayers = 0;
	for(i = 0; i < MAXPLAYERS; i++)
		numplayers += playeringame[i];

	if(ev->data1 == KEY_RSHIFT) {
		shiftdown = ev->type == ev_keydown;
		return SDL_FALSE;
	} else if(ev->data1 == KEY_RALT || ev->data1 == KEY_LALT) {
		altdown = ev->type == ev_keydown;
		return SDL_FALSE;
	}

	if(ev->type != ev_keydown)
		return SDL_FALSE;

	if(!chat_on) {
		if(ev->data1 == HU_MSGREFRESH) {
			message_on = SDL_TRUE;
			message_counter = HU_MSGTIMEOUT;
			eatkey = SDL_TRUE;
		} else if(netgame && ev->data1 == HU_INPUTTOGGLE) {
			eatkey = chat_on = SDL_TRUE;
			HUlib_resetIText(&w_chat);
			HU_queueChatChar(HU_BROADCAST);
		} else if(netgame && numplayers > 2) {
			for(i = 0; i < MAXPLAYERS; i++) {
				if(ev->data1 == destination_keys[i]) {
					if(playeringame[i] && i != consoleplayer) {
						eatkey = chat_on = SDL_TRUE;
						HUlib_resetIText(&w_chat);
						HU_queueChatChar(i + 1);
						break;
					} else if(i == consoleplayer) {
						num_nobrainers++;
						if(num_nobrainers < 3)
							plr->message = HUSTR_TALKTOSELF1;
						else if(num_nobrainers < 6)
							plr->message = HUSTR_TALKTOSELF2;
						else if(num_nobrainers < 9)
							plr->message = HUSTR_TALKTOSELF3;
						else if(num_nobrainers < 32)
							plr->message = HUSTR_TALKTOSELF4;
						else
							plr->message = HUSTR_TALKTOSELF5;
					}
				}
			}
		}
	} else {
		c = ev->data1;
		// send a macro
		if(altdown) {
			c = c - '0';
			if(c > 9)
				return SDL_FALSE;
			// fprintf(stderr, "got here\n");
			macromessage = chat_macros[c];

			// kill last message with a '\n'
			HU_queueChatChar(KEY_ENTER); // DEBUG!!!

			// send the macro message
			while(*macromessage)
				HU_queueChatChar(*macromessage++);
			HU_queueChatChar(KEY_ENTER);

			// leave chat mode and notify that it was sent
			chat_on = SDL_FALSE;
			strcpy(lastmessage, chat_macros[c]);
			plr->message = lastmessage;
			eatkey = SDL_TRUE;
		} else {
			if(french)
				c = ForeignTranslation(c);
			if(shiftdown || (c >= 'a' && c <= 'z'))
				c = shiftxform[c];
			eatkey = HUlib_keyInIText(&w_chat, c);
			if(eatkey) {
				// static unsigned char buf[20]; // DEBUG
				HU_queueChatChar(c);

				// sprintf(buf, "KEY: %d => %d", ev->data1, c);
				//      plr->message = buf;
			}
			if(c == KEY_ENTER) {
				chat_on = SDL_FALSE;
				if(w_chat.l.len) {
					strcpy(lastmessage, w_chat.l.l);
					plr->message = lastmessage;
				}
			} else if(c == KEY_ESCAPE)
				chat_on = SDL_FALSE;
		}
	}

	return eatkey;

}
