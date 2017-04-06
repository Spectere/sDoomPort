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
//    
//-----------------------------------------------------------------------------


#ifndef __M_MISC__
#define __M_MISC__

#include <SDL_stdinc.h>

#include "doomtype.h"
//
// MISC
//

SDL_bool M_WriteFile(char const* name, void* source, int length);
int M_ReadFile(char const* name, Uint8** buffer);
void M_ScreenShot(void);
void M_LoadDefaults(void);
void M_SaveDefaults(void);
int M_DrawText(int x, int y, SDL_bool direct, char* string);

#endif
