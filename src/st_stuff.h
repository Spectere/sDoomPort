//-----------------------------------------------------------------------------
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 2017-2018 by Ian Burgmyer
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
//	Status bar code.
//	Does the face/direction indicator animatin.
//	Does palette indicators as well (red pain/berserk, bright pickup)
//
//-----------------------------------------------------------------------------

#ifndef __STSTUFF_H__
#define __STSTUFF_H__

#include <SDL_stdinc.h>
#include "doomtype.h"
#include "d_event.h"

// Size of statusbar.
// Now sensitive for scaling.
#define ST_HEIGHT	32*SCREEN_MUL
#define ST_WIDTH	320
#define ST_X        ((SCREENWIDTH - ST_WIDTH) / 2)
#define ST_Y        (SCREENHEIGHT - ST_HEIGHT)


//
// STATUS BAR
//

// Called by main loop.
SDL_bool ST_Responder(event_t* ev);

// Called by main loop.
void ST_Ticker(void);

// Called by main loop.
void ST_Drawer(SDL_bool fullscreen);

// Called when the console player is spawned on each level.
void ST_Start(void);

// Called by startup code.
void ST_Init(void);


// States for status bar code.
typedef enum {
	AutomapState,
	FirstPersonState

} st_stateenum_t;


// States for the chat code.
typedef enum {
	StartChatState,
	WaitDestState,
	GetChatState

} st_chatstateenum_t;

SDL_bool ST_Responder(event_t* ev);

#endif
