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
// DESCRIPTION:
//	The new memory management system, a replacement for the old zone
//  memory allocation system.
//
//-----------------------------------------------------------------------------

#ifndef __X_MEMMGR__
#define __X_MEMMGR__

#include <SDL_stdinc.h>

typedef enum {
	XTag_Static = 0x00,
	XTag_Sound  = 0x01,
	XTag_Music  = 0x02,

	XTag_Level  = 0x40,

	XTag_Purge  = 0x80,
	XTag_Cache  = 0x81
} xmemtag_t;

void X_ChangeTag(void* ptr, xmemtag_t newtag);
void X_Free(void* ptr);
void X_FreeTags(xmemtag_t start, xmemtag_t end);
void X_Init(void);
void* X_Malloc(size_t size, xmemtag_t tag);

#endif /* __X_MEMMGR__ */
