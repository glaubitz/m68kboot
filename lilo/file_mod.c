/*
 * file_mod.c -- Module for reading vector-mapped disk files
 *
 * Copyright (c) 1997 by Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 * 
 * $Id: file_mod.c,v 1.1 1997-08-12 15:26:56 rnhodek Exp $
 * 
 * $Log: file_mod.c,v $
 * Revision 1.1  1997-08-12 15:26:56  rnhodek
 * Import of Amiga and newly written Atari lilo sources, with many mods
 * to separate out common parts.
 *
 * 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

#include "bootstrap.h"
#include "loader.h"
#include "parsetags.h"
#include "config.h"
#include "stream.h"
#include "minmax.h"

/***************************** Prototypes *****************************/

static int file_open( const char *name );
static long file_fillbuf( void *buf );
static int file_skip( long cnt );
static int file_close( void );
static long file_filesize( void );

/************************* End of Prototypes **************************/

#define MAXBUF	(16*512)

/* definition of the module structure */
MODULE file_mod = {
	"file",						/* name */
	MAXBUF,						/* maxbuf (arbitrary) */
	file_open,
	file_fillbuf,
	file_skip,
	file_close,
	file_filesize,
	MOD_REST_INIT
};


static const struct vecent *Vector;
static const struct vecent *CurrVecElt;
static u_long CurrSector, CurrCnt;


static int file_open( const char *name )
{
#ifdef USE_BOOTP
	/* strip off "local:" prefix, if any */
	if (strncmp( name, "local:", 6 ) == 0)
		name += 6;
#endif
	if (!(Vector = FindVector( name )))
		return( -1 );
	CurrVecElt = Vector+1;
	CurrSector = CurrVecElt->start;
	CurrCnt = CurrVecElt->length;
	return( 0 );
}

static long file_fillbuf( void *buf )
{
	unsigned len;
	long err;

	if (CurrCnt == 0)
		return( 0 ); /* EOF */
	
	len = min( MAXBUF/512, CurrCnt );
	if ((err = ReadSectors( buf, Vector[0].length, CurrSector, len )))
		return( err );
	if ((CurrCnt -= len) == 0) {
		CurrVecElt++;
		CurrSector = CurrVecElt->start;
		CurrCnt = CurrVecElt->length;
	}
	else
		CurrSector += len;
	return( len*512 );
}

static int file_skip( long cnt )
{
	unsigned len;
	long pos = currmod->fpos;

	while( cnt >= 512 ) {
		len = min( cnt/512, CurrCnt );
		if ((CurrCnt -= len) == 0) {
			CurrVecElt++;
			CurrSector = CurrVecElt->start;
			CurrCnt = CurrVecElt->length;
		}
		else
			CurrSector += len;
		cnt -= len*512;
		pos += len*512;
	}
	return( pos );
}

static int file_close( void )
{
	return( 0 );
}

static long file_filesize( void )
{
	/* File length is stored in 'start' field of first vector element */
	return( Vector[0].start );
}


/* Local Variables: */
/* tab-width: 4     */
/* End:             */
