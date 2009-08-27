/*!\file  sfexp.h
 * \brief declarations for starfree expressions
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
 *  02111-1307 USA
 */
#ifndef _SFEXP_H
#define _SFEXP_H

#include <amore/global.h>	/* basic types */

#ifdef __cplusplus
extern "C" {
#endif

/** a starfree regular expression.
 *  @ingroup LDO_DECL
 */
struct sexp {
    /**  indexed 0 to mno-1 */
	array ulength;
    /**  indexed 0 to mno-1 */
	array vlength;
    /**  indexed 0 to mno-1 */
	array w0length;
    /**  indexed 0 to mno-1 */
	array w1length;
    /** length of u[x] :   2*ulength[x]   element,generator */
	array_of_int_array u;
    /** length of v[x] :   2*vlength[x]   generator,element */
	array_of_int_array v;
    /** length of w0[x] :   3*w0length[x]  generator,element,generator */
	array_of_int_array w0;
    /** length of w1[x] :   1*w1length[x]     */
	array_of_int_array w1;
    /** 0 to mno-1 */
	b_array computed;
};

/** pointer to a starfree regular expression
 *  @ingroup LDO_DECL
 */
typedef struct sexp *starfexp;


/** allocates a new starfexp
 */
starfexp newsfexp();


/** frees the memory used by the starfree regular expression sf.
 *  flag == TRUE iff memory for the array's has been allocated
 *  mno is the number of monoid elements
 */
void freesf(starfexp sf, boole flag, posint mno);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
