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
//	Handles WAD file header, directory, lump I/O.
//
//-----------------------------------------------------------------------------

#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>
#include <SDL.h>

#include "doomdef.h"
#include "doomtype.h"
#include "i_system.h"
#include "z_zone.h"

#include "w_wad.h"
#include "x_memmgr.h"


//
// GLOBALS
//

// Location of each lump on disk.
lumpinfo_t* lumpinfo;
int numlumps;

void** lumpcache;


//#define strcmpi	strcasecmp

void d_strupr(char* s) {
	while(*s) {
		*s = toupper(*s);
		s++;
	}
}

void ExtractFileBase(char* path, char* dest) {
	char* src;
	int length;

	src = path + strlen(path) - 1;

	// back up until a \ or the start
	while(src != path
	      && *(src - 1) != '\\'
	      && *(src - 1) != '/') { }

	// copy up to eight characters
	memset(dest, 0, 8);
	length = 0;

	while(*src && *src != '.') {
		if(++length == 9)
			I_Error("Filename base of %s >8 chars", path);

		*dest++ = toupper((int)*src++);
	}
}


//
// LUMP BASED ROUTINES.
//

//
// W_AddFile
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
//
// If filename starts with a tilde, the file is handled
//  specially to allow map reloads.
// But: the reload feature is a fragile hack...

int reloadlump;
char* reloadname;


void W_AddFile(char* filename) {
	wadinfo_t header;
	lumpinfo_t* lump_p;
	unsigned i;
	SDL_RWops* handle;
	int length;
	int startlump;
	filelump_t* fileinfo;
	filelump_t singleinfo;
	SDL_RWops* storehandle;

	// open the file and add to directory

	// handle reload indicator.
	if(filename[0] == '~') {
		filename++;
		reloadname = filename;
		reloadlump = numlumps;
	}

	if((handle = SDL_RWFromFile(filename, "rb")) == NULL) {
		printf(" couldn't open %s\n", filename);
		return;
	}

	printf(" adding %s\n", filename);
	startlump = numlumps;

	if(_stricmp(filename + strlen(filename) - 3, "wad")) {
		// single lump file
		fileinfo = &singleinfo;
		singleinfo.filepos = 0;
		singleinfo.size = SDL_SwapLE32(SDL_RWsize(handle));
		ExtractFileBase(filename, singleinfo.name);
		numlumps++;
	} else {
		// WAD file
		SDL_RWread(handle, &header, sizeof(header), 1);
		if(strncmp(header.identification, "IWAD", 4)) {
			// Homebrew levels?
			if(strncmp(header.identification, "PWAD", 4)) {
				I_Error("Wad file %s doesn't have IWAD "
				        "or PWAD id\n", filename);
			}

			// ???modifiedgame = true;		
		}
		header.numlumps = SDL_SwapLE32(header.numlumps);
		header.infotableofs = SDL_SwapLE32(header.infotableofs);
		length = header.numlumps * sizeof(filelump_t);
		fileinfo = alloca(length);
		SDL_RWseek(handle, header.infotableofs, RW_SEEK_SET);
		SDL_RWread(handle, fileinfo, length, 1);
		numlumps += header.numlumps;
	}


	// Fill in lumpinfo
	lumpinfo = realloc(lumpinfo, numlumps * sizeof(lumpinfo_t));

	if(!lumpinfo)
		I_Error("Couldn't realloc lumpinfo");

	lump_p = &lumpinfo[startlump];

	storehandle = reloadname ? NULL : handle;

	for(i = startlump; i < numlumps; i++ , lump_p++ , fileinfo++) {
		lump_p->handle = storehandle;
		lump_p->position = SDL_SwapLE32(fileinfo->filepos);
		lump_p->size = SDL_SwapLE32(fileinfo->size);
		strncpy(lump_p->name, fileinfo->name, 8);
	}

	if(reloadname)
		SDL_RWclose(handle);
}


//
// W_Reload
// Flushes any of the reloadable lumps in memory
//  and reloads the directory.
//
void W_Reload(void) {
	wadinfo_t header;
	int lumpcount;
	lumpinfo_t* lump_p;
	unsigned i;
	SDL_RWops* handle;
	int length;
	filelump_t* fileinfo;

	if(!reloadname)
		return;

	if((handle = SDL_RWFromFile(reloadname, "rb")) == NULL)
		I_Error("W_Reload: couldn't open %s", reloadname);

	SDL_RWread(handle, &header, sizeof(header), 1);
	lumpcount = SDL_SwapLE32(header.numlumps);
	header.infotableofs = SDL_SwapLE32(header.infotableofs);
	length = lumpcount * sizeof(filelump_t);
	fileinfo = alloca(length);
	SDL_RWseek(handle, header.infotableofs, RW_SEEK_SET);
	SDL_RWread(handle, fileinfo, length, 1);

	// Fill in lumpinfo
	lump_p = &lumpinfo[reloadlump];

	for(i = reloadlump;
	    i < reloadlump + lumpcount;
	    i++ , lump_p++ , fileinfo++) {
		if(lumpcache[i])
			Z_Free(lumpcache[i]);

		lump_p->position = SDL_SwapLE32(fileinfo->filepos);
		lump_p->size = SDL_SwapLE32(fileinfo->size);
	}

	SDL_RWclose(handle);
}


