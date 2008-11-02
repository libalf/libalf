/*
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

/* enfa2nfa.c 
   from fafa.c 3.0 (KIEL) 11.94 */

#include <amore/enfa2nfa.h>

/* functions included:
 *
 *	static	void  epsclosure()
 *		nfa   enfa2nfa()
 *	
 */

/* mat is a matrix with dimensions 2,#states,#states; matp (0 or 1) denotes
 * the valid matrix at the and of the computations 
 */
static boole ***mat;
static int matp;

/* kleene: set matrix F and use matrix G to assign point (B,C) with cut A */
#define kleene(F,G,A,B,C) \
		mat[F][B][C] = mat[G][B][C] || (mat[G][B][A] && mat[G][A][C])

/******************************************************************/
static void epsclosure(na)
register nfa na;
/* computes in mat the epsilon-closure of all na states; mat[matp] is the final
 * two-dimensional matrix
 */
{
	register posint k;
	boole rrobin;
	register posint i, j;
	posint q = na->qno;
	/* allocate memory in the scratchpad for mat */
	mat = (boole ***) newbuf(2, sizeof(boole **));
	for (i = 0; i < 2; i++) {
		mat[i] = (boole **) newbuf(q + 1, sizeof(boole *));
		for (j = 0; j <= q; mat[i][j++] = (boole *) newbuf(q + 1, sizeof(boole)));
	}
	/* known epsilon-moves and the trivial one's are placed in mat[0] */
	for (j = 0; j <= q; j++) {
		for (k = 0; k <= q; k++)
			if(testcon(na->delta, 0, j, k))
				mat[0][j][k] = TRUE;
			else
				mat[0][j][k] = FALSE;
		mat[0][j][j] = TRUE;
	}
	/* now, use the algorithm of kleene (general path finding algorithm)
	 * to find the epsilon-transitions; we use the Boolean Semiring 
	 */
	rrobin = TRUE;
	for (i = 0; i <= q; i++) {
		for (j = 0; j <= q; j++)
			for (k = 0; k <= q; k++)
				if(rrobin)
					kleene(1, 0, i, j, k);
				else
					kleene(0, 1, i, j, k);
		rrobin = !rrobin;
	}
	/* set matp to the final matrix */
	matp = (rrobin) ? 0 : 1;	/* rrobin has rrobined */
}				/* epsclosure */

/******************************************************************/
/* transformation from a given epsilon-nfa to a nfa without epsilon-moves */

nfa enfa2nfa(nfa epsa)
{
	nfa result;
	posint goal = 0;
	posint state, cut, letter;
	/* copy automaton */
	result = newnfa();
	result->is_eps = FALSE;
	result->qno = epsa->qno;
	result->sno = epsa->sno;
	result->minimal = FALSE;
	result->infin = newfinal(result->qno);
	result->delta = newndelta(result->sno, result->qno);
	for (state = 0; state <= result->qno; state++)
		result->infin[state] = epsa->infin[state];
	/* copy all letter moves */
	for (letter = 1; letter <= result->sno; letter++)
		for (state = 0; state <= result->qno; state++)
			for (goal = 0; goal <= result->qno; goal++)
				cpdelta(result->delta, letter, state, goal, epsa->delta, letter, state, goal);
	if(epsa->is_eps) {	/* construct the epsilon-closure of all states
				 * they are stored in mat[matp]
				 */
		epsclosure(epsa);
		/* check if an initial state is not final, but there is an epsilon-path
		 * to a final state; in this case this state becomes a final state
		 * mat[matp][f][t]   iff  epsilon path from f to t 
		 */
		for (state = 0; state <= result->qno; state++)
			if(isinit(result->infin[state]) && (!isfinal(result->infin[state]))) {
				for (goal = 0; goal <= result->qno; goal++)
					if(isfinal(result->infin[goal]) && mat[matp][state][goal]) {
						setfinalT(result->infin[state]);
						break;
					}
			}
		/* the new letter moves are:
		 * - the old ones
		 * - epsilon-move then letter-move
		 * - letter-move then epsilon-move
		 * cut is the state reached in the latter two cases
		 * before the "then"
		 */
		for (letter = 1; letter <= result->sno; letter++)
			for (state = 0; state <= result->qno; state++)
				for (goal = 0; goal <= result->qno; goal++)
					for (cut = 0; cut <= result->qno; cut++) {
						if(testcon(result->delta, letter, state, goal))
							connect(result->delta, letter, state, goal);
						else if((mat[matp][state][cut]) && testcon(result->delta, letter, cut, goal))
							connect(result->delta, letter, state, goal);
						else if((mat[matp][cut][goal]) && testcon(result->delta, letter, state, cut))
							connect(result->delta, letter, state, goal);
					}
	}
	return result;
}				/* enfa2nfa */
