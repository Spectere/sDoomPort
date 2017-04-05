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
//  MapObj data. Map Objects or mobjs are actors, entities,
//  thinker, take-your-pick... anything that moves, acts, or
//  suffers state changes of more or less violent nature.
//
//-----------------------------------------------------------------------------


#ifndef __D_THINK__
#define __D_THINK__

//
// Experimental stuff.
// To compile this as "ANSI C with classes"
//  we will need to handle the various
//  action functions cleanly.
//
typedef void (*actionf_v)();
typedef void (*actionf_p1)(void*);
typedef void (*actionf_p2)(void*, void*);

typedef union {
	actionf_p1 acp1;
	actionf_v acv;
	actionf_p2 acp2;
} actionf_t;


// Historically, "think_t" is yet another
//  function pointer to a routine to handle
//  an actor.
typedef struct think_s {
	actionf_t action;
	void* object;
} think_t;

#endif
