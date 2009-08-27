/** \file
 *  \brief contains binary test
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

#ifndef _TESTBIN_H
#define _TESTBIN_H

#include <amore/dfa.h>
#include <amore/language.h>

#ifdef __cplusplus
extern "C" {
#endif

/** checks if curlan is an empty language
 * @ingroup TEST_BINARY
 * \return TRUE iff full, FALSE iff empty, UN_KNOWN else
 */
boolx empty_full_lan(language curlan);

/** test whether 2 dfas are equivalent
 *  @ingroup TEST_BINARY
 *  these MUST be minimal dfa's
 */
boole equiv(dfa dfa1, dfa dfa2);

/** if inclusion tests if L(dfa1) is contained in L(dfa2)
 * else         tests if L(dfa1) is disjoint to  L(dfa2) .
 * @ingroup TEST_BINARY
 *
 * compute reachable states of the crossprodukt of dfa1 and dfa2
 * for each pair (q1,q2) in the crossprodukt:
 *
 *     if(q1 is final and q2 is not final)
 *
 *            then L(dfa1) is not a subset of L(dfa2)
 *
 * works only if sizeof(Q1) * sizeof(Q2) <= sizeof(posint)
 *
 * use coding (q1,q2) <-> [q1+q2*(dfa1->highest_state+1)]+1
 *
 */
boole inclusion(dfa dfa1, dfa dfa2, boole inclusion);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
