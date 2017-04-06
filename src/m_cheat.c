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
//	Cheat sequence checking.
//
//-----------------------------------------------------------------------------

#include "m_cheat.h"

//
// CHEAT SEQUENCE PACKAGE
//


//
// Called in st_stuff module, which handles the input.
// Returns a 1 if the cheat was successful, 0 if failed.
//
int cht_CheckCheat(cheatseq_t* cht, char key) {
	int rc = 0;

	if(!cht->p)
		cht->p = cht->sequence; // initialize if first time

	if(*cht->p == 0)
		*(cht->p++) = key;
	else if(key == *cht->p)
		cht->p++;
	else
		cht->p = cht->sequence;

	if(*cht->p == 1)
		cht->p++;
	else if(*cht->p == 0xff) // end of sequence character
	{
		cht->p = cht->sequence;
		rc = 1;
	}

	return rc;
}

void cht_GetParam(cheatseq_t* cht, char* buffer) {
	unsigned char *p, c;

	p = cht->sequence;
	// ReSharper disable once CppPossiblyErroneousEmptyStatements
	while(*(p++) != 1);

	do {
		c = *p;
		*(buffer++) = c;
		*(p++) = 0;
	} while(c && *p != 0xff);

	if(*p == 0xff)
		*buffer = 0;

}
