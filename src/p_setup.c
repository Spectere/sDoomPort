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
//	Do all the WAD I/O, get map description,
//	set up initial state and misc. LUTs.
//
//-----------------------------------------------------------------------------

#include <math.h>
#include <SDL_stdinc.h>

#include "x_memmgr.h"

#include "m_bbox.h"

#include "g_game.h"

#include "i_system.h"
#include "w_wad.h"

#include "doomdef.h"
#include "p_local.h"

#include "s_sound.h"

#include "doomstat.h"
#include "x_memmgr.h"


void P_SpawnMapThing(mapthing_t* mthing);


//
// MAP related Lookup tables.
// Store VERTEXES, LINEDEFS, SIDEDEFS, etc.
//
int numvertexes;
vertex_t* vertexes;

int numsegs;
seg_t* segs;

int numsectors;
sector_t* sectors;

int numsubsectors;
subsector_t* subsectors;

int numnodes;
node_t* nodes;

int numlines;
line_t* lines;

int numsides;
side_t* sides;


// BLOCKMAP
// Created from axis aligned bounding box
// of the map, a rectangular array of
// blocks of size ...
// Used to speed up collision detection
// by spatial subdivision in 2D.
//
// Blockmap size.
int bmapwidth;
int bmapheight; // size in mapblocks
short* blockmap; // int for larger maps
// offsets in blockmap are from here
Sint16* blockmaplump;
// origin of block map
fixed_t bmaporgx;
fixed_t bmaporgy;
// for thing chains
mobj_t** blocklinks;


// REJECT
// For fast sight rejection.
// Speeds up enemy AI by skipping detailed
//  LineOf Sight calculation.
// Without special effect, this could be
//  used as a PVS lookup as well.
//
Uint8* rejectmatrix;


// Maintain single and multi player starting spots.
#define MAX_DEATHMATCH_STARTS	10

mapthing_t deathmatchstarts[MAX_DEATHMATCH_STARTS];
mapthing_t* deathmatch_p;
mapthing_t playerstarts[MAXPLAYERS];


//
// P_LoadVertexes
//
void P_LoadVertexes(int lump) {
	Uint8* data;
	int i;
	mapvertex_t* ml;
	vertex_t* li;

	// Determine number of lumps:
	//  total lump length / vertex record length.
	numvertexes = W_LumpLength(lump) / sizeof(mapvertex_t);

	// Allocate zone memory for buffer.
	vertexes = X_Malloc(numvertexes * sizeof(vertex_t), XTag_Level);

	// Load data into cache.
	data = W_CacheLumpNum(lump, XTag_Static);

	ml = (mapvertex_t *)data;
	li = vertexes;

	// Copy and convert vertex coordinates,
	// internal representation as fixed.
	for(i = 0; i < numvertexes; i++ , li++ , ml++) {
		li->x = SDL_SwapLE16(ml->x) << FRACBITS;
		li->y = SDL_SwapLE16(ml->y) << FRACBITS;
	}
}


//
// P_LoadSegs
//
void P_LoadSegs(int lump) {
	Uint8* data;
	int i;
	mapseg_t* ml;
	seg_t* li;
	line_t* ldef;
	int linedef;
	int side;

	numsegs = W_LumpLength(lump) / sizeof(mapseg_t);
	segs = X_Malloc(numsegs * sizeof(seg_t), XTag_Level);
	memset(segs, 0, numsegs * sizeof(seg_t));
	data = W_CacheLumpNum(lump, XTag_Static);

	ml = (mapseg_t *)data;
	li = segs;
	for(i = 0; i < numsegs; i++ , li++ , ml++) {
		li->v1 = &vertexes[SDL_SwapLE16(ml->v1)];
		li->v2 = &vertexes[SDL_SwapLE16(ml->v2)];

		li->angle = (SDL_SwapLE16(ml->angle)) << 16;
		li->offset = (SDL_SwapLE16(ml->offset)) << 16;
		linedef = SDL_SwapLE16(ml->linedef);
		ldef = &lines[linedef];
		li->linedef = ldef;
		side = SDL_SwapLE16(ml->side);
		li->sidedef = &sides[ldef->sidenum[side]];
		li->frontsector = sides[ldef->sidenum[side]].sector;
		if(ldef->flags & ML_TWOSIDED)
			li->backsector = sides[ldef->sidenum[side ^ 1]].sector;
		else
			li->backsector = 0;
	}
}


