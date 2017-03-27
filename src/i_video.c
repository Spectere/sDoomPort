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
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

#include <SDL.h>

#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"

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

Uint16 w_width, w_height;
const Uint16 t_width = SCREENWIDTH, t_height = SCREENHEIGHT;

void I_ShutdownGraphics(void) {
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

extern int scale;
extern int capture_mouse;

//
// I_StartFrame
//
void I_StartFrame(void) {}


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
	Uint32 *tex_pixels;
	int pitch;

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

	/* Draw to the texture. */
	SDL_LockTexture(texture, NULL, (void*)&tex_pixels, &pitch);
	for(i = 0; i < t_width * t_height; i++) {
		/* TODO: Make this endian-agnostic. */
		color c = g_palette[screens[0][i]];
		tex_pixels[i] = 0xFF000000 | c.r << 16 | c.g << 8 | c.b;
	}
	SDL_UnlockTexture(texture);

	/* Update the display. */
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
	int i;
	for(i = 0; i < 256; i++) {
		g_palette[i].r = gammatable[usegamma][*palette++];
		g_palette[i].g = gammatable[usegamma][*palette++];
		g_palette[i].b = gammatable[usegamma][*palette++];
	}
}


void I_InitGraphics(void) {
	/* Initialize the video subsystem. */
	if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
		I_Error("I_InitGraphics: Unable to initialize SDL video!");

	/* Scale the window appropriately. */
	w_width = SCREENWIDTH * scale;
	w_height = SCREENHEIGHT * scale * 1.2;  /* Include aspect ratio correction. */

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

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, t_width, t_height);
	if(texture == NULL) I_Error("I_InitGraphics(): Error creating texture!");

	/* Capture the mouse cursor (if applicable). */
	if(capture_mouse) {
		SDL_CaptureMouse(SDL_TRUE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}
