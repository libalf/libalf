/** \file
 *  \brief functions that alter the language
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

#ifndef _UNARY_B
#define _UNARY_B

#include <amore/nfa.h>
#include <amore/dfa.h>

#ifdef __cplusplus
extern "C" {
#endif

/** computes the star (iff plus == FALSE) or the plus automaton
 * RETURN: a nfa describing (L(na))* or (L(na))+
 * na without epsilon-transitions
 *  @ingroup TRANSF_UNARY_B
 */
nfa starnfa(nfa na, boole plus);

/** computes the complement automaton
 * RETURN: a dfa describing ~L(da)
 *  @ingroup TRANSF_UNARY_B
 */
dfa compldfa(dfa da);

/** produces an NFA representing the reversal language
 *  @ingroup TRANSF_UNARY_B
 */
nfa revnfa(nfa na);

/** produces a DFA representing
 * MIN(L) = { u | u in L and not exists v, w: w neq epsilon, v \in L, vw=u
 *  @ingroup TRANSF_UNARY_B
 */
dfa minL(dfa indfa);

/** produces a dfa representing
 * MAX(L) ={ u | u in L and notexists v neq epsilon: uv in L}
 * MAX(L) is empty if no deadstate exists in the minimal automaton
 * indfa must be minimal
 *  @ingroup TRANSF_UNARY_B
 */
dfa maxL(dfa indfa);

/** produces a dfa representing PREF(L) = { u | exists v : uv in L}
 * if indfa has not deadstate, then PREF(L)=A*
 * indfa must be minimal
 *  @ingroup TRANSF_UNARY_B
 */
dfa pref(dfa indfa);

/** produces a nfa representing SUFFIX(L)={v | exists u uv in L}
 * indfa must be minimal, at least all states must be reachable
 * all states become initial states
 *  @ingroup TRANSF_UNARY_B
 */
nfa suff(dfa indfa);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