//
// P_LoadSubsectors
//
void P_LoadSubsectors(int lump) {
	Uint8* data;
	int i;
	mapsubsector_t* ms;
	subsector_t* ss;

	numsubsectors = W_LumpLength(lump) / sizeof(mapsubsector_t);
	subsectors = X_Malloc(numsubsectors * sizeof(subsector_t), XTag_Level);
	data = W_CacheLumpNum(lump, XTag_Static);

	ms = (mapsubsector_t *)data;
	memset(subsectors, 0, numsubsectors * sizeof(subsector_t));
	ss = subsectors;

	for(i = 0; i < numsubsectors; i++ , ss++ , ms++) {
		ss->numlines = SDL_SwapLE16(ms->numsegs);
		ss->firstline = SDL_SwapLE16(ms->firstseg);
	}
}


//
// P_LoadSectors
//
void P_LoadSectors(int lump) {
	Uint8* data;
	int i;
	mapsector_t* ms;
	sector_t* ss;

	numsectors = W_LumpLength(lump) / sizeof(mapsector_t);
	sectors = X_Malloc(numsectors * sizeof(sector_t), XTag_Level);
	memset(sectors, 0, numsectors * sizeof(sector_t));
	data = W_CacheLumpNum(lump, XTag_Static);

	ms = (mapsector_t *)data;
	ss = sectors;
	for(i = 0; i < numsectors; i++ , ss++ , ms++) {
		ss->floorheight = SDL_SwapLE16(ms->floorheight) << FRACBITS;
		ss->ceilingheight = SDL_SwapLE16(ms->ceilingheight) << FRACBITS;
		ss->floorpic = R_FlatNumForName(ms->floorpic);
		ss->ceilingpic = R_FlatNumForName(ms->ceilingpic);
		ss->lightlevel = SDL_SwapLE16(ms->lightlevel);
		ss->special = SDL_SwapLE16(ms->special);
		ss->tag = SDL_SwapLE16(ms->tag);
		ss->thinglist = NULL;
	}
}


//
// P_LoadNodes
//
void P_LoadNodes(int lump) {
	Uint8* data;
	int i;
	int j;
	int k;
	mapnode_t* mn;
	node_t* no;

	numnodes = W_LumpLength(lump) / sizeof(mapnode_t);
	nodes = X_Malloc(numnodes * sizeof(node_t), XTag_Level);
	data = W_CacheLumpNum(lump, XTag_Static);

	mn = (mapnode_t *)data;
	no = nodes;

	for(i = 0; i < numnodes; i++ , no++ , mn++) {
		no->x = SDL_SwapLE16(mn->x) << FRACBITS;
		no->y = SDL_SwapLE16(mn->y) << FRACBITS;
		no->dx = SDL_SwapLE16(mn->dx) << FRACBITS;
		no->dy = SDL_SwapLE16(mn->dy) << FRACBITS;
		for(j = 0; j < 2; j++) {
			no->children[j] = SDL_SwapLE16(mn->children[j]);
			for(k = 0; k < 4; k++)
				no->bbox[j][k] = SDL_SwapLE16(mn->bbox[j][k]) << FRACBITS;
		}
	}
}


