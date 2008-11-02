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

/* win_two.c 3.0 (KIEL) 11.94 */

#include <amore/testBinary.h>

/* functions included in win_two.c
 *
 *	boolx	empty_full_lan()
 *	boole	equiv(dfa dfa1,dfa dfa2);
 *	boole	inclusion()
 */

/** checks if curlan is an empty language
 *  mindfa must be present (sic!)
 * RETURN: TRUE  iff full
 * RETURN: FALSE iff empty 
 * RETURN: UN_KNOWN else
 */
boolx empty_full_lan(language curlan)
{
	/* compute the min. dfa and check if only
	 * one state is present; this state must
	 * not be a final state
	 */
	/* compmdfa(); DISABLED */
	if(!curlan->ldfa->qno)	/* one state only */
		return curlan->ldfa->final[curlan->ldfa->init];
	else
		return UN_KNOWN;
}				/* empty_full_lan */



/** test whether 2 dfas are equivalent
 *
 *  these MUST be minimal dfa's 
 */
boole equiv(dfa dfa1, dfa dfa2)
{
	posint letter, stateno1, stateno2, state1, state2;
	b_array mark1, mark2;	/* mark the state with a preimage or image in bijection */
	array bijection, stack;
	posint height = 1;	/* number of states in stack and first free place */
	boole equal = TRUE;
	/* search for a valid bijection from the states of dfa1 to the states of dfa2 */
	stateno1 = dfa1->qno + 1;
	stateno2 = dfa2->qno + 1;
	if(stateno1 != stateno2)
		return (FALSE);
	bijection = newarray(stateno1 + 1);
	stack = newarray(stateno1 + 1);
	mark1 = newb_array(stateno1 + 1);
	mark2 = newb_array(stateno1 + 1);
	mark1[dfa1->init] = TRUE;
	mark2[dfa2->init] = TRUE;
	bijection[dfa1->init] = dfa2->init;
	stack[0] = dfa1->init;
	while(equal && height) {
		stateno1 = stack[--height];
		stateno2 = bijection[stateno1];
		for (letter = 1; equal && (letter <= dfa1->sno); letter++) {
			state1 = dfa1->delta[letter][stateno1];	/*abbreviation */
			state2 = dfa2->delta[letter][stateno2];	/*abbreviation */
			if(mark1[state1]) {
				if((!mark2[state2]) || (bijection[state1] != state2))
					equal = FALSE;
			} else if(mark2[state2])
				equal = FALSE;
			else {
				mark1[state1] = TRUE;
				mark2[state2] = TRUE;
				bijection[state1] = state2;
				stack[height++] = state1;
			}
		}
	}
	if(equal)		/* bijection found, test for final states */
		for (state1 = 0; equal && (state1 <= dfa1->qno); state1++) {
			state2 = bijection[state1];
			if(dfa1->final[state1] && (!dfa2->final[state2]))
				equal = FALSE;
			if(dfa2->final[state2] && (!dfa1->final[state1]))
				equal = FALSE;
		}
	freebuf();
	return (equal);
}				/* equiv */

/* if inclusion tests if L(dfa1) is contained in L(dfa2)
 * else         tests if L(dfa1) is disjoint to  L(dfa2) 
 * 
 * compute reachable states of the crossprodukt of dfa1 and dfa2
 * for each pair (q1,q2) in the crossprodukt: 
 *     if(q1 is final and q2 is not final) 
 *            then L(dfa1) is not a subset of L(dfa2) 
 * works only if sizeof(Q1) * sizeof(Q2) <= sizeof(posint)
 * use coding (q1,q2) <-> [q1+q2*(dfa1->qno+1)]+1
 *
 */
boole inclusion(dfa dfa1, dfa dfa2, boole inclusion)
{
	boole result = TRUE;
	array mark;		/* mark reachable pairs in the crossprodukt 
				 * mark[i] is the index of the next pair
				 */
	posint letter, test;
	posint q1 = dfa1->init;	/* q1,q2 aktuel pair */
	posint q2 = dfa2->init;
	posint r1, r2;
	posint last;		/* last pair in list */
	posint actuel;		/* abbreviation  for pair(q1,q2) */
	posint max = 1 + (dfa1->qno + 1) * (dfa2->qno + 1);	/* endmarker */
	mark = newarray(max);
	actuel = 1 + q1 + (q2 * (dfa1->qno + 1));
	last = actuel;
	mark[last] = max;	/* mark (q1,q2) */
	while(actuel && result) {
		for (letter = 1; letter <= dfa1->sno; letter++) {

			r1 = dfa1->delta[letter][q1];
			r2 = dfa2->delta[letter][q2];
			if(inclusion) {
				if(dfa1->final[r1] && (!dfa2->final[r2]))
					result = FALSE;
			} else {
				if(dfa1->final[r1] && dfa2->final[r2])
					result = FALSE;
			}
			test = 1 + r1 + (r2 * (dfa1->qno + 1));
			if(!mark[test]) {	/* append pair test to list */
				mark[last] = test;
				mark[test] = max;	/* insert endmarker at the end of the list */
				last = test;
			}
		}
		actuel = mark[actuel];
		if(actuel != max) {
			q2 = (actuel - 1) / (dfa1->qno + 1);
			q1 = (actuel - 1) % (dfa1->qno + 1);
		}
	}
	freebuf();
	return result;
}				/* inclusion */
