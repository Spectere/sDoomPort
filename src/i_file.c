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
//
// Miscellaneous file handling code.
//    
//-----------------------------------------------------------------------------

#include <SDL.h>

SDL_bool I_FileExists(char* filename) {
	SDL_RWops* handle;

	if((handle = SDL_RWFromFile(filename, "r")) == NULL)
		return SDL_FALSE;

	SDL_RWclose(handle);
	return SDL_TRUE;
}
