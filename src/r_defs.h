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
//      Refresh/rendering module, shared data struct definitions.
//
//-----------------------------------------------------------------------------


#ifndef __R_DEFS__
#define __R_DEFS__

#include <SDL_stdinc.h>

// Screenwidth.
#include "doomdef.h"

// Some more or less basic data types
// we depend on.
#include "m_fixed.h"

// We rely on the thinker data struct
// to handle sound origins in sectors.
#include "d_think.h"
// SECTORS do store MObjs anyway.
#include "p_mobj.h"

// Invalid textures will be flood filled with this color.
#define INVALID_TEX 65535
#define INVALID_FLAT -1
#define INVALID_TEX_COLOR 175

// Silhouette, needed for clipping Segs (mainly)
// and sprites representing things.
#define SIL_NONE		0
#define SIL_BOTTOM		1
#define SIL_TOP			2
#define SIL_BOTH		3


//
// INTERNAL MAP TYPES
//  used by play and refresh
//

//
// Your plain vanilla vertex.
// Note: transformed values not buffered locally,
//  like some DOOM-alikes ("wt", "WebView") did.
//
typedef struct {
	fixed_t x;
	fixed_t y;

} vertex_t;


// Forward of LineDefs, for Sectors.
struct line_s;

// Each sector has a degenmobj_t in its center
//  for sound origin purposes.
// I suppose this does not handle sound from
//  moving objects (doppler), because
//  position is prolly just buffered, not
//  updated.
typedef struct {
	fixed_t x;
	fixed_t y;
	fixed_t z;

} degenmobj_t;

//
// The SECTORS record, at runtime.
// Stores things/mobjs.
//
typedef struct {
	fixed_t floorheight;
	fixed_t ceilingheight;
	Sint16 floorpic;
	Sint16 ceilingpic;
	Sint16 lightlevel;
	Sint16 special;
	Sint16 tag;

	// 0 = untraversed, 1,2 = sndlines -1
	Sint32 soundtraversed;

	// thing that made a sound (or null)
	mobj_t* soundtarget;

	// mapblock bounding box for height changes
	Sint32 blockbox[4];

	// origin for any sounds played by the sector
	degenmobj_t soundorg;

	// if == validcount, already checked
	Sint32 validcount;

	// list of mobjs in sector
	mobj_t* thinglist;

	// thinker_t for reversable actions
	void* specialdata;

	Sint32 linecount;
	struct line_s** lines; // [linecount] size

} sector_t;


//
// The SideDef.
//

typedef struct {
	// add this to the calculated texture column
	fixed_t textureoffset;

	// add this to the calculated texture top
	fixed_t rowoffset;

	// Texture indices.
	// We do not maintain names here. 
	Sint16 toptexture;
	Sint16 bottomtexture;
	Sint16 midtexture;

	// Sector the SideDef is facing.
	sector_t* sector;

} side_t;


//
// Move clipping aid for LineDefs.
//
typedef enum {
	ST_HORIZONTAL,
	ST_VERTICAL,
	ST_POSITIVE,
	ST_NEGATIVE

} slopetype_t;


typedef struct line_s {
	// Vertices, from v1 to v2.
	vertex_t* v1;
	vertex_t* v2;

	// Precalculated v2 - v1 for side checking.
	fixed_t dx;
	fixed_t dy;

	// Animation related.
	Sint16 flags;
	Sint16 special;
	Sint16 tag;

	// Visual appearance: SideDefs.
	//  sidenum[1] will be -1 if one sided
	Sint16 sidenum[2];

	// Neat. Another bounding box, for the extent
	//  of the LineDef.
	fixed_t bbox[4];

	// To aid move clipping.
	slopetype_t slopetype;

	// Front and back sector.
	// Note: redundant? Can be retrieved from SideDefs.
	sector_t* frontsector;
	sector_t* backsector;

	// if == validcount, already checked
	Sint32 validcount;

	// thinker_t for reversable actions
	void* specialdata;
} line_t;


//
// A SubSector.
// References a Sector.
// Basically, this is a list of LineSegs,
//  indicating the visible walls that define
//  (all or some) sides of a convex BSP leaf.
//
typedef struct subsector_s {
	sector_t* sector;
	Sint16 numlines;
	Sint16 firstline;

} subsector_t;


//
// The LineSeg.
//
typedef struct {
	vertex_t* v1;
	vertex_t* v2;

	fixed_t offset;

	angle_t angle;

	side_t* sidedef;
	line_t* linedef;

	// Sector references.
	// Could be retrieved from linedef, too.
	// backsector is NULL for one sided lines
	sector_t* frontsector;
	sector_t* backsector;

} seg_t;


