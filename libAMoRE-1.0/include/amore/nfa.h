/*!\file  nfa.h
 * \brief declarations for the (epsilon-) NFA
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


#ifndef _NFA_H
#define _NFA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <amore/global.h>	/* basic typedefs */

/** bitmap representing a set of states */
typedef char *qlist;

/** transformation of a letter in a (e)nfa */
typedef qlist *ntrans;

/** nondeterministic transition table.
 * <p>ndelta[i][j] is a bitmap representing all states
 * <p>reachable from state j via letter i
 */
typedef ntrans *ndelta;

/** a (epsilon-) nondeterministic finite automaton
 *  @ingroup LDO_DECL
 */
struct nfauto {
    /** number of states */
	posint highest_state;
    /** size of alphabet. labels are 1 .. alphabet_size, 0 is epsilon */
	posint alphabet_size;
    /** marks for final and init. states */
	mrkfin infin;
    /** ndet. transition relation */
	ndelta delta;
    /** true iff automaton is minimal */
	boole minimal;
    /** true iff epsilon-moves are present, epsilon is letter 0
     */
	boole is_eps;
};

/** pointer to a (epsilon-) nondeterministic finite automaton
 *  @ingroup LDO_DECL
 */
typedef struct nfauto *nfa;

/** alloc a new nfa */
nfa newnfa();

/** clone the nfa */
nfa clonenfa(nfa n);

/** frees the memory used by the nfa na */
void freenfa(nfa na);

/** allocate memory for the transition relation of a nondeterministic automaton
 * [0...s][0...q]
 * s, q are the maximal letter and the maximal state
 */
ndelta newendelta(posint s, posint q);

/** allocate memory for the transition relation of a nondeterministic automaton
 * [1...s][0...q]
 * s, q are the maximal letter and the maximal state
 */
ndelta newndelta(int s, int q);

/* actions for nodeterministic automata
 * Abbr.: Delta Letter From To
 *
 */

/** make transition in D from F to T with Letter L
 */
#define connect(D,L,F,T)\
     D[L][F][(T)/SIZEOFBYTE] |= 0x1 << ((T)%SIZEOFBYTE)

/** \def disconnect destroy connection */
#define disconnect(D,L,F,T)\
     D[L][F][(T)/SIZEOFBYTE] &= ~(0x1 << ((T)%SIZEOFBYTE))

/** TRUE iff connection exists
 */
#define testcon(D,L,F,T)\
     (D[L][F][(T)/SIZEOFBYTE] & (0x1 << ((T) % SIZEOFBYTE)))

/** copy all connections in D2 non-destructive to D1
 */
#define cpdelta(D1,L1,F1,T1,D2,L2,F2,T2)\
     {if(testcon(D2,L2,F2,T2)) connect(D1,L1,F1,T1);}

/**   max. value for the 2nd state loop
 */
#define lastdelta(MAX_QNO)	((MAX_QNO+1) / SIZEOFBYTE)

#ifdef __cplusplus
} // extern "C"
#endif

#endif