//
// P_LoadThings
//
void P_LoadThings(int lump) {
	Uint8* data;
	int i;
	mapthing_t* mt;
	int numthings;
	SDL_bool spawn;

	data = W_CacheLumpNum(lump, XTag_Static);
	numthings = W_LumpLength(lump) / sizeof(mapthing_t);

	mt = (mapthing_t *)data;
	for(i = 0; i < numthings; i++ , mt++) {
		spawn = SDL_TRUE;

		// Do not spawn cool, new monsters if !commercial
		if(gamemode != commercial) {
			switch(mt->type) {
				case 68: // Arachnotron
				case 64: // Archvile
				case 88: // Boss Brain
				case 89: // Boss Shooter
				case 69: // Hell Knight
				case 67: // Mancubus
				case 71: // Pain Elemental
				case 65: // Former Human Commando
				case 66: // Revenant
				case 84: // Wolf SS
					spawn = SDL_FALSE;
					break;
			}
		}
		if(spawn == SDL_FALSE)
			break;

		// Do spawn all other stuff. 
		mt->x = SDL_SwapLE16(mt->x);
		mt->y = SDL_SwapLE16(mt->y);
		mt->angle = SDL_SwapLE16(mt->angle);
		mt->type = SDL_SwapLE16(mt->type);
		mt->options = SDL_SwapLE16(mt->options);

		P_SpawnMapThing(mt);
	}
}


//
// P_LoadLineDefs
// Also counts secret lines for intermissions.
//
void P_LoadLineDefs(int lump) {
	Uint8* data;
	int i;
	maplinedef_t* mld;
	line_t* ld;
	vertex_t* v1;
	vertex_t* v2;

	numlines = W_LumpLength(lump) / sizeof(maplinedef_t);
	lines = X_Malloc(numlines * sizeof(line_t), XTag_Level);
	memset(lines, 0, numlines * sizeof(line_t));
	data = W_CacheLumpNum(lump, XTag_Static);

	mld = (maplinedef_t *)data;
	ld = lines;
	for(i = 0; i < numlines; i++ , mld++ , ld++) {
		ld->flags = SDL_SwapLE16(mld->flags);
		ld->special = SDL_SwapLE16(mld->special);
		ld->tag = SDL_SwapLE16(mld->tag);
		v1 = ld->v1 = &vertexes[SDL_SwapLE16(mld->v1)];
		v2 = ld->v2 = &vertexes[SDL_SwapLE16(mld->v2)];
		ld->dx = v2->x - v1->x;
		ld->dy = v2->y - v1->y;

		if(!ld->dx)
			ld->slopetype = ST_VERTICAL;
		else if(!ld->dy)
			ld->slopetype = ST_HORIZONTAL;
		else {
			if(FixedDiv(ld->dy, ld->dx) > 0)
				ld->slopetype = ST_POSITIVE;
			else
				ld->slopetype = ST_NEGATIVE;
		}

		if(v1->x < v2->x) {
			ld->bbox[BOXLEFT] = v1->x;
			ld->bbox[BOXRIGHT] = v2->x;
		} else {
			ld->bbox[BOXLEFT] = v2->x;
			ld->bbox[BOXRIGHT] = v1->x;
		}

		if(v1->y < v2->y) {
			ld->bbox[BOXBOTTOM] = v1->y;
			ld->bbox[BOXTOP] = v2->y;
		} else {
			ld->bbox[BOXBOTTOM] = v2->y;
			ld->bbox[BOXTOP] = v1->y;
		}

		ld->sidenum[0] = SDL_SwapLE16(mld->sidenum[0]);
		ld->sidenum[1] = SDL_SwapLE16(mld->sidenum[1]);

		if(ld->sidenum[0] != -1)
			ld->frontsector = sides[ld->sidenum[0]].sector;
		else
			ld->frontsector = 0;

		if(ld->sidenum[1] != -1)
			ld->backsector = sides[ld->sidenum[1]].sector;
		else
			ld->backsector = 0;
	}
}


