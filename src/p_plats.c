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
//	Plats (i.e. elevator platforms) code, raising/lowering.
//
//-----------------------------------------------------------------------------

#include <SDL_stdinc.h>

#include "i_system.h"
#include "z_zone.h"
#include "m_random.h"

#include "doomdef.h"
#include "p_local.h"

#include "s_sound.h"

// State.
#include "doomstat.h"
#include "r_state.h"

// Data.
#include "sounds.h"


list activeplats;


//
// Move a plat up and down
//
void T_PlatRaise(think_t* th) {
	result_e res;
	plat_t* plat = th->object;

	switch(plat->status) {
		case up:
			res = T_MovePlane(plat->sector,
			                  plat->speed,
			                  plat->high,
			                  plat->crush, 0, 1);

			if(plat->type == raiseAndChange
			   || plat->type == raiseToNearestAndChange) {
				if(!(leveltime & 7))
					S_StartSound((mobj_t *)&plat->sector->soundorg,
					             sfx_stnmov);
			}


			if(res == crushed && (!plat->crush)) {
				plat->count = plat->wait;
				plat->status = down;
				S_StartSound((mobj_t *)&plat->sector->soundorg,
				             sfx_pstart);
			} else {
				if(res == pastdest) {
					plat->count = plat->wait;
					plat->status = waiting;
					S_StartSound((mobj_t *)&plat->sector->soundorg,
					             sfx_pstop);

					switch(plat->type) {
						case blazeDWUS:
						case downWaitUpStay:
							P_RemoveActivePlat(plat);
							break;

						case raiseAndChange:
						case raiseToNearestAndChange:
							P_RemoveActivePlat(plat);
							break;

						default:
							break;
					}
				}
			}
			break;

		case down:
			res = T_MovePlane(plat->sector, plat->speed, plat->low, SDL_FALSE, 0, -1);

			if(res == pastdest) {
				plat->count = plat->wait;
				plat->status = waiting;
				S_StartSound((mobj_t *)&plat->sector->soundorg, sfx_pstop);
			}
			break;

		case waiting:
			if(!--plat->count) {
				if(plat->sector->floorheight == plat->low)
					plat->status = up;
				else
					plat->status = down;
				S_StartSound((mobj_t *)&plat->sector->soundorg, sfx_pstart);
			}
		case in_stasis:
			break;
	}
}


//
// Do Platforms
//  "amount" is only used for SOME platforms.
//
int EV_DoPlat(line_t* line, plattype_e type, int amount) {
	plat_t* plat;
	int secnum;
	int rtn;
	sector_t* sec;

	secnum = -1;
	rtn = 0;


	//	Activate all <type> plats that are in_stasis
	switch(type) {
		case perpetualRaise:
			P_ActivateInStasis(line->tag);
			break;

		default:
			break;
	}

	while((secnum = P_FindSectorFromLineTag(line, secnum)) >= 0) {
		sec = &sectors[secnum];

		if(sec->specialdata)
			continue;

		// Find lowest & highest floors around sector
		rtn = 1;
		plat = list_insert_last(&activeplats);
		plat->thinker = P_NewThinker(plat);

		plat->type = type;
		plat->sector = sec;
		plat->sector->specialdata = plat;
		plat->thinker->action.acp1 = (actionf_p1) T_PlatRaise;
		plat->crush = SDL_FALSE;
		plat->tag = line->tag;

		switch(type) {
			case raiseToNearestAndChange:
				plat->speed = PLATSPEED / 2;
				sec->floorpic = sides[line->sidenum[0]].sector->floorpic;
				plat->high = P_FindNextHighestFloor(sec, sec->floorheight);
				plat->wait = 0;
				plat->status = up;
				// NO MORE DAMAGE, IF APPLICABLE
				sec->special = 0;

				S_StartSound((mobj_t *)&sec->soundorg, sfx_stnmov);
				break;

			case raiseAndChange:
				plat->speed = PLATSPEED / 2;
				sec->floorpic = sides[line->sidenum[0]].sector->floorpic;
				plat->high = sec->floorheight + amount * FRACUNIT;
				plat->wait = 0;
				plat->status = up;

				S_StartSound((mobj_t *)&sec->soundorg, sfx_stnmov);
				break;

			case downWaitUpStay:
				plat->speed = PLATSPEED * 4;
				plat->low = P_FindLowestFloorSurrounding(sec);

				if(plat->low > sec->floorheight)
					plat->low = sec->floorheight;

				plat->high = sec->floorheight;
				plat->wait = 35 * PLATWAIT;
				plat->status = down;
				S_StartSound((mobj_t *)&sec->soundorg, sfx_pstart);
				break;

			case blazeDWUS:
				plat->speed = PLATSPEED * 8;
				plat->low = P_FindLowestFloorSurrounding(sec);

				if(plat->low > sec->floorheight)
					plat->low = sec->floorheight;

				plat->high = sec->floorheight;
				plat->wait = 35 * PLATWAIT;
				plat->status = down;
				S_StartSound((mobj_t *)&sec->soundorg, sfx_pstart);
				break;

			case perpetualRaise:
				plat->speed = PLATSPEED;
				plat->low = P_FindLowestFloorSurrounding(sec);

				if(plat->low > sec->floorheight)
					plat->low = sec->floorheight;

				plat->high = P_FindHighestFloorSurrounding(sec);

				if(plat->high < sec->floorheight)
					plat->high = sec->floorheight;

				plat->wait = 35 * PLATWAIT;
				plat->status = P_Random() & 1;

				S_StartSound((mobj_t *)&sec->soundorg, sfx_pstart);
				break;
		}
	}
	return rtn;
}


void P_ActivateInStasis(int tag) {
	plat_t* plat;

	LIST_ITERATE(plat, &activeplats) {
		if(plat->tag == tag && plat->status == in_stasis) {
			plat->status = plat->oldstatus;
			plat->thinker->action.acp1 = (actionf_p1)T_PlatRaise;
		}
	}
}

void EV_StopPlat(line_t* line) {
	plat_t* plat;

	LIST_ITERATE(plat, &activeplats) {
		if(plat->status != in_stasis && plat->tag == line->tag) {
			plat->oldstatus = plat->status;
			plat->status = in_stasis;
			plat->thinker->action.acv = NULL;
		}
	}
}

void P_RemoveActivePlat(plat_t* plat) {
	plat_t* cur;

	LIST_ITERATE(cur, &activeplats) {
		if(plat == cur) {
			cur->sector->specialdata = NULL;
			P_RemoveThinker(cur->thinker);
			list_delete_current(&activeplats);

			return;
		}
	}
	I_Error("P_RemoveActivePlat: can't find plat!");
}

void T_InitPlats(void) {
	if(activeplats.data_size == 0)
		list_new(&activeplats, sizeof(plat_t));
	else
		list_clear(&activeplats);
}
