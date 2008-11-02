/* \file
 * \brief additional stuff for nfa2mnfa
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
#ifndef _NFA2MNFA_HELP_H
#define _NFA2MNFA_H

#include <amore/nfa.h>
#include <amore/dfa.h>
#include <amore/nfa2mnfa_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** deletes states that are not reachable and states that are not
 * able to reach final states of a given nfa inputnfa and returns an
 * automaton without these states.
 */
nfa delsta(nfa inputnfa);

/** inverts dfa's.
 */
nfa invers_d(dfa inputdfa);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
