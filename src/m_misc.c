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
// DESCRIPTION:
//	Main loop menu stuff.
//	Default Config File.
//	PCX Screenshots.
//
//-----------------------------------------------------------------------------

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <SDL.h>

#include <ctype.h>


#include "doomdef.h"

#include "z_zone.h"

#include "m_argv.h"

#include "w_wad.h"

#include "i_file.h"
#include "i_system.h"
#include "i_video.h"
#include "v_video.h"

#include "hu_stuff.h"

// State.
#include "doomstat.h"

// Data.
#include "dstrings.h"

#include "m_misc.h"

//
// M_DrawText
// Returns the final X coordinate
// HU_Init must have been called to init the font
//
extern patch_t* hu_font[HU_FONTSIZE];

int M_DrawText(int x, int y, SDL_bool direct, char* string) {
	int c;
	int w;

	while(*string) {
		c = toupper(*string) - HU_FONTSTART;
		string++;
		if(c < 0 || c > HU_FONTSIZE) {
			x += 4;
			continue;
		}

		w = SDL_SwapLE16(hu_font[c]->width);
		if(x + w > SCREENWIDTH)
			break;
		if(direct)
			V_DrawPatchDirect(x, y, 0, hu_font[c]);
		else
			V_DrawPatch(x, y, 0, hu_font[c]);
		x += w;
	}

	return x;
}


//
// M_WriteFile
//
#ifndef O_BINARY
#define O_BINARY 0
#endif

SDL_bool M_WriteFile(char const* name, void* source, int length) {
	SDL_RWops* handle;
	int count;

	handle = SDL_RWFromFile(name, "wb");

	if(handle == NULL)
		return SDL_FALSE;

	count = SDL_RWwrite(handle, source, length, 1);
	SDL_RWclose(handle);

	if(count == 0)
		return SDL_FALSE;

	return SDL_TRUE;
}


//
// M_ReadFile
//
int M_ReadFile(char const* name, Uint8** buffer) {
	SDL_RWops* handle;
	int count, length;
	Uint8* buf;

	handle = SDL_RWFromFile(name, "rb");
	if(handle == NULL)
		I_Error("Couldn't read file %s", name);
	length = SDL_RWsize(handle);
	buf = Z_Malloc(length, PU_STATIC, NULL);
	count = SDL_RWread(handle, buf, length, 1);
	SDL_RWclose(handle);

	if(count != 1)
		I_Error("Couldn't read file %s", name);

	*buffer = buf;
	return length;
}


//
// DEFAULTS
//
int usejoystick;

extern int key_right;
extern int key_left;
extern int key_up;
extern int key_down;

extern int key_strafeleft;
extern int key_straferight;

extern int key_fire;
extern int key_use;
extern int key_strafe;
extern int key_speed;

extern int mousebfire;
extern int mousebstrafe;
extern int mousebforward;

extern int joybfire;
extern int joybstrafe;
extern int joybuse;
extern int joybspeed;

extern int viewwidth;
extern int viewheight;

extern int mouseSensitivity;
extern int showMessages;

extern int detailLevel;

extern int screenblocks;

extern int showMessages;

// machine-independent sound params
extern int numChannels;

extern int scale;
extern int use_mouse;
extern int capture_mouse;
extern int mouse_sensitivity_multiplier_x;
extern int mouse_sensitivity_multiplier_y;

extern char* chat_macros[];


typedef struct {
	char* name;
	int* location;
	int defaultvalue;
	char* stringvalue; // somehow less hacky than the original code
} default_t;

