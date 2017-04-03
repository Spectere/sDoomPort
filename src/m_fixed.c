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
//	Fixed point implementation.
//
//-----------------------------------------------------------------------------

#include <SDL_stdinc.h>

#include "stdlib.h"

#include "doomtype.h"
#include "i_system.h"

#include "m_fixed.h"

// Fixme. __USE_C_FIXED__ or something.

fixed_t
FixedMul
(fixed_t a,
 fixed_t b) {
	return (Sint64) a * (Sint64) b >> FRACBITS;
}


//
// FixedDiv, C version.
//

fixed_t
FixedDiv
(fixed_t a,
 fixed_t b) {
	Sint64 c;
	if(abs(a) >> 14 >= abs(b))
		return (a ^ b) < 0 ? MININT : MAXINT;
	c = ((Sint64)a << FRACBITS) / (Sint64)b;
	return (fixed_t)c;
}
