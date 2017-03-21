// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
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
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <SDL.h>
#include <stdarg.h>

#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "d_main.h"

#include "doomdef.h"


typedef struct {
	Uint8 r;
	Uint8 g;
	Uint8 b;
} color;

static color g_palette[256];

static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* texture;
static Uint32* pixels;

const Uint16 w_width = 320, w_height = 200;

void I_ShutdownGraphics(void) {}


//
// I_StartFrame
//
void I_StartFrame(void) {}

void I_GetEvent(void) {
	event_t event;

	/* TODO: Actually process events. :) */

	D_PostEvent(&event);
}

//
// I_StartTic
//
void I_StartTic(void) {
	I_GetEvent();
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit(void) {}

//
// I_FinishUpdate
//
void I_FinishUpdate(void) {

	static int lasttic;
	int tics;
	int i;

	// draws little dots on the bottom of the screen
	if(devparm) {
		i = I_GetTime();
		tics = i - lasttic;
		lasttic = i;
		if(tics > 20) tics = 20;

		for(i = 0; i < tics * 2; i += 2)
			screens[0][(SCREENHEIGHT - 1) * SCREENWIDTH + i] = 0xff;
		for(; i < 20 * 2; i += 2)
			screens[0][(SCREENHEIGHT - 1) * SCREENWIDTH + i] = 0x0;
	}

	/* Convert the palettized buffer to a 32-bit ARGB texture. */
	for(i = 0; i < w_width * w_height; i++) {
		/* TODO: Make this endian-agnostic. */
		color c = g_palette[screens[0][i]];
		pixels[i] = c.r << 24 && c.g << 16 && c.b;
	}

	SDL_UpdateTexture(texture, NULL, pixels, w_width * sizeof(Uint8));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


//
// I_ReadScreen
//
void I_ReadScreen(byte* scr) {
	memcpy(scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}


//
// I_SetPalette
//
void I_SetPalette(byte* palette) {
	memcpy(g_palette, palette, sizeof(palette));
}


void I_InitGraphics(void) {
	/* Create the SDL window. */
	window = SDL_CreateWindow(
		"sDoomPort",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		w_width, w_height,
		0
	);
	if(window == NULL) I_Error("I_InitGraphics(): Error creating window!");

	/* Create a hardware renderer if possible. Otherwise, try to fall back to software.*/
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == NULL) {
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if(renderer == NULL) I_Error("I_InitGraphics(): Error creating renderer!");
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w_width, w_height);
	if(texture == NULL) I_Error("I_InitGraphics(): Error creating texture!");

	screens[0] = (Uint8*)malloc(w_width * w_height);
	pixels = (Uint32*)malloc(w_width * w_height * sizeof(Uint32));
}
