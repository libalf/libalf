/** \file nfam2nfa_sol.h
 *  \brief more additional stuff
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
#ifndef _NFA2MNFA_SOL_H
#define _NFA2MNFA_SOL_H

#include <amore/nfa.h>
#include <amore/nfa2mnfa_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* this procedure tries to find a nfa result with less states than f and
 * L(result)==L(na) (see nfamnfa.c). This is done by searching for a
 * minimal closed cover as a subset of the grids.
 * @param delta Delta of the fundamental table
 * @param version true iff calculation with rows, false if with cols
 * This is only relevant for the automaton
 * to be returned.
 * et cetera...
 */

nfa calc_sol(ptset ** delta, boole version, gridlist fI, gridlist fII,	/* Pointer to grids; fI->info is the first  */
	     /* grid and fII->info the first non-        */
	     /* essential grid.                          */
	     posint G, posint N, posint R, posint MAXI,	/* G is the number of grids;               */
	     /* N is the number of essential grids;      */
	     /* R=GITTER-NECESSARY;                      */
	     /* MAXI is the number of states of the best  */
	     /* automaton so far - 1.                    */
	     nfa f,		/* f is the minimal dfa with additional     */
	     /* initial states.                          */
	     ptset i_gr, ptset f_gr);	/* Testelem(i,i_gr) iff grid i considered   */
		 /* as a state of the fundamental table is   */
		 /* an initial state.                        */
		 /* Testelem(i,f_gr) iff grid i considered   */
		 /* as a state of the fundamental table is a */
		 /* final state.                             */

#ifdef __cplusplus
} // extern "C"
#endif

#endif
