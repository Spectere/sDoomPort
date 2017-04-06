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
//	Created by a sound utility.
//	Kept as a sample, DOOM2 sounds.
//
//-----------------------------------------------------------------------------

#include <SDL_stdinc.h>

#include "doomtype.h"
#include "sounds.h"

//
// Information about all the music
//

musicinfo_t S_music[] = {
	{0},
	{"e1m1", 0},
	{"e1m2", 0},
	{"e1m3", 0},
	{"e1m4", 0},
	{"e1m5", 0},
	{"e1m6", 0},
	{"e1m7", 0},
	{"e1m8", 0},
	{"e1m9", 0},
	{"e2m1", 0},
	{"e2m2", 0},
	{"e2m3", 0},
	{"e2m4", 0},
	{"e2m5", 0},
	{"e2m6", 0},
	{"e2m7", 0},
	{"e2m8", 0},
	{"e2m9", 0},
	{"e3m1", 0},
	{"e3m2", 0},
	{"e3m3", 0},
	{"e3m4", 0},
	{"e3m5", 0},
	{"e3m6", 0},
	{"e3m7", 0},
	{"e3m8", 0},
	{"e3m9", 0},
	{"inter", 0},
	{"intro", 0},
	{"bunny", 0},
	{"victor", 0},
	{"introa", 0},
	{"runnin", 0},
	{"stalks", 0},
	{"countd", 0},
	{"betwee", 0},
	{"doom", 0},
	{"the_da", 0},
	{"shawn", 0},
	{"ddtblu", 0},
	{"in_cit", 0},
	{"dead", 0},
	{"stlks2", 0},
	{"theda2", 0},
	{"doom2", 0},
	{"ddtbl2", 0},
	{"runni2", 0},
	{"dead2", 0},
	{"stlks3", 0},
	{"romero", 0},
	{"shawn2", 0},
	{"messag", 0},
	{"count2", 0},
	{"ddtbl3", 0},
	{"ampie", 0},
	{"theda3", 0},
	{"adrian", 0},
	{"messg2", 0},
	{"romer2", 0},
	{"tense", 0},
	{"shawn3", 0},
	{"openin", 0},
	{"evil", 0},
	{"ultima", 0},
	{"read_m", 0},
	{"dm2ttl", 0},
	{"dm2int", 0}
};


//
// Information about all the sfx
//

