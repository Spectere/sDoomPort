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

#include <stdlib.h>
#include <string.h>
#include <SDL_stdinc.h>

#include "i_system.h"
#include "d_event.h"
#include "d_net.h"
#include "m_argv.h"

#include "doomstat.h"

#include "i_net.h"

void (*netget)(void);
void (*netsend)(void);

//
// I_InitNetwork
//
void I_InitNetwork(void) {
	int i;

	doomcom = malloc(sizeof (*doomcom));
	memset(doomcom, 0, sizeof(*doomcom));

	// set up for network
	i = M_CheckParm("-dup");
	if(i && i < myargc - 1) {
		doomcom->ticdup = myargv[i + 1][0] - '0';
		if(doomcom->ticdup < 1)
			doomcom->ticdup = 1;
		if(doomcom->ticdup > 9)
			doomcom->ticdup = 9;
	} else
		doomcom->ticdup = 1;

	if(M_CheckParm("-extratic"))
		doomcom->extratics = 1;
	else
		doomcom->extratics = 0;

	// parse network game options,
	//  -net <consoleplayer> <host> <host> ...
	i = M_CheckParm("-net");
	if(!i) {
		// single player game
		netgame = SDL_FALSE;
		doomcom->id = DOOMCOM_ID;
		doomcom->numplayers = doomcom->numnodes = 1;
		doomcom->deathmatch = SDL_FALSE;
		doomcom->consoleplayer = 0;
		return;
	}

	netsend = NULL;
	netget = NULL;
	netgame = SDL_TRUE;

	// parse player number and host list
	doomcom->consoleplayer = myargv[i + 1][0] - '1';

	doomcom->numnodes = 1; // this node for sure

	doomcom->id = DOOMCOM_ID;
	doomcom->numplayers = doomcom->numnodes;

	// TODO: build message to receive
}


void I_NetCmd(void) {
	if(doomcom->command == CMD_SEND) {
		if(netsend != NULL) netsend();
	} else if(doomcom->command == CMD_GET) {
		if(netget != NULL) netget();
	} else
		I_Error("Bad net cmd: %i\n", doomcom->command);
}
