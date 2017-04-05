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
//	Teleportation.
//
//-----------------------------------------------------------------------------

#include "s_sound.h"

#include "p_local.h"


// Data.
#include "sounds.h"

// State.
#include "r_state.h"


//
// TELEPORTATION
//
int
EV_Teleport
(line_t* line,
 int side,
 mobj_t* thing) {
	int i;
	int tag;
	mobj_t* m;
	mobj_t* fog;
	unsigned an;
	think_t* thinker;
	sector_t* sector;
	fixed_t oldx;
	fixed_t oldy;
	fixed_t oldz;

	// don't teleport missiles
	if(thing->flags & MF_MISSILE)
		return 0;

	// Don't teleport if hit back of line,
	//  so you can get out of teleporter.
	if(side == 1)
		return 0;


	tag = line->tag;
	for(i = 0; i < numsectors; i++) {
		if(sectors[i].tag == tag) {
			LIST_ITERATE(thinker, &thinkers) {
				// not a mobj
				if(thinker->action.acp1 != (actionf_p1)P_MobjThinker)
					continue;

				m = thinker->object;

				// not a teleportman
				if(m->type != MT_TELEPORTMAN)
					continue;

				sector = m->subsector->sector;
				// wrong sector
				if(sector - sectors != i)
					continue;

				oldx = thing->x;
				oldy = thing->y;
				oldz = thing->z;

				if(!P_TeleportMove(thing, m->x, m->y))
					return 0;

				thing->z = thing->floorz; //fixme: not needed?
				if(thing->player)
					thing->player->viewz = thing->z + thing->player->viewheight;

				// spawn teleport fog at source and destination
				fog = P_SpawnMobj(oldx, oldy, oldz, MT_TFOG);
				S_StartSound(fog, sfx_telept);
				an = m->angle >> ANGLETOFINESHIFT;
				fog = P_SpawnMobj(m->x + 20 * finecosine[an], m->y + 20 * finesine[an]
				                  , thing->z, MT_TFOG);

				// emit sound, where?
				S_StartSound(fog, sfx_telept);

				// don't move for a bit
				if(thing->player)
					thing->reactiontime = 18;

				thing->angle = m->angle;
				thing->momx = thing->momy = thing->momz = 0;
				return 1;
			}
		}
	}
	return 0;
}
