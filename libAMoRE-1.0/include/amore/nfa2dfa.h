/*!\file  nfa2dfa.h
 *  \brief contains nfa2dfa function
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
#ifndef _NFA2DFA_H
#define _NFA2DFA_H

#include <amore/dfa.h>
#include <amore/nfa.h>

#ifdef __cplusplus
extern "C" {
#endif

/** determinisation ( DFA to NFA )
 * @ingroup TRANSF_A_FROM_NFA
 * This is the Rabin-Scott-Powersetconstruction to build an
 * equivalent dfa from the given nfa
 */
dfa nfa2dfa(nfa na);

/** only called in nfamnfa.c
 *  \ingroup TRANSF_A_FROM_NFA
 */
dfa modnfa2dfa(nfa na, char ***transformation);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
