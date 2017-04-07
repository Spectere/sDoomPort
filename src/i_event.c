//-----------------------------------------------------------------------------
//
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
//	SDL event handler.
//
//-----------------------------------------------------------------------------

#include <SDL.h>

#include "d_event.h"
#include "d_main.h"
#include "doomdef.h"

extern int mouse_sensitivity_multiplier_x;
extern int mouse_sensitivity_multiplier_y;

static int MapSDLKey(SDL_Keysym keysym) {
	/* Convert uppercase symbobls into lowercase. */
	if(keysym.sym >= 'A' && keysym.sym <= 'Z')
		return keysym.sym + 0x20;

	/* Handles everything from Space to Tilde. */
	if(keysym.sym >= 0x20 && keysym.sym <= 0x7E)
		return keysym.sym;

	/* Try to handle everything else. */
	switch(keysym.sym) {
	case SDLK_LCTRL: case SDLK_RCTRL:
		return KEY_RCTRL;
	case SDLK_LSHIFT: case SDLK_RSHIFT:
		return KEY_RSHIFT;
	case SDLK_LALT: case SDLK_RALT:
		return KEY_RALT;
	case SDLK_UP:
		return KEY_UPARROW;
	case SDLK_DOWN:
		return KEY_DOWNARROW;
	case SDLK_LEFT:
		return KEY_LEFTARROW;
	case SDLK_RIGHT:
		return KEY_RIGHTARROW;
	case SDLK_RETURN: case SDLK_RETURN2:
		return KEY_ENTER;
	case SDLK_BACKSPACE:
		return KEY_BACKSPACE;
	case SDLK_ESCAPE:
		return KEY_ESCAPE;
	case SDLK_PAUSE:
		return KEY_PAUSE;
	case SDLK_TAB:
		return KEY_TAB;
	case SDLK_F1:
		return KEY_F1;
	case SDLK_F2:
		return KEY_F2;
	case SDLK_F3:
		return KEY_F3;
	case SDLK_F4:
		return KEY_F4;
	case SDLK_F5:
		return KEY_F5;
	case SDLK_F6:
		return KEY_F6;
	case SDLK_F7:
		return KEY_F7;
	case SDLK_F8:
		return KEY_F8;
	case SDLK_F9:
		return KEY_F9;
	case SDLK_F10:
		return KEY_F10;
	case SDLK_F11:
		return KEY_F11;
	case SDLK_F12:
		return KEY_F12;
	}

	/* The player hit...something, I guess. */
	return -1;
}

void HandleWindowEvent(SDL_Event event) {
	event_t d_event;

	switch(event.window.event) {
		case SDL_WINDOWEVENT_CLOSE:
			/* TODO: Put a common quit function together. */
			d_event.type = ev_keydown;
			d_event.data1 = KEY_F10;
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
			/* TODO: Put a common menu/pause function together. */
			d_event.type = ev_keydown;
			d_event.data1 = KEY_ESCAPE;
			break;
		default:
			return;
	}

	D_PostEvent(&d_event);
}

void I_GetEvent(void) {
	event_t d_event;
	SDL_Event sdl_event;
	Uint32 button_mask;
	SDL_bool dispatch_event;

	/* TODO: Add gamepad/joystick support. */
	while(SDL_PollEvent(&sdl_event)) {
		dispatch_event = SDL_FALSE;

		switch(sdl_event.type) {
			case SDL_KEYDOWN:
				d_event.type = ev_keydown;
				break;
			case SDL_KEYUP:
				d_event.type = ev_keyup;
				break;
			case SDL_MOUSEMOTION:
				button_mask = SDL_GetMouseState(NULL, NULL);

				d_event.type = ev_mouse;
				d_event.data1 = button_mask & SDL_BUTTON(SDL_BUTTON_LEFT) ? 1 : 0 |
					button_mask & SDL_BUTTON(SDL_BUTTON_RIGHT) ? 2 : 0 |
					button_mask & SDL_BUTTON(SDL_BUTTON_MIDDLE) ? 4 : 0;
				d_event.data2 = sdl_event.motion.xrel << mouse_sensitivity_multiplier_x;
				d_event.data3 = -sdl_event.motion.yrel << mouse_sensitivity_multiplier_y;

				dispatch_event = SDL_TRUE;
				break;
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				button_mask = SDL_GetMouseState(NULL, NULL);

				d_event.type = ev_mouse;
				d_event.data1 = button_mask & SDL_BUTTON(SDL_BUTTON_LEFT) ? 1 : 0 |
					button_mask & SDL_BUTTON(SDL_BUTTON_RIGHT) ? 2 : 0 |
					button_mask & SDL_BUTTON(SDL_BUTTON_MIDDLE) ? 4 : 0;
				d_event.data2 = d_event.data3 = 0;

				dispatch_event = SDL_TRUE;
				break;
			case SDL_WINDOWEVENT:
				HandleWindowEvent(sdl_event);
				return;
			default:
				continue;
		}

		/* Handle keyboard events. */
		if((d_event.type == ev_keydown || d_event.type == ev_keyup) && !dispatch_event)
			if((d_event.data1 = MapSDLKey(sdl_event.key.keysym)) != -1)
				dispatch_event = SDL_TRUE;

		/* Dispatch events, if applicable. */
		if(dispatch_event) D_PostEvent(&d_event);
	}
}
