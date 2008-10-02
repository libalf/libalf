/** \file
 *  \brief contains global variable declarations.
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

#include <setjmp.h> /* jmp_buf */
#include <stdio.h>  /* FILE */
#include "cons.h" /* constants */
#include "typedefs.h" /* language... */

/** ( global ), for transition arrays */
char itoc[28] = "@abcdefghijklmnopqrstuvwxyz";

/** ( global ) enviroment place for jmp's / Lu-Go */
jmp_buf _jmp;			 

/** ( global ), dummy values */
char dummy[PILEN];

/** ( global ), ?? */
unsigned long comptime = 0;


/* these were used in the ncurses version... maybe gets reactivated ?*/
/* #ifndef LIBAMORE */
/* * ( global ), ?? */
/* int plines; */
/*  ( global ), ?? */
/* int pcols; */
/* #endif */


/** ( global ), ?? */
/* char *lpdest; */