sfxinfo_t S_sfx[] = {
	// S_sfx[0] needs to be a dummy for odd reasons.
	{"none", SDL_FALSE, 0, 0, -1, -1, 0},

	{"pistol", SDL_FALSE, 64, 0, -1, -1, 0},
	{"shotgn", SDL_FALSE, 64, 0, -1, -1, 0},
	{"sgcock", SDL_FALSE, 64, 0, -1, -1, 0},
	{"dshtgn", SDL_FALSE, 64, 0, -1, -1, 0},
	{"dbopn", SDL_FALSE, 64, 0, -1, -1, 0},
	{"dbcls", SDL_FALSE, 64, 0, -1, -1, 0},
	{"dbload", SDL_FALSE, 64, 0, -1, -1, 0},
	{"plasma", SDL_FALSE, 64, 0, -1, -1, 0},
	{"bfg", SDL_FALSE, 64, 0, -1, -1, 0},
	{"sawup", SDL_FALSE, 64, 0, -1, -1, 0},
	{"sawidl", SDL_FALSE, 118, 0, -1, -1, 0},
	{"sawful", SDL_FALSE, 64, 0, -1, -1, 0},
	{"sawhit", SDL_FALSE, 64, 0, -1, -1, 0},
	{"rlaunc", SDL_FALSE, 64, 0, -1, -1, 0},
	{"rxplod", SDL_FALSE, 70, 0, -1, -1, 0},
	{"firsht", SDL_FALSE, 70, 0, -1, -1, 0},
	{"firxpl", SDL_FALSE, 70, 0, -1, -1, 0},
	{"pstart", SDL_FALSE, 100, 0, -1, -1, 0},
	{"pstop", SDL_FALSE, 100, 0, -1, -1, 0},
	{"doropn", SDL_FALSE, 100, 0, -1, -1, 0},
	{"dorcls", SDL_FALSE, 100, 0, -1, -1, 0},
	{"stnmov", SDL_FALSE, 119, 0, -1, -1, 0},
	{"swtchn", SDL_FALSE, 78, 0, -1, -1, 0},
	{"swtchx", SDL_FALSE, 78, 0, -1, -1, 0},
	{"plpain", SDL_FALSE, 96, 0, -1, -1, 0},
	{"dmpain", SDL_FALSE, 96, 0, -1, -1, 0},
	{"popain", SDL_FALSE, 96, 0, -1, -1, 0},
	{"vipain", SDL_FALSE, 96, 0, -1, -1, 0},
	{"mnpain", SDL_FALSE, 96, 0, -1, -1, 0},
	{"pepain", SDL_FALSE, 96, 0, -1, -1, 0},
	{"slop", SDL_FALSE, 78, 0, -1, -1, 0},
	{"itemup", SDL_TRUE, 78, 0, -1, -1, 0},
	{"wpnup", SDL_TRUE, 78, 0, -1, -1, 0},
	{"oof", SDL_FALSE, 96, 0, -1, -1, 0},
	{"telept", SDL_FALSE, 32, 0, -1, -1, 0},
	{"posit1", SDL_TRUE, 98, 0, -1, -1, 0},
	{"posit2", SDL_TRUE, 98, 0, -1, -1, 0},
	{"posit3", SDL_TRUE, 98, 0, -1, -1, 0},
	{"bgsit1", SDL_TRUE, 98, 0, -1, -1, 0},
	{"bgsit2", SDL_TRUE, 98, 0, -1, -1, 0},
	{"sgtsit", SDL_TRUE, 98, 0, -1, -1, 0},
	{"cacsit", SDL_TRUE, 98, 0, -1, -1, 0},
	{"brssit", SDL_TRUE, 94, 0, -1, -1, 0},
	{"cybsit", SDL_TRUE, 92, 0, -1, -1, 0},
	{"spisit", SDL_TRUE, 90, 0, -1, -1, 0},
	{"bspsit", SDL_TRUE, 90, 0, -1, -1, 0},
	{"kntsit", SDL_TRUE, 90, 0, -1, -1, 0},
	{"vilsit", SDL_TRUE, 90, 0, -1, -1, 0},
	{"mansit", SDL_TRUE, 90, 0, -1, -1, 0},
	{"pesit", SDL_TRUE, 90, 0, -1, -1, 0},
	{"sklatk", SDL_FALSE, 70, 0, -1, -1, 0},
	{"sgtatk", SDL_FALSE, 70, 0, -1, -1, 0},
	{"skepch", SDL_FALSE, 70, 0, -1, -1, 0},
	{"vilatk", SDL_FALSE, 70, 0, -1, -1, 0},
	{"claw", SDL_FALSE, 70, 0, -1, -1, 0},
	{"skeswg", SDL_FALSE, 70, 0, -1, -1, 0},
	{"pldeth", SDL_FALSE, 32, 0, -1, -1, 0},
	{"pdiehi", SDL_FALSE, 32, 0, -1, -1, 0},
	{"podth1", SDL_FALSE, 70, 0, -1, -1, 0},
	{"podth2", SDL_FALSE, 70, 0, -1, -1, 0},
	{"podth3", SDL_FALSE, 70, 0, -1, -1, 0},
	{"bgdth1", SDL_FALSE, 70, 0, -1, -1, 0},
	{"bgdth2", SDL_FALSE, 70, 0, -1, -1, 0},
	{"sgtdth", SDL_FALSE, 70, 0, -1, -1, 0},
	{"cacdth", SDL_FALSE, 70, 0, -1, -1, 0},
	{"skldth", SDL_FALSE, 70, 0, -1, -1, 0},
	{"brsdth", SDL_FALSE, 32, 0, -1, -1, 0},
	{"cybdth", SDL_FALSE, 32, 0, -1, -1, 0},
	{"spidth", SDL_FALSE, 32, 0, -1, -1, 0},
	{"bspdth", SDL_FALSE, 32, 0, -1, -1, 0},
	{"vildth", SDL_FALSE, 32, 0, -1, -1, 0},
	{"kntdth", SDL_FALSE, 32, 0, -1, -1, 0},
	{"pedth", SDL_FALSE, 32, 0, -1, -1, 0},
	{"skedth", SDL_FALSE, 32, 0, -1, -1, 0},
	{"posact", SDL_TRUE, 120, 0, -1, -1, 0},
	{"bgact", SDL_TRUE, 120, 0, -1, -1, 0},
	{"dmact", SDL_TRUE, 120, 0, -1, -1, 0},
	{"bspact", SDL_TRUE, 100, 0, -1, -1, 0},
	{"bspwlk", SDL_TRUE, 100, 0, -1, -1, 0},
	{"vilact", SDL_TRUE, 100, 0, -1, -1, 0},
	{"noway", SDL_FALSE, 78, 0, -1, -1, 0},
	{"barexp", SDL_FALSE, 60, 0, -1, -1, 0},
	{"punch", SDL_FALSE, 64, 0, -1, -1, 0},
	{"hoof", SDL_FALSE, 70, 0, -1, -1, 0},
	{"metal", SDL_FALSE, 70, 0, -1, -1, 0},
	{"chgun", SDL_FALSE, 64, &S_sfx[sfx_pistol], 150, 0, 0},
	{"tink", SDL_FALSE, 60, 0, -1, -1, 0},
	{"bdopn", SDL_FALSE, 100, 0, -1, -1, 0},
	{"bdcls", SDL_FALSE, 100, 0, -1, -1, 0},
	{"itmbk", SDL_FALSE, 100, 0, -1, -1, 0},
	{"flame", SDL_FALSE, 32, 0, -1, -1, 0},
	{"flamst", SDL_FALSE, 32, 0, -1, -1, 0},
	{"getpow", SDL_FALSE, 60, 0, -1, -1, 0},
	{"bospit", SDL_FALSE, 70, 0, -1, -1, 0},
	{"boscub", SDL_FALSE, 70, 0, -1, -1, 0},
	{"bossit", SDL_FALSE, 70, 0, -1, -1, 0},
	{"bospn", SDL_FALSE, 70, 0, -1, -1, 0},
	{"bosdth", SDL_FALSE, 70, 0, -1, -1, 0},
	{"manatk", SDL_FALSE, 70, 0, -1, -1, 0},
	{"mandth", SDL_FALSE, 70, 0, -1, -1, 0},
	{"sssit", SDL_FALSE, 70, 0, -1, -1, 0},
	{"ssdth", SDL_FALSE, 70, 0, -1, -1, 0},
	{"keenpn", SDL_FALSE, 70, 0, -1, -1, 0},
	{"keendt", SDL_FALSE, 70, 0, -1, -1, 0},
	{"skeact", SDL_FALSE, 70, 0, -1, -1, 0},
	{"skesit", SDL_FALSE, 70, 0, -1, -1, 0},
	{"skeatk", SDL_FALSE, 70, 0, -1, -1, 0},
	{"radio", SDL_FALSE, 60, 0, -1, -1, 0}
};