//
// BSP node.
//
typedef struct {
	// Partition line.
	fixed_t x;
	fixed_t y;
	fixed_t dx;
	fixed_t dy;

	// Bounding box for each child.
	fixed_t bbox[2][4];

	// If NF_SUBSECTOR its a subsector.
	Uint16 children[2];

} node_t;


// posts are runs of non masked source pixels
typedef struct {
	Uint8 topdelta; // -1 is the last post in a column
	Uint8 length; // length data bytes follows
} post_t;

// column_t is a list of 0 or more post_t, (byte)-1 terminated
typedef post_t column_t;


// PC direct to screen pointers
//B UNUSED - keep till detailshift in r_draw.c resolved
//extern byte*	destview;
//extern byte*	destscreen;


//
// OTHER TYPES
//

// This could be wider for >8 bit display.
// Indeed, true color support is posibble
//  precalculating 24bpp lightmap/colormap LUT.
//  from darkening PLAYPAL to all black.
// Could even us emore than 32 levels.
typedef Uint8 lighttable_t;


//
// ?
//
typedef struct drawseg_s {
	seg_t* curline;
	Sint32 x1;
	Sint32 x2;

	fixed_t scale1;
	fixed_t scale2;
	fixed_t scalestep;

	// 0=none, 1=bottom, 2=top, 3=both
	Sint32 silhouette;

	// do not clip sprites above this
	fixed_t bsilheight;

	// do not clip sprites below this
	fixed_t tsilheight;

	// Pointers to lists for sprite clipping,
	//  all three adjusted so [x1] is first value.
	Sint16* sprtopclip;
	Sint16* sprbottomclip;
	Sint16* maskedtexturecol;

} drawseg_t;


// Patches.
// A patch holds one or more columns.
// Patches are used for sprites and all masked pictures,
// and we compose textures from the TEXTURE1/2 lists
// of patches.
typedef struct {
	Sint16 width; // bounding box size 
	Sint16 height;
	Sint16 leftoffset; // pixels to the left of origin 
	Sint16 topoffset; // pixels below the origin 
	Sint32 columnofs[8]; // only [width] used
	// the [0] is &columnofs[width] 
} patch_t;


// A vissprite_t is a thing
//  that will be drawn during a refresh.
// I.e. a sprite object that is partly visible.
typedef struct vissprite_s {
	// Doubly linked list.
	struct vissprite_s* prev;
	struct vissprite_s* next;

	Sint32 x1;
	Sint32 x2;

	// for line side calculation
	fixed_t gx;
	fixed_t gy;

	// global bottom / top for silhouette clipping
	fixed_t gz;
	fixed_t gzt;

	// horizontal position of x1
	fixed_t startfrac;

	fixed_t scale;

	// negative if flipped
	fixed_t xiscale;

	fixed_t texturemid;
	Sint32 patch;

	// for color translation and shadow draw,
	//  maxbright frames as well
	lighttable_t* colormap;

	Sint32 mobjflags;

} vissprite_t;


//	
// Sprites are patches with a special naming convention
//  so they can be recognized by R_InitSprites.
// The base name is NNNNFx or NNNNFxFx, with
//  x indicating the rotation, x = 0, 1-7.
// The sprite and frame specified by a thing_t
//  is range checked at run time.
// A sprite is a patch_t that is assumed to represent
//  a three dimensional object and may have multiple
//  rotations pre drawn.
// Horizontal flipping is used to save space,
//  thus NNNNF2F5 defines a mirrored patch.
// Some sprites will only have one picture used
// for all views: NNNNF0
//
typedef struct {
	// If false use 0 for any position.
	// Note: as eight entries are available,
	//  we might as well insert the same name eight times.
	SDL_bool rotate;

	// Lump to use for view angles 0-7.
	Sint16 lump[8];

	// Flip bit (1 = flip) to use for view angles 0-7.
	Uint8 flip[8];

} spriteframe_t;


//
// A sprite definition:
//  a number of animation frames.
//
typedef struct {
	int numframes;
	spriteframe_t* spriteframes;

} spritedef_t;


//
// Now what is a visplane, anyway?
// 
typedef struct {
	fixed_t height;
	Sint32 picnum;
	Sint32 lightlevel;
	Sint32 minx;
	Sint32 maxx;

	// leave pads for [minx-1]/[maxx+1]

	Uint8 pad1;
	// Here lies the rub for all
	//  dynamic resize/change of resolution.
	Uint8 top[SCREENWIDTH];
	Uint8 pad2;
	Uint8 pad3;
	// See above.
	Uint8 bottom[SCREENWIDTH];
	Uint8 pad4;

} visplane_t;

#endif
