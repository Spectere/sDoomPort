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
//	Main loop menu stuff.
//	Random number LUT.
//	Default Config File.
//	PCX Screenshots.
//
//-----------------------------------------------------------------------------

#include "m_bbox.h"

void M_ClearBox(fixed_t* box) {
	box[BOXTOP] = box[BOXRIGHT] = MININT;
	box[BOXBOTTOM] = box[BOXLEFT] = MAXINT;
}

void M_AddToBox(fixed_t* box, fixed_t x, fixed_t y) {
	if(x < box[BOXLEFT])
		box[BOXLEFT] = x;
	else if(x > box[BOXRIGHT])
		box[BOXRIGHT] = x;
	if(y < box[BOXBOTTOM])
		box[BOXBOTTOM] = y;
	else if(y > box[BOXTOP])
		box[BOXTOP] = y;
}