//
// P_LoadSideDefs
//
void P_LoadSideDefs(int lump) {
	Uint8* data;
	int i;
	mapsidedef_t* msd;
	side_t* sd;

	numsides = W_LumpLength(lump) / sizeof(mapsidedef_t);
	sides = X_Malloc(numsides * sizeof(side_t), XTag_Level);
	memset(sides, 0, numsides * sizeof(side_t));
	data = W_CacheLumpNum(lump, XTag_Static);

	msd = (mapsidedef_t *)data;
	sd = sides;
	for(i = 0; i < numsides; i++ , msd++ , sd++) {
		sd->textureoffset = SDL_SwapLE16(msd->textureoffset) << FRACBITS;
		sd->rowoffset = SDL_SwapLE16(msd->rowoffset) << FRACBITS;
		sd->toptexture = R_TextureNumForName(msd->toptexture);
		sd->bottomtexture = R_TextureNumForName(msd->bottomtexture);
		sd->midtexture = R_TextureNumForName(msd->midtexture);
		sd->sector = &sectors[SDL_SwapLE16(msd->sector)];
	}
}


//
// P_LoadBlockMap
//
void P_LoadBlockMap(int lump) {
	int i;
	int count;

	blockmaplump = W_CacheLumpNum(lump, XTag_Static);
	blockmap = blockmaplump + 4;
	count = W_LumpLength(lump) / 2;

	for(i = 0; i < count; i++)
		blockmaplump[i] = SDL_SwapLE16(blockmaplump[i]);

	bmaporgx = blockmaplump[0] << FRACBITS;
	bmaporgy = blockmaplump[1] << FRACBITS;
	bmapwidth = blockmaplump[2];
	bmapheight = blockmaplump[3];

	// clear out mobj chains
	count = sizeof(*blocklinks) * bmapwidth * bmapheight;
	blocklinks = X_Malloc(count, XTag_Level);
	memset(blocklinks, 0, count);
}


//
// P_GroupLines
// Builds sector line lists and subsector sector numbers.
// Finds block bounding boxes for sectors.
//
void P_GroupLines(void) {
	line_t** linebuffer;
	int i;
	int j;
	int total;
	line_t* li;
	sector_t* sector;
	subsector_t* ss;
	seg_t* seg;
	fixed_t bbox[4];
	int block;

	// look up sector number for each subsector
	ss = subsectors;
	for(i = 0; i < numsubsectors; i++ , ss++) {
		seg = &segs[ss->firstline];
		ss->sector = seg->sidedef->sector;
	}

	// count number of lines in each sector
	li = lines;
	total = 0;
	for(i = 0; i < numlines; i++ , li++) {
		total++;
		li->frontsector->linecount++;

		if(li->backsector && li->backsector != li->frontsector) {
			li->backsector->linecount++;
			total++;
		}
	}

	// build line tables for each sector	
	linebuffer = X_Malloc(total * PTRSIZE, XTag_Level);
	sector = sectors;
	for(i = 0; i < numsectors; i++ , sector++) {
		M_ClearBox(bbox);
		sector->lines = linebuffer;
		li = lines;
		for(j = 0; j < numlines; j++ , li++) {
			if(li->frontsector == sector || li->backsector == sector) {
				*linebuffer++ = li;
				M_AddToBox(bbox, li->v1->x, li->v1->y);
				M_AddToBox(bbox, li->v2->x, li->v2->y);
			}
		}
		if(linebuffer - sector->lines != sector->linecount)
			I_Error("P_GroupLines: miscounted");

		// set the degenmobj_t to the middle of the bounding box
		sector->soundorg.x = (bbox[BOXRIGHT] + bbox[BOXLEFT]) / 2;
		sector->soundorg.y = (bbox[BOXTOP] + bbox[BOXBOTTOM]) / 2;

		// adjust bounding box to map blocks
		block = (bbox[BOXTOP] - bmaporgy + MAXRADIUS) >> MAPBLOCKSHIFT;
		block = block >= bmapheight ? bmapheight - 1 : block;
		sector->blockbox[BOXTOP] = block;

		block = (bbox[BOXBOTTOM] - bmaporgy - MAXRADIUS) >> MAPBLOCKSHIFT;
		block = block < 0 ? 0 : block;
		sector->blockbox[BOXBOTTOM] = block;

		block = (bbox[BOXRIGHT] - bmaporgx + MAXRADIUS) >> MAPBLOCKSHIFT;
		block = block >= bmapwidth ? bmapwidth - 1 : block;
		sector->blockbox[BOXRIGHT] = block;

		block = (bbox[BOXLEFT] - bmaporgx - MAXRADIUS) >> MAPBLOCKSHIFT;
		block = block < 0 ? 0 : block;
		sector->blockbox[BOXLEFT] = block;
	}

}


