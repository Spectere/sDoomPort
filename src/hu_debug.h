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

#ifndef __HUDEBUG_H__
#define __HUDEBUG_H__

typedef struct {
	char* desc;
	int cur;
	int max;
	int(*update)(void);
} debug_stat_t;

extern debug_stat_t debug_stat[];
int debug_count;

void HU_DebugInit(void);
void HU_DebugResetStats(void);
void HU_DebugUpdate(void);

#endif /* __HUDEBUG_H__ */
