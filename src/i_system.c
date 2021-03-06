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
//
//-----------------------------------------------------------------------------

#include <SDL_stdinc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>

#include "m_misc.h"
#include "i_event.h"
#include "i_video.h"
#include "i_sound.h"

#include "d_net.h"
#include "g_game.h"

#include "i_system.h"


int mb_used = 6;

ticcmd_t emptycmd;

ticcmd_t* I_BaseTiccmd(void) {
	return &emptycmd;
}


int I_GetHeapSize(void) {
	return mb_used * 1024 * 1024;
}

Uint8* I_ZoneBase(int* size) {
	*size = mb_used * 1024 * 1024;
	return (Uint8 *) malloc(*size);
}


//
// I_GetTime
// returns time in 1/35th second tics
//
int I_GetTime(void) {
	/* TODO: Update this to use "wall clock" time instead. */
	return SDL_GetTicks() / 35;
}


//
// I_Init
//
void I_Init(void) {
	I_InitSound();
	//  I_InitGraphics();
}

//
// I_Quit
//
void I_Quit(void) {
	D_QuitNetGame();
	I_ShutdownSound();
	I_ShutdownMusic();
	M_SaveDefaults();
	I_ShutdownGraphics();
	exit(0);
}

void I_WaitVBL(int count) {
	/* TODO: Probably not the best approach. */
	SDL_Delay(count * (1000 / 70));
}

void I_BeginRead(void) {}

void I_EndRead(void) {}

Uint8* I_AllocLow(int length) {
	Uint8* mem;

	mem = (Uint8 *)malloc(length);
	memset(mem, 0, length);
	return mem;
}

//
// I_StartTic
//
void I_StartTic(void) {
	I_GetEvent();
}

//
// I_Error
//
extern SDL_bool demorecording;

void I_Error(char* error, ...) {
	va_list argptr;

	// Message first.
	va_start (argptr,error);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, error, argptr);
	fprintf(stderr, "\n");
	va_end (argptr);

	fflush(stderr);

	// Shutdown. Here might be other errors.
	if(demorecording)
		G_CheckDemoStatus();

	D_QuitNetGame();
	I_ShutdownGraphics();

	exit(-1);
}