//
// W_InitMultipleFiles
// Pass a null terminated list of files to use.
// All files are optional, but at least one file
//  must be found.
// Files with a .wad extension are idlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
// Lump names can appear multiple times.
// The name searcher looks backwards, so a later file
//  does override all earlier ones.
//
void W_InitMultipleFiles(char** filenames) {
	int size;

	// open all the files, load headers, and count lumps
	numlumps = 0;

	// will be realloced as lumps are added
	lumpinfo = malloc(1);

	for(; *filenames; filenames++)
		W_AddFile(*filenames);

	if(!numlumps)
		I_Error("W_InitFiles: no files found");

	// set up caching
	size = numlumps * sizeof(*lumpcache);
	lumpcache = malloc(size);

	if(!lumpcache)
		I_Error("Couldn't allocate lumpcache");

	memset(lumpcache, 0, size);
}


//
// W_InitFile
// Just initialize from a single file.
//
void W_InitFile(char* filename) {
	char* names[2];

	names[0] = filename;
	names[1] = NULL;
	W_InitMultipleFiles(names);
}


//
// W_NumLumps
//
int W_NumLumps(void) {
	return numlumps;
}


//
// W_CheckNumForName
// Returns -1 if name not found.
//

int W_CheckNumForName(char* name) {
	union {
		char s[9];
		int x[2];

	} name8;

	int v1;
	int v2;
	lumpinfo_t* lump_p;

	// make the name into two integers for easy compares
	strncpy(name8.s, name, 8);

	// in case the name was a fill 8 chars
	name8.s[8] = 0;

	// case insensitive
	d_strupr(name8.s);

	v1 = name8.x[0];
	v2 = name8.x[1];


	// scan backwards so patch lump files take precedence
	lump_p = lumpinfo + numlumps;

	while(lump_p-- != lumpinfo) {
		if(*(int *)lump_p->name == v1
		   && *(int *)&lump_p->name[4] == v2) {
			return lump_p - lumpinfo;
		}
	}

	// TFB. Not found.
	return -1;
}


//
// W_GetNumForName
// Calls W_CheckNumForName, but bombs out if not found.
//
int W_GetNumForName(char* name) {
	int i;

	i = W_CheckNumForName(name);

	if(i == -1)
		I_Error("W_GetNumForName: %s not found!", name);

	return i;
}


//
// W_LumpLength
// Returns the buffer size needed to load the given lump.
//
int W_LumpLength(int lump) {
	if(lump >= numlumps)
		I_Error("W_LumpLength: %i >= numlumps", lump);

	return lumpinfo[lump].size;
}


//
// W_ReadLump
// Loads the lump into the given buffer,
//  which must be >= W_LumpLength().
//
void W_ReadLump(int lump, void* dest) {
	int c;
	lumpinfo_t* l;
	SDL_RWops* handle;

	if(lump >= numlumps)
		I_Error("W_ReadLump: %i >= numlumps", lump);

	l = lumpinfo + lump;

	// ??? I_BeginRead ();

	if(l->handle == NULL) {
		// reloadable file, so use open / read / close
		if((handle = SDL_RWFromFile(reloadname, "rb")) == NULL)
			I_Error("W_ReadLump: couldn't open %s", reloadname);
	} else
		handle = l->handle;

	SDL_RWseek(handle, l->position, RW_SEEK_SET);
	c = SDL_RWread(handle, dest, l->size, 1);

	if(c == 0)
		I_Error("W_ReadLump: error reading lump %i",
		        lump);

	if(l->handle == NULL)
		SDL_RWclose(handle);

	// ??? I_EndRead ();
}


//
// W_CacheLumpNum
//
void* W_CacheLumpNum(int lump, xmemtag_t tag) {
	if((unsigned)lump >= numlumps)
		I_Error("W_CacheLumpNum: %i >= numlumps", lump);

	if(!lumpcache[lump]) {
		// read the lump in

		//printf ("cache miss on lump %i\n",lump);
		lumpcache[lump] = X_Malloc(W_LumpLength(lump), tag);
		W_ReadLump(lump, lumpcache[lump]);
	} else {
		//printf ("cache hit on lump %i\n",lump);
		/* TODO: XMEM: Reimplement me. */
		/* Z_ChangeTag(lumpcache[lump], tag); */
	}

	return lumpcache[lump];
}


//
// W_CacheLumpName
//
void* W_CacheLumpName(char* name, xmemtag_t tag) {
	return W_CacheLumpNum(W_GetNumForName(name), tag);
}


//
// W_Profile
//
int info[2500][10];
int profilecount;

void W_Profile(void) {
	int i;
	memblock_t* block;
	void* ptr;
	char ch;
	FILE* f;
	int j;
	char name[9];


	for(i = 0; i < numlumps; i++) {
		ptr = lumpcache[i];
		if(!ptr) {
			continue;
		} else {
			block = (memblock_t *) ((Uint8 *)ptr - sizeof(memblock_t));
			if(block->tag < PU_PURGELEVEL)
				ch = 'S';
			else
				ch = 'P';
		}
		info[i][profilecount] = ch;
	}
	profilecount++;

	f = fopen("waddump.txt", "w");
	name[8] = 0;

	for(i = 0; i < numlumps; i++) {
		memcpy(name, lumpinfo[i].name, 8);

		for(j = 0; j < 8; j++)
			if(!name[j])
				break;

		for(; j < 8; j++)
			name[j] = ' ';

		fprintf(f, "%s ", name);

		for(j = 0; j < profilecount; j++)
			fprintf(f, "    %c", info[i][j]);

		fprintf(f, "\n");
	}
	fclose(f);
}
