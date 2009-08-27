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

/* nfa2mnfa_help.c
 */

#include <amore/nfa2mnfa_help.h>

nfa delsta(nfa inputnfa)
{
	mrkfin infin = inputnfa->infin;	/* abbreviation */
	nfa result;		/* Automaton to be returned.       */
	b_array reach1, reach2;	/* mark reachable and productive states */
	array stack;
	posint height = 0;	/* height of stack */
	posint state1, state2, s1, s2, letter;
	posint count = 0;	/* count reachable and productive states */

	reach1 = newb_array(inputnfa->highest_state + 1);
	reach2 = newb_array(inputnfa->highest_state + 1);
	stack = newarray_of_int(inputnfa->highest_state + 1);
	/* init stack with initial states */
	for (state1 = 0; state1 <= inputnfa->highest_state; state1++)
		if(isinit(infin[state1])) {
			reach1[state1] = TRUE;
			stack[height++] = state1;
		}
	while(height) {		/* stack not empty */
		state1 = stack[--height];
		for (letter = 1; letter <= inputnfa->alphabet_size; letter++)
			for (state2 = 0; state2 <= inputnfa->highest_state; state2++)
				if(testcon(inputnfa->delta, letter, state1, state2) && (!reach1[state2])) {
					reach1[state2] = TRUE;
					stack[height++] = state2;
				}
	}
	/* all reachable states are marked in reach1
	 * initialize stack with reachable final states 
	 */
	for (state1 = 0; state1 <= inputnfa->highest_state; state1++)
		if(isfinal(infin[state1]) && reach1[state1]) {
			reach2[state1] = TRUE;
			stack[height++] = state1;
			count++;
		}
	while(height) {		/* stack not empty */
		state1 = stack[--height];
		for (state2 = 0; state2 <= inputnfa->highest_state; state2++)
			if(reach1[state2])
				for (letter = 1; letter <= inputnfa->alphabet_size; letter++)
					if(testcon(inputnfa->delta, letter, state2, state1) && (!reach2[state2])) {
						reach2[state2] = TRUE;
						stack[height++] = state2;
						count++;
					}
	}
	/* in reach2 all reachable and productive states are marked
	 * there are exactly count many of these states
	 */
	if(count == (inputnfa->highest_state + 1)) {	/* No state has to be eliminated. */
		freebuf();
		return inputnfa;
	}

	/* A new automaton has to be calculated. */
	result = newnfa();
	result->alphabet_size = inputnfa->alphabet_size;
	result->is_eps = FALSE;
	result->minimal = FALSE;
	if(count)
		result->highest_state = count - 1;
	else
		result->highest_state = 0;
	result->delta = newndelta(result->alphabet_size, result->highest_state);
	result->infin = newfinal(result->highest_state);

	if(count == 0) {	/* L(na)==empty */
		setinit(result->infin[0]);
		setfinalF(result->infin[0]);
	} else {
		for (state1 = 0, count = 0; state1 <= inputnfa->highest_state; state1++)
			if(reach2[state1])
				stack[count++] = state1;
		/* stack contains all old states, that are reachable and productive */
		for (state1 = 0; state1 < count; state1++) {
			s1 = stack[state1];	/* abbreviation for old state */
			setfinal(result->infin[state1], isfinal(infin[s1]));
			if(isinit(infin[s1]))
				setinit(result->infin[state1]);
			for (state2 = 0; state2 < count; state2++) {
				s2 = stack[state1];	/* abbreviation for old state */
				for (letter = 1; letter <= result->alphabet_size; letter++)
					if(testcon(inputnfa->delta, letter, s1, s2))
						connect(result->delta, letter, state1, state2);
			}
		}
	}
	freebuf();
	return result;
}

// reverse-order all words in language
nfa invers_d(dfa inputdfa)
{
	int i, j;
	nfa result;

	result = newnfa();
	result->is_eps = FALSE;
	result->minimal = FALSE;
	result->alphabet_size = inputdfa->alphabet_size;
	result->highest_state = inputdfa->highest_state;
	result->infin = newfinal(result->highest_state);
	result->delta = newndelta(result->alphabet_size, result->highest_state);
	/* Find initial and final states and compute delta. */
	for (i = 0; i <= inputdfa->highest_state; i++) {	/* Test if i is a new initial state. */
		if(isfinal(inputdfa->final[i]))
			setinit(result->infin[i]);
		/* Compute delta for state i. */
		for (j = 1; j <= inputdfa->alphabet_size; j++)
			connect(result->delta, j, inputdfa->delta[j][i], i);
	}
	/* Now find only final state. */
	setfinalT(result->infin[inputdfa->init]);
	return result;
}

/***************************************************************************/
