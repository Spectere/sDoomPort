//-----------------------------------------------------------------------------
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 2017 by Ian Burgmyer
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	WAD I/O functions.
//
//-----------------------------------------------------------------------------


#ifndef __W_WAD__
#define __W_WAD__

#include <SDL.h>
#include "x_memmgr.h"

//
// TYPES
//
typedef struct {
	// Should be "IWAD" or "PWAD".
	char identification[4];
	Sint32 numlumps;
	Sint32 infotableofs;

} wadinfo_t;


typedef struct {
	Sint32 filepos;
	Sint32 size;
	char name[8];

} filelump_t;

//
// WADFILE I/O related stuff.
//
typedef struct {
	char name[8];
	SDL_RWops* handle;
	Sint32 position;
	Sint32 size;
} lumpinfo_t;


extern void** lumpcache;
extern lumpinfo_t* lumpinfo;
extern int numlumps;

void W_InitMultipleFiles(char** filenames);
void W_Reload(void);

int W_CheckNumForName(char* name);
int W_GetNumForName(char* name);

int W_LumpLength(int lump);
void W_ReadLump(int lump, void* dest);

void* W_CacheLumpNum(int lump, xmemtag_t tag);
void* W_CacheLumpName(char* name, xmemtag_t tag);

#endif
