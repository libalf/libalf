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

#ifdef __cplusplus
extern "C" {
#endif

/** basic type for numbers */
typedef unsigned int posint;
/** FALSE,TRUE */
typedef char boole;
/** FALSE,TRUE, UN_KNOWN,...*/
typedef char boolx;
/** array_of_c_string */
typedef char **array_of_c_string;
/** array_of_int */
typedef posint *array_of_int;
/** array_of_int_array */
typedef array_of_int *array_of_int_array;
/**  b_array */
typedef boole *b_array;
/** arrayofb_array */
typedef b_array *arrayofb_array;

/** for final states(dfa, nfa) and initial states(nfa only) */
typedef boolx *mrkfin;

/*  actions for final and initial */

/** <b> ((A) & FINMASK)
 *  @memo
 */
#define isfinal(A)	((A) & FINMASK)
/** <b> {if ((B)) A |= FINMASK; else A &= ~FINMASK;}
 *  @memo
 */
#define setfinal(A,B)	{if ((B)) A |= FINMASK; else A &= ~FINMASK;}
/** <b> A |= FINMASK (state is final)
 *  @memo
 */
#define setfinalT(A)     A |= FINMASK
/** <b> A &= ~FINMASK (state is not final)
 *  @memo
 */
#define setfinalF(A)     A &= ~FINMASK
/** <b> ((A) & INMASK)
 *  @memo
 */
#define isinit(A)	((A) & INMASK)
/** <b> A |= INMASK (state is initial)
 *  @memo
 */
#define setinit(A)	A |= INMASK
/** <b> A &= ~INMASK (state is not initial)
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

#ifdef __cplusplus
} // extern "C"
#endif

#endif
