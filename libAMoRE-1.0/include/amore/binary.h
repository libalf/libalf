/** \file
 *  \brief contains binary transformations
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
#ifndef _BINARY_H
#define _BINARY_H

#include <amore/dfa.h>
#include <amore/nfa.h>

#ifdef __cplusplus
extern "C" {
#endif

/** computes the union L(na1)+L(na2)
 * alphabets need to be the same.
 * RETURN a nfa
 * @ingroup TRANSF_BINARY
 */
nfa unionfa(nfa na1, nfa na2);

/** computes the concatenation L(na1).\ L(na2).
 * alphabets need to be the same.
 * RETURN a nfa describing the new language
 * @ingroup TRANSF_BINARY
 */
nfa concatfa(nfa na1, nfa na2);


/** if minus  compute L(da1) \ L(da2) else L(da1) & L(da2).
 *  alphabets need to be the same.
 * @ingroup TRANSF_BINARY
 */
dfa insecfa(dfa da1, dfa da2, boole minus);


/** compute the shuffle of na1 and na2
 *  alphabets need to be the same.
 * compute a nfa for the shuffle
 * @ingroup TRANSF_BINARY
 */
nfa shuffle(nfa na1, nfa na2);

/* L(leftquot(dfa1,dfa2))= { w | exists  v in L(dfa2) vw in L(dfa1) }.
 *  alphabets need to be the same.
 * output is a nfa with usualy more than one initial state.
 * @ingroup TRANSF_BINARY
 */
nfa leftquot(dfa indfa1, dfa indfa2);


/** L(rightquot(dfa1,dfa2)) = { w | exists v in L(dfa2) wv in L(dfa1)  }.
 *  alphabets need to be the same.
 * @ingroup TRANSF_BINARY
 */
dfa rightquot(dfa indfa1, dfa indfa2);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
