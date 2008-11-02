/*!\file
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
#ifndef _NFA2MNFA_H
#define _NFA2MNFA_H

#include <amore/nfa.h>
#include <amore/dfa.h>

#include <amore/nfa2mnfa_types.h>	/* additional typedefs and things */
#include <amore/nfa2mnfa_sol.h>
#include <amore/nfa2mnfa_help.h>

#ifdef __cplusplus
extern "C" {
#endif

/** reduce NFA to REDNFA.
 * \ingroup TRANSF_A_FROM_NFA
 * finds a nfa with the smallest number of states that accepts
 * the same language as na.
 */
nfa nfa2mnfa(nfa na, dfa minda);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
