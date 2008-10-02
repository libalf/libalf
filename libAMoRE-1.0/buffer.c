/*
 *  Copyright (c) ?    - 2000 Lehrstuhl fuer Informatik VII, RWTH Aachen
 *  Copyright (c) 2000 - 2002 Burak Emir
 *  This file is part of the libAMoRE library.
 *
 *  libAMoRE is  free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with the GNU C Library; if not, write to the Free
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307 USA.  
 */

/* buffer.c 3.0 (KIEL) 11.94 */
   
#include "global.h"

#include "buffer.h"

#include <stdlib.h> /* calloc */

#include "atime.h"  /* clock_reset */

#include "liberror.h" /* BOSERR, LOSERR, ... */
/* functions included:
 *
 *	static	int switchbuf
 *	
 *		void initbuf
 *		char*newbuf
 *		void freebuf
 *		void bufmark
 *		void buffree
 */
/******************************************************************/
/******************************************************************/
#ifdef DOS
#define BUFPOS	    6         /* number of tries to fetch memory from the OS */
#define BUFS_BIG    20000      /* buffer size */
#define BUFS_SMALL  10000      /* buffer size */
#define BUFSBSWITCH 2	   /* switch from small to big */
#endif

#ifdef UNIX
#define BUFPOS	     15                  /* number of tries to fetch memory from the OS */
#define BUFS_BIG     1048576             /* buffer size */
#define BUFS_SMALL   524288              /* buffer size */
#define BUFSBSWITCH  8	                 /* switch from small to big */
#endif

/* L2BITS: the two least sig. bits
 * MULTOF: addresses should start at numbers which are divisible by MULTOF 
 */
#define L2BITS	        0x3

#ifdef BIT32
#define MULTOF		4
#endif

#ifdef BIT64
#define MULTOF		8
#endif

/******************************************************************/
static char *buffer[BUFPOS];
static int bufcount;               /* marks current buffer position */
static char *start[BUFPOS];        /* pt to the next free position */
static posint rest[BUFPOS];        /* number of free positions */
static posint restsize[BUFPOS];    /* max. size of buffer */
static char *markbuf;              /* mark for freebuf */
static int markcount;
static posint markrest;

/******************************************************************/
/******************************************************************/
static int switchbuf(posint i) {

/* creates a new buffer with size >= i */

	bufcount++;		/* the new buffer has number bufcount */
	if (bufcount == BUFPOS) { 
	  buffree(); 
	  bufcount--; 
	  freebuf(); 
	   
	  (void)longjmp(_jmp,EXCEPTION_PARAM_TOO_SMALL); 
	}
	if (bufcount < BUFSBSWITCH) rest[bufcount] = BUFS_SMALL;
	else {
#ifndef LIBAMORE
	  mess("MEMORY MANAGEMENT: please wait, this computation needs a lot of memory");
#endif
		rest[bufcount] = BUFS_BIG;
	}
	if (rest[bufcount] < i) rest[bufcount] = i;
	restsize[bufcount] = rest[bufcount];

	buffer[bufcount] = (char *)calloc(rest[bufcount],sizeof(char));
	if (buffer[bufcount] == NULL) {
#ifdef DEBUG
		liberror("buffer.h:?","switchbuf 1");
#endif
		buffree(); freebuf();  
                (void)longjmp(_jmp,EXCEPTION_OUT_OF_MEMORY);
	}
	start[bufcount] = buffer[bufcount]; 
	return bufcount;
} /* switchbuf */

/* inits the static variables */

void initbuf() { 
 buffer[0] = (char *)calloc(BUFS_SMALL,sizeof(char)); 
 if (buffer[0] == NULL) {
#ifdef DEBUG
     liberror("buffer.h:?","initbuf 2"); 
#endif
     errex(BOSERR,"INIT: not enough memory to run AMORE");
 }
 start[0] = buffer[0];
 markbuf = buffer[0];
 markcount = 0;
 markrest = BUFS_SMALL;
 rest[0] = BUFS_SMALL;
 restsize[0] = BUFS_SMALL;
 bufcount = 0;

} /* initbuf */


/* get num times size bytes from the buffer (if possible) 
 * RETURN: pointer to the first element
 */
char *newbuf(posint num,posint size) {
  register posint i;
  register int pos;
  char *hlp;  /* save current position in hlp */

	size += (i=(size & L2BITS))? MULTOF - i : 0; 
			      /* size must be a multiple of MULTOF */
	i = num * size;
	for(pos=markcount; pos < bufcount; pos++)
		if (rest[pos] >= i) break;
	
	if (rest[pos] < i) pos = switchbuf(i);
	hlp = start[pos];
	start[pos] += i; /* adjust start and rest */
	rest[pos] -= i;
	return hlp;
} /* newbuf */

/* clear the buffer above the mark */
void freebuf() { 
  int i;
  register char *pts;
	for(i=markcount+1; i <= bufcount; i++) {
		dispose(buffer[i]);
		rest[i] = 0; restsize[i] = 0;
		start[i] = NULL; buffer[i] = NULL;
	}
        bufcount=markcount;
	
	/* clear area: positions markbuf to start-1 are used */
	for(pts = start[markcount]; pts != markbuf; *(--pts) = 0);

	start[bufcount] = markbuf; rest[bufcount] = markrest;

} /* freebuf */

/* only used in genrexnf */
void
bufmark()
/* set a mark in the buffer for freebuf */
{	markbuf = start[bufcount]; markcount = bufcount;
	/* markrest = restsize[bufcount] - rest[bufcount]; */
        markrest = rest[bufcount]; 
} /* bufmark */

/* delete the mark */
void buffree() {	
  markbuf = buffer[0]; markcount = 0; markrest = BUFS_SMALL;
} /* buffree */
