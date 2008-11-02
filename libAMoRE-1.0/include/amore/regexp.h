/*!\file regexp.h
 * \brief declarations for regular expressions
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

#ifndef _REGEXP_H
#define _REGEXP_H

#include <amore/global.h>

#ifdef __cplusplus
extern "C" {
#endif

/** a regular expression
 *  @ingroup LDO_DECL
 */
struct rexstruct {
    /** size of alphabet */
	posint sno;
    /** reg. expr. in infix, with abbreviations */
	string rex;
    /** reg. expr. in postfix, abbreviations expanded */
	string exprex;
    /** abbreviations in infix */
	string abbr[NABBR];
    /** length of abbr */
	posint abbl[NABBR];
    /** number of used abbr */
	int useda;
    /** length of rex */
	posint rexl;
    /** length of exprex */
	posint erexl;
    /** TRUE iff generalized rex */
	boole grex;
};

/** pointer to a regular expression
 *  @ingroup LDO_DECL
 */
typedef struct rexstruct *regex;

/** allocates a new regex
 */
regex newrex();
/** allocates a string
 */
string newrexstr(posint strl);
/** frees the memory used by the regular expression rx. */

void freerex(regex rx);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
