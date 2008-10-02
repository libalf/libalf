/* typedefs.h 
 *  \brief -  basic type definitions etc.
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

#ifndef _TYPEDEF_H
#define _TYPEDEF_H

/** basic type for numbers */
typedef unsigned int    posint;
/** FALSE,TRUE */
typedef char            boole;       
/** FALSE,TRUE, UN_KNOWN,...*/
typedef char            boolx;
/** string */
typedef char*           string;
/** arrayofstring */
typedef string*         arrayofstring;
/** array */
typedef posint*         array;
/** arrayofarray */
typedef array*          arrayofarray;
/**  b_array */
typedef boole*          b_array;
/** arrayofb_array */
typedef b_array*        arrayofb_array;

/** for final states(dfa, nfa) and initial states(nfa only) */
typedef boolx   *mrkfin;  

/*  actions for final and initial */

/** <b> ((A) & FINMASK)
 *  @memo
 */
#define isfinal(A)	((A) & FINMASK)
/** <b> {if ((B)) A |= FINMASK; else A &= ~FINMASK;}
 *  @memo
 */
#define setfinal(A,B)	{if ((B)) A |= FINMASK; else A &= ~FINMASK;}
/** <b> A |= FINMASK
 *  @memo
 */
#define setfinalT(A)     A |= FINMASK
/** <b> A &= ~FINMASK
 *  @memo
 */
#define setfinalF(A)     A &= ~FINMASK
/** <b> ((A) & INMASK)
 *  @memo
 */
#define isinit(A)	((A) & INMASK)
/** <b> A |= INMASK
 *  @memo
 */
#define setinit(A)	A |= INMASK
/** <b> A &= ~INMASK
 *  @memo
 */
#define rminit(A)	A &= ~INMASK


/** list of d_class. wrapper around d_class with next field */
/* struct dlist { 
  d_class *info;
  struct dlist *next;
  } ; */

/** pointer to a dlist */
/* typedef struct dlist *d_list; */


#endif
