//-----------------------------------------------------------------------------
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
// DESCRIPTION:
//	Archiving: SaveGame I/O.
//	Thinker, Ticker.
//
//-----------------------------------------------------------------------------

#include "z_zone.h"
#include "p_local.h"
#include "m_list.h"

#include "doomstat.h"


int leveltime;

//
// THINKERS
// All thinkers should be allocated by Z_Malloc
// so they can be operated on uniformly.
// The actual structures will vary in size,
// but the first element must be thinker_t.
//


// Both the head and tail of the thinker list.
list thinkers;


//
// P_InitThinkers
//
void P_InitThinkers(void) {
	list_new(&thinkers, sizeof(think_t));
}


//
// P_NewThinker
// Allocates and returns a new thinker.
//
void* P_NewThinker(void* object) {
	think_t* new_thinker = list_insert_last(&thinkers);
	new_thinker->object = object;
	return new_thinker;
}


//
// P_RemoveThinker
// Deallocate immediately to prevent thinkers from being
// prematurely freed by other linked list functions.
//
void P_RemoveThinker(think_t* thinker) {
	think_t* t;
	LIST_ITERATE(t, &thinkers) {
		if(t == thinker) {
			list_delete_current(&thinkers);
			return;
		}
	}
}


//
// P_RunThinkers
// This got a whole lot simpler.
//
void P_RunThinkers(void) {
	think_t* thinker;

	LIST_ITERATE(thinker, &thinkers) {
		if(thinker->action.acp1)
			thinker->action.acp1(thinker);
	}
}


//
// P_Ticker
//

void P_Ticker(void) {
	int i;

	// run the tic
	if(paused)
		return;

	// pause if in menu and at least one tic has been run
	if(!netgame
	   && menuactive
	   && !demoplayback
	   && players[consoleplayer].viewz != 1) {
		return;
	}


	for(i = 0; i < MAXPLAYERS; i++)
		if(playeringame[i])
			P_PlayerThink(&players[i]);

	P_RunThinkers();
	P_UpdateSpecials();
	P_RespawnSpecials();

	// for par times
	leveltime++;
}
