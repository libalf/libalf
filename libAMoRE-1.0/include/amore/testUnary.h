/** \file
 *  \brief contains unary tests
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

#ifndef _TESTUNA
#define _TESTUNA

#include <amore/dfa.h>
#include <amore/mon.h>
#include <amore/language.h>

#ifdef __cplusplus
extern "C" {
#endif

/** test whether indfa has dead states
 * @ingroup TEST_UNARY
 */
void deadsearch(dfa indfa, posint * deadpos, posint * deadneg, boole * pos, boole * neg);

/** test whether language is in FOLU
 * @ingroup TEST_UNARY
 */
boole folutest(dfa indfa);


/** test whether lan is starfree.
 * @ingroup TEST_UNARY
 * a monoid M is aperiodic
 * <=> M is H-trivial
 * <=> M is groupfree (contains only trivial groups)
 *
 *     L is starfree
 * <=> the syntactical monoid of L is aperiodic
 */
boole sftest(monoid mon);

/** test membership of the syntactic semigroup in several +-Varieties
 * @ingroup TEST_UNARY
 *
 * L(S) is finite or cofinite   <=>  eS=Se=e for all idempotent e
 *         definite             <=>  Se  = e   for all idempotent e
 *         reverse definite     <=>  eS  = e   for all idempotent e
 *         generalized definite <=>  eSe = e   for all idempotent e
 *         local testable       <=>  eSe is idempotent and commutative
 *                                          for all idempotent e
 *
 *  nilpotent => definite,revdefinite,gendefinite,localtest
 *  definite or revdefinite => gendefinite => localtest
 *
 * if mon->mequals and mon->starfree then
 *   there is a letter whose transformation is the identity
 *   in this case L can`t be nilpotent,definite,revdefinite,gendefinite
 *
 * if stamon->mno==1 then L = A* or L = 0
 */
void proptest(language lan, monoid mon);

/** test whether monoid is piecewise testable
 * @ingroup TEST_UNARY
 *
 * - Varieties
 *
 *      L is piecewiese testable
 *   <=>M(L) is D trivial (size of each dclass is 1)
 */
boole testpwt(monoid mon);

/** test whether monoid has dot depth <= 1
 * @ingroup TEST_UNARY
 *
 *       + - V
 *       L has dot depth <=1
 *  <=>
 *       for all idempotent elements e and id2
 *       and all elements i,j,k,l in S=S(L)
 *         (e*i*f*j)^g * e*i*f*l*e * (k*f*l*e)^g
 *       =
 *         (e*i*f*j)^g * e * (k*f*l*e)^g
 */
boole testdd1(monoid mon);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
