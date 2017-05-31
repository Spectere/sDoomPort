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
// DESCRIPTION:  SDPBLAH display.
//
//-----------------------------------------------------------------------------

#include <stdlib.h>

#include "hu_debug.h"
#include "hu_stuff.h"

/* Headers for debug stats. */
#include "p_local.h"
#include "p_spec.h"
#include "p_tick.h"
#include "r_bsp.h"
#include "r_plane.h"

debug_stat_t debug_stat[] = {
	{  0,   "vis", 0, 0, R_PlaneCount },
	{  1,   "seg", 0, 0, R_SegCount },
	{  2, "think", 0, 0, P_ThinkerCount },
	{  3,  "plat", 0, 0, T_PlatCount },
	{  4,  "mobj", 0, 0, P_MobjCount },
	{ -1,    NULL, 0, 0, NULL }
};

static int GetDebugCount(void) {
	int i = -1;

	// ReSharper disable once CppPossiblyErroneousEmptyStatements
	while(debug_stat[++i].update != NULL);

	return i;
}

static void UpdateMax(void) {
	int i;
	for(i = 0; i < debug_count; i++) {
		if(debug_stat[i].max < debug_stat[i].cur)
			debug_stat[i].max = debug_stat[i].cur;
	}
}

void HU_DebugInit(void) {
	debug_count = GetDebugCount();
}

void HU_DebugResetStats(void) {
	int i;
	for(i = 0; i < debug_count; i++)
		debug_stat[i].cur = debug_stat[i].max = 0;
}

void HU_DebugUpdate(void) {
	int i;
	for(i = 0; i < debug_count; i++)
		debug_stat[i].cur = debug_stat[i].update();

	UpdateMax();
}
