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
//	Put all global tate variables here.
//
//-----------------------------------------------------------------------------

#include <SDL_stdinc.h>
#include "doomstat.h"

// Game Mode - identify IWAD as shareware, retail etc.
GameMode_t gamemode = indetermined;
GameMission_t gamemission = doom;
SDL_bool bfg_edition = SDL_FALSE;

// Language.
Language_t language = english;

// Set if homebrew PWAD stuff has been added.
SDL_bool modifiedgame;
