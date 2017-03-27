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

#include "stdlib.h"

#include "doomtype.h"
#include "i_system.h"

#ifdef __GNUG__
#pragma implementation "m_fixed.h"
#endif
#include "m_fixed.h"


// Fixme. __USE_C_FIXED__ or something.

fixed_t
FixedMul
(fixed_t a,
 fixed_t b) {
	return ((long long) a * (long long) b) >> FRACBITS;
}


//
// FixedDiv, C version.
//

fixed_t
FixedDiv
(fixed_t a,
 fixed_t b) {
	if((abs(a) >> 14) >= abs(b))
		return (a ^ b) < 0 ? MININT : MAXINT;
	return FixedDiv2(a, b);
}


fixed_t
FixedDiv2
(fixed_t a,
 fixed_t b) {
#if 0
    long long c;
    c = ((long long)a<<16) / ((long long)b);
    return (fixed_t) c;
#endif

	double c;

	c = ((double)a) / ((double)b) * FRACUNIT;

	if(c >= 2147483648.0 || c < -2147483648.0)
		I_Error("FixedDiv: divide by zero");
	return (fixed_t) c;
}
