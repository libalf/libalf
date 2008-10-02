/* @name    ext.h
 *  this file has to be included from sourcefiles to access global variables
 *    declared in 'vars.h'. The decision to equally include all useful 
 *    header files potentially useful here is a pragmatic decision, though 
 *    questionable.
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

#ifndef _EXT_H_
#define _EXT_H_

#include <setjmp.h>
#include <stdio.h>
#include <ctype.h> 
#include <string.h>

#ifdef UNIX	
#ifndef LIBAMORE
#include <curses.h> 
#endif
#include <sys/param.h>
#endif        

#ifdef DOS
#include <malloc.h>
#include <conio.h>
#include <stdlib.h>
#endif


#include "cons.h"
#include "typedefs.h"

#ifdef UNIX
#include "unix.h"
#endif

#ifndef AMORE

extern char itoc[28];

extern char dummy[PILEN];

/* these were used in the curses version */
/* extern int plines; */
/* extern int pcols; */
/* extern char *lpdest;*/


extern FILE *amrp;
/* extern char line[LINELEN1]; MOVED TO READAMR */
extern char *text[];

#endif


/* memory management with the buffer */

/** <b>(array)newbuf((posint)(A),(posint)sizeof(posint))
 *  @memo allocates new {@link array array} of posint via newbuf
 */
#define newarray(A)		(array)newbuf((posint)(A),(posint)sizeof(posint))
/** <b>(mrkfin)newbuf((posint)(A),(posint)sizeof(boolx))
 *  @memo allocates new {@link mrkfin mrkfin} via newbuf
 */
#define newfin(A)		(mrkfin)newbuf((posint)(A),(posint)sizeof(boolx))
/** <b>(b_array)newbuf((posint)(A),(posint)sizeof(boole))
 *  @memo allocates new {@link b_array b_array} via newbuf
 */
#define newb_array(A)		(b_array)newbuf((posint)(A),(posint)sizeof(boole))
/** <b>(arrayofarray)newbuf((posint)(A),(posint)sizeof(array))
 *  @memo allocates new {@link arrayofarray arrayofarray} via newbuf
 */
#define newarrayofarray(A)	(arrayofarray)newbuf((posint)(A),(posint)sizeof(array))
/** <b>(arrayofb_array)newbuf((posint)(A),(posint)sizeof(b_array))
 *  @memo allocates new {@link arrayofb_array arrayofb_array} via newbuf
 */
#define newarrayofb_array(A)	(arrayofb_array)newbuf((posint)(A),(posint)sizeof(b_array))
/** <b>(string)newbuf((posint)(A),(posint)sizeof(char))
 *  @memo allocates new {@link string string} via newbuf
 */
#define newstring(A)		(string)newbuf((posint)(A),(posint)sizeof(char))
/** <b>(arrayofstring)newbuf((posint)(A),(posint)sizeof(string))
 *  @memo allocates {@link new arrayofstring arrayofstring} via newbuf
 */
#define newarrayofstring(A)	(arrayofstring)newbuf((posint)(A),(posint)sizeof(string))

/** FIXME (ext.h */
#define newdlist() (d_list)newbuf((posint)1,(posint)sizeof(struct dlist))


#ifdef DOS
#define  cbreak()		/* Lu-Go 29.11.91 */
#define  crmode()
#define  echo()			/* Lu-Go 29.11.91 */
#define  endwin()
#define  gtyx(A,B,C)  getyx(&B,&C)
#define  initscr()  clear()
#define  nocbreak()		/* Lu-Go 29.11.91 */
#define  nocrmode()
#define  noecho()
#define  nonl()
#define  refresh()
#define  sleep(A)
#endif

#ifdef UNIX
#define gtyx(A,B,C)  getyx(A,B,C)
#endif

#endif