default_t defaults[] = {
	{"mouse_sensitivity",&mouseSensitivity, 5},
	{"sfx_volume",&snd_SfxVolume, 8},
	{"music_volume",&snd_MusicVolume, 8},
	{"show_messages",&showMessages, 1},

    {"key_right",&key_right, KEY_RIGHTARROW},
    {"key_left",&key_left, KEY_LEFTARROW},
    {"key_up",&key_up, KEY_UPARROW},
    {"key_down",&key_down, KEY_DOWNARROW},
    {"key_strafeleft",&key_strafeleft, ','},
    {"key_straferight",&key_straferight, '.'},

    {"key_fire",&key_fire, KEY_RCTRL},
    {"key_use",&key_use, ' '},
    {"key_strafe",&key_strafe, KEY_RALT},
    {"key_speed",&key_speed, KEY_RSHIFT},

	{"mouseb_fire",&mousebfire,0},
	{"mouseb_strafe",&mousebstrafe,1},
	{"mouseb_forward",&mousebforward,2},

	{"use_joystick",&usejoystick, 0},
	{"joyb_fire",&joybfire,0},
	{"joyb_strafe",&joybstrafe,1},
	{"joyb_use",&joybuse,3},
	{"joyb_speed",&joybspeed,2},

	{"screenblocks",&screenblocks, 9},
	{"detaillevel",&detailLevel, 0},

	{"snd_channels",&numChannels, 3},

	{"scale",&scale, 2},
	{"use_mouse",&use_mouse, 1},
	{"capture_mouse",&capture_mouse, 1},
	{"mouse_sensitivity_multiplier_x",&mouse_sensitivity_multiplier_x, 5},
	{"mouse_sensitivity_multiplier_y",&mouse_sensitivity_multiplier_y, 5},


	{"usegamma",&usegamma, 0},

	{"chatmacro0", (int *) &chat_macros[0], MININT, HUSTR_CHATMACRO0},
	{"chatmacro1", (int *) &chat_macros[1], MININT, HUSTR_CHATMACRO1},
	{"chatmacro2", (int *) &chat_macros[2], MININT, HUSTR_CHATMACRO2},
	{"chatmacro3", (int *) &chat_macros[3], MININT, HUSTR_CHATMACRO3},
	{"chatmacro4", (int *) &chat_macros[4], MININT, HUSTR_CHATMACRO4},
	{"chatmacro5", (int *) &chat_macros[5], MININT, HUSTR_CHATMACRO5},
	{"chatmacro6", (int *) &chat_macros[6], MININT, HUSTR_CHATMACRO6},
	{"chatmacro7", (int *) &chat_macros[7], MININT, HUSTR_CHATMACRO7},
	{"chatmacro8", (int *) &chat_macros[8], MININT, HUSTR_CHATMACRO8},
	{"chatmacro9", (int *) &chat_macros[9], MININT, HUSTR_CHATMACRO9}

};

int numdefaults;
char* defaultfile;


//
// M_SaveDefaults
//
void M_SaveDefaults(void) {
	int i;
	int v;
	FILE* f;

	f = fopen(defaultfile, "w");
	if(!f)
		return; // can't write the file, but don't complain

	for(i = 0; i < numdefaults; i++) {
		if(defaults[i].defaultvalue > -0xfff
		   && defaults[i].defaultvalue < 0xfff) {
			v = *defaults[i].location;
			fprintf(f, "%s\t\t%i\n", defaults[i].name, v);
		} else {
			fprintf(f, "%s\t\t\"%s\"\n", defaults[i].name,
			        defaults[i].stringvalue);
		}
	}

	fclose(f);
}


//
// M_LoadDefaults
//
extern Uint8 scantokey[128];