//
// P_SetupLevel
//
void P_SetupLevel(int episode, int map, int playermask, skill_t skill) {
	int i;
	char lumpname[9];
	int lumpnum;

	totalkills = totalitems = totalsecret = wminfo.maxfrags = 0;
	wminfo.partime = 180;
	for(i = 0; i < MAXPLAYERS; i++) {
		players[i].killcount = players[i].secretcount
		                       = players[i].itemcount = 0;
	}

	// Initial height of PointOfView
	// will be set by player think.
	players[consoleplayer].viewz = 1;

	// Make sure all sounds are stopped before Z_FreeTags.
	S_Start();


#if 0 // UNUSED
    if (debugfile)
    {
	Z_FreeTags (PU_LEVEL, MAXINT);
	Z_FileDumpHeap (debugfile);
    }
    else
#endif
	X_FreeTags(XTag_Level, XTag_Purge - 1);

	T_InitPlats();
	P_InitMobjs();
	P_InitThinkers();

	// if working with a devlopment map, reload it
	W_Reload();

	// find map name
	if(gamemode == commercial) {
		if(map < 10)
			sprintf(lumpname, "map0%i", map);
		else
			sprintf(lumpname, "map%i", map);
	} else {
		lumpname[0] = 'E';
		lumpname[1] = '0' + episode;
		lumpname[2] = 'M';
		lumpname[3] = '0' + map;
		lumpname[4] = 0;
	}

	lumpnum = W_GetNumForName(lumpname);

	leveltime = 0;

	// note: most of this ordering is important	
	P_LoadBlockMap(lumpnum + ML_BLOCKMAP);
	P_LoadVertexes(lumpnum + ML_VERTEXES);
	P_LoadSectors(lumpnum + ML_SECTORS);
	P_LoadSideDefs(lumpnum + ML_SIDEDEFS);

	P_LoadLineDefs(lumpnum + ML_LINEDEFS);
	P_LoadSubsectors(lumpnum + ML_SSECTORS);
	P_LoadNodes(lumpnum + ML_NODES);
	P_LoadSegs(lumpnum + ML_SEGS);

	rejectmatrix = W_CacheLumpNum(lumpnum + ML_REJECT, XTag_Level);
	P_GroupLines();

	bodyqueslot = 0;
	deathmatch_p = deathmatchstarts;
	P_LoadThings(lumpnum + ML_THINGS);

	// if deathmatch, randomly spawn the active players
	if(deathmatch) {
		for(i = 0; i < MAXPLAYERS; i++)
			if(playeringame[i]) {
				players[i].mo = NULL;
				G_DeathMatchSpawnPlayer(i);
			}

	}

	// clear special respawning que
	iquehead = iquetail = 0;

	// set up world state
	P_SpawnSpecials();

	// build subsector connect matrix
	//	UNUSED P_ConnectSubsectors ();

	// preload graphics
	if(precache)
		R_PrecacheLevel();

	//printf ("free memory: 0x%x\n", Z_FreeMemory());

}


//
// P_Init
//
void P_Init(void) {
	P_InitSwitchList();
	P_InitPicAnims();
	R_InitSprites(sprnames);

	/* Initialize all of the necessary linked lists. */
	T_InitPlats();
	P_InitMobjs();
}
