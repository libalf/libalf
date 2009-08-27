/** \file nfa2mnfa_types.h
 *  \brief ...
 *  Copyright (c) ?    - 2000 Lehrstuhl fuer Informatik VII, RWTH Aachen
 *  Copyright (c) 2000 - 2002 Burak Emir
 *                2008, 2009 David R. Piegdon <david-i2@piegdon.de>
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
#ifndef _MNFA_TYPES_H
#define _MNFA_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Memory management - fetching memory for objects. */
#define newcover(A,B)        { A = (cover) newbuf(1,sizeof(struct cov)); \
                             newgridlist(A->glist,B); }
#define newgridlist(A,B)     { A = (gridlist) newbuf(1,sizeof(struct gl)); \
                             newgrid(A->info,B); }
#define newgrid(A,B)         { A = (grid) newbuf(1,sizeof(struct gr)); \
                             A->col = (char *)newbuf(B,sizeof(char)); }
#define newmdelta(A)       (ptset**)newbuf((posint)(A),sizeof(ptset*));
/* Memory for delta of fundamental table. */
#define newgsarray(A)      (ptset*)newbuf((posint)(A),sizeof(ptset));
/* Memory for array_of_int of ptset's. */
#define newpattern(A,B)    { (A) = (ptset)newbuf(1,sizeof(struct ptset_s)); \
                           (A)->grset = (char *)newbuf(B,sizeof(char)); }
#define new_termlist(A)     { A = (termlist) newbuf(1,sizeof(struct tl));}
#define new_term(A)         { A = (term) newbuf(1,sizeof(struct te)); \
                           A->set = (char *)newbuf(1,nbr); }

/* Altering sets and testing of relationships.
 * add(A,B)                 element A is joined to set B.
 * sub(A,B)                 element A is deleted from set B.
 * Testelem(A,B)    iff     A is element of set B.
 */
#define add(A,B)	     { ai = A / SIZEOFBYTE; am = 0x1 << (A % SIZEOFBYTE); \
                                     if (!((B)[ai] & am)) (B)[ai] += am; }
#define sub(A,B)	     { ai = A / SIZEOFBYTE; am = 0x1 << (A % SIZEOFBYTE); \
				     if ((B)[ai] & am) (B)[ai] -= am; }
#define testelem(A,B)        ((B)[A / SIZEOFBYTE] & (0x1 << (A % SIZEOFBYTE)))

/* Typedefs. */

/* Bitmaps are used for sets. */
typedef char *bitmap;

/* The number of elements of set grset is ngrset.  */
typedef struct ptset_s {
	bitmap grset;
	posint ngrset;
} *ptset;

/* col is the set of columns in the grid
 * ncol is the number of columns of grid j.
 */
typedef struct gr {
	bitmap col;
	posint ncol;
} *grid;


/* Gridlists are used for defining lists of grids. Each
 * element has a component named info which is a grid and a
 * pointer to the next gridlist.
 */
typedef struct gl {
	grid info;
	struct gl *next;
} *gridlist;


/* A cover has a component for the number of grids and a
 * pointer to the first element of the list of grids.
 */
typedef struct cov {
	gridlist glist;
	posint sizeofglist;
} *cover;


/* Terms are used for boolean sums. The set is used for
 * representing the uncomplemented variables. Their number
 * is nset. Only one complemented variable is allowed; this
 * can be stored in the posint k.
 */
typedef struct te {
	bitmap set;
	posint nset, k;
} *term;



/* Termlists are used for defining lists of terms. Each
 * element has a component named info which is an array_of_int.
 * This array_of_int is used for storing the numbers of the
 * variables of the term. The component named nv is a posint
 * used for storing the number of the variables. A termlist
 * also has a pointer each to the termlist before and after
 * it.
 */
typedef struct tl {
	array_of_int info;
	posint nv;
	struct tl *next;
	struct tl *bef;
} *termlist;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