void M_LoadDefaults(void) {
	int i;
	int len;
	FILE* f;
	char def[80];
	char strparm[100];
	char* newstring;
	int parm;
	SDL_bool isstring;

	// set everything to base values
	numdefaults = sizeof(defaults) / sizeof(defaults[0]);
	for(i = 0; i < numdefaults; i++)
		*defaults[i].location = defaults[i].defaultvalue;

	// check for a custom default file
	i = M_CheckParm("-config");
	if(i && i < myargc - 1) {
		defaultfile = myargv[i + 1];
		printf("	default file: %s\n", defaultfile);
	} else
		defaultfile = basedefault;

	// read the file in, overriding any set defaults
	f = fopen(defaultfile, "r");
	if(f) {
		while(!feof(f)) {
			isstring = SDL_FALSE;
			if(fscanf(f, "%79s %[^\n]\n", def, strparm) == 2) {
				if(strparm[0] == '"') {
					// get a string default
					isstring = SDL_TRUE;
					len = strlen(strparm);
					newstring = (char *) malloc(len);
					strparm[len - 1] = 0;
					strcpy(newstring, strparm + 1);
				} else if(strparm[0] == '0' && strparm[1] == 'x') {
					sscanf(strparm + 2, "%x", &parm);
				} else {
					sscanf(strparm, "%i", &parm);
				}
				for(i = 0; i < numdefaults; i++)
					if(!strcmp(def, defaults[i].name)) {
						if(!isstring)
							*defaults[i].location = parm;
						else
							// ReSharper disable once CppLocalVariableMightNotBeInitialized
							*defaults[i].location = (int)newstring;
						break;
					}
			}
		}

		fclose(f);
	}
}


//
// SCREEN SHOTS
//


typedef struct {
	char manufacturer;
	char version;
	char encoding;
	char bits_per_pixel;

	unsigned short xmin;
	unsigned short ymin;
	unsigned short xmax;
	unsigned short ymax;

	unsigned short hres;
	unsigned short vres;

	unsigned char palette[48];

	char reserved;
	char color_planes;
	unsigned short bytes_per_line;
	unsigned short palette_type;

	char filler[58];
	unsigned char data; // unbounded
} pcx_t;


//
// WritePCXfile
//
void WritePCXfile(char* filename, Uint8* data, int width, int height, Uint8* palette) {
	int i;
	int length;
	pcx_t* pcx;
	Uint8* pack;

	pcx = Z_Malloc(width * height * 2 + 1000, PU_STATIC, NULL);

	pcx->manufacturer = 0x0a; // PCX id
	pcx->version = 5; // 256 color
	pcx->encoding = 1; // uncompressed
	pcx->bits_per_pixel = 8; // 256 color
	pcx->xmin = 0;
	pcx->ymin = 0;
	pcx->xmax = SDL_SwapLE16(width-1);
	pcx->ymax = SDL_SwapLE16(height-1);
	pcx->hres = SDL_SwapLE16(width);
	pcx->vres = SDL_SwapLE16(height);
	memset(pcx->palette, 0, sizeof(pcx->palette));
	pcx->color_planes = 1; // chunky image
	pcx->bytes_per_line = SDL_SwapLE16(width);
	pcx->palette_type = SDL_SwapLE16(2); // not a grey scale
	memset(pcx->filler, 0, sizeof(pcx->filler));


	// pack the image
	pack = &pcx->data;

	for(i = 0; i < width * height; i++) {
		if((*data & 0xc0) != 0xc0)
			*pack++ = *data++;
		else {
			*pack++ = 0xc1;
			*pack++ = *data++;
		}
	}

	// write the palette
	*pack++ = 0x0c; // palette ID byte
	for(i = 0; i < 768; i++)
		*pack++ = *palette++;

	// write output file
	length = pack - (Uint8 *)pcx;
	M_WriteFile(filename, pcx, length);

	Z_Free(pcx);
}


//
// M_ScreenShot
//
void M_ScreenShot(void) {
	int i;
	Uint8* linear;
	char lbmname[12];

	// munge planar buffer to linear
	linear = screens[2];
	I_ReadScreen(linear);

	// find a file name to save it to
	strcpy(lbmname, "DOOM00.pcx");

	for(i = 0; i <= 99; i++) {
		lbmname[4] = i / 10 + '0';
		lbmname[5] = i % 10 + '0';
		if(I_FileExists(lbmname) == SDL_FALSE)
			break; // file doesn't exist
	}
	if(i == 100)
		I_Error("M_ScreenShot: Couldn't create a PCX");

	// save the pcx file
	WritePCXfile(lbmname, linear,
	             SCREENWIDTH, SCREENHEIGHT,
	             W_CacheLumpName("PLAYPAL",PU_CACHE));

	players[consoleplayer].message = "screen shot";
}
