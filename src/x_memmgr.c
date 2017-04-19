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

#include <SDL_stdinc.h>
#include <stdlib.h>

#include "m_list.h"
#include "x_memmgr.h"
#include "i_system.h"

static list xmem;

typedef struct {
	xmemtag_t tag;
	size_t size;
	void* data;
} xmementry_t;

void X_ChangeTag(void* ptr, xmemtag_t newtag) {
	xmementry_t* mem;

	LIST_ITERATE(mem, &xmem) {
		if(mem->data == ptr) {
			mem->tag = newtag;
			return;
		}
	}
}

void X_Free(void* ptr) {
	xmementry_t* mem;

	LIST_ITERATE(mem, &xmem) {
		if(mem->data == ptr) {
			free(mem->data);
			list_delete_current(&xmem);
			return;
		}
	}

	I_Error("X_Free: Tried to free unallocated memory!");
}

void X_FreeTags(xmemtag_t start, xmemtag_t end) {
	xmementry_t* mem;

	LIST_ITERATE(mem, &xmem) {
		if(mem->tag >= start && mem->tag <= end) {
			free(mem->data);
			list_delete_current(&xmem);
		}
	}
}

void X_Init(void) {
	list_new(&xmem, sizeof(xmementry_t));
}

void* X_Malloc(size_t size, xmemtag_t tag) {
	xmementry_t* mem;

	mem = list_insert_last(&xmem);
	mem->size = size;
	mem->tag = tag;
	mem->data = malloc(size);

	return mem->data;
}
