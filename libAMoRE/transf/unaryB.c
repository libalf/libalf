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

/* win_una.c 3.1 (KIEL) 4.95 */

#include <amore/unaryB.h>

#include <amore/testUnary.h>	/* deadsearch */

static dfa simpledfa(boole full, posint alphabet_size);


nfa starnfa(nfa na, boole plus)
{
	nfa result;
	posint letter, to, from, ini;
	boole finalinit;
	/* result has a new and unique initial state 
	 * new transitions (p,a,q) iff p final  
	 */
	result = newnfa();
	result->highest_state = na->highest_state + 1;
	/* states 0 .. result->highest_state are states of na 
	 *        result->highest_state+1    new initial state
	 */
	result->alphabet_size = na->alphabet_size;
	result->minimal = FALSE;
	result->is_eps = na->is_eps;
	result->infin = newfinal(result->highest_state);
	result->delta = newndelta(result->alphabet_size, result->highest_state);
	if(plus) {		/* search for a state which is initial and final 
				 * in this case L* = L+ 
				 * and the new initil state must be final state 
				 */
		finalinit = FALSE;
		for (ini = 0; ((ini <= na->highest_state) && (!finalinit)); ini++)
			if(isinit(na->infin[ini]))
				finalinit = isfinal(na->infin[ini]);
	} else
		finalinit = TRUE;
	setfinal(result->infin[result->highest_state], finalinit);
	setinit(result->infin[result->highest_state]);

	/* now copy all known transitions and final state marks */
	for (letter = (na->is_eps) ? 0 : 1; letter <= na->alphabet_size; letter++)
		for (from = 0; from <= na->highest_state; from++) {
			setfinal(result->infin[from], isfinal(na->infin[from]));
			for (to = 0; to <= na->highest_state; to++)
				cpdelta(result->delta, letter, from, to, na->delta, letter, from, to);
		}

	/* add transitions (q,a,q') 
	 *      iff 
	 *  q is the new initial state or an old final state of na
	 *  and there ex. a transition (ini,a,q') for an old initial state of na 
	 */
	for (ini = 0; ini <= na->highest_state; ini++)
		if(isinit(na->infin[ini]))
			for (letter = (na->is_eps) ? 0 : 1; letter <= na->alphabet_size; letter++)
				for (to = 0; to <= na->highest_state; to++) {
					cpdelta(result->delta, letter, result->highest_state, to, na->delta, letter, ini, to);
					for (from = 0; from <= na->highest_state; from++)
						if(isfinal(na->infin[from]))
							cpdelta(result->delta, letter, from, to, na->delta, letter, ini, to);
				}

	return result;
}				/* starnfa */


dfa compldfa(dfa da)
{
	dfa result;
	posint i, j;
	result = newdfa();
	result->highest_state = da->highest_state;
	result->alphabet_size = da->alphabet_size;
	result->init = da->init;
	result->minimal = da->minimal;
	result->final = newfinal(result->highest_state);
	result->delta = newddelta(result->alphabet_size, result->highest_state);
	for (j = 0; j <= result->highest_state; j++) {
		result->final[j] = !da->final[j];
		for (i = 1; i <= result->alphabet_size; i++)
			result->delta[i][j] = da->delta[i][j];
	}
	return result;
}				/* compldfa */


/** produces an NFA representing the reversal language 
 */
nfa revnfa(nfa na)
{
	nfa result;
	register posint i, j, s;

	result = newnfa();
	result->highest_state = na->highest_state;
	result->alphabet_size = na->alphabet_size;
	result->minimal = na->minimal;
	result->is_eps = na->is_eps;
	result->infin = newfinal(result->highest_state);
	result->delta = newndelta(result->alphabet_size, result->highest_state);
	for (i = 0; i <= result->highest_state; i++) {
		if(isfinal(na->infin[i]))
			setinit(result->infin[i]);
		setfinal(result->infin[i], isinit(na->infin[i]));
		for (s = (na->is_eps) ? 0 : 1; s <= result->alphabet_size; s++)
			for (j = 0; j <= result->highest_state; j++)
				cpdelta(result->delta, s, i, j, na->delta, s, j, i);
	}
	return (result);
}				/* revnfa */


dfa minL(dfa indfa)
{
	register dfa outdfa;
	posint i, s;
	posint count = 0, dead;
	array old2new;
	outdfa = newdfa();
	if(indfa->final[indfa->init]) {	/* MIN(L)={epsilon} */
		outdfa->highest_state = 1;
		outdfa->alphabet_size = indfa->alphabet_size;
		outdfa->minimal = TRUE;
		outdfa->init = 0;
		outdfa->final = newfinal(outdfa->highest_state);
		outdfa->delta = newddelta(outdfa->alphabet_size, outdfa->highest_state);
		outdfa->final[0] = TRUE;	/* only one final state */
		outdfa->final[1] = FALSE;
		for (i = 0; i <= 1; i++)
			for (s = 1; s <= outdfa->alphabet_size; s++)
				outdfa->delta[s][i] = 1;
		return (outdfa);
	}
	/* every transition leaving the final state enters the dead state 
	 * count nonfinal states and compute transformation
	 * old2new[q] = 0 iff q is a final state in indfa
	 * old2new[q] = k iff q is the kth nonfinal state in indfa
	 */
	old2new = newarray_of_int(indfa->highest_state + 1);
	for (i = 0; i <= indfa->highest_state; i++)
		if(!indfa->final[i])
			old2new[i] = ++count;
	/* state 0 is the only final state
	 * states 1 .. count are the nonfinal states
	 * state count+1 is a dead state
	 */
	dead = count + 1;
	outdfa->highest_state = dead;
	outdfa->alphabet_size = indfa->alphabet_size;
	outdfa->minimal = FALSE;
	outdfa->init = old2new[indfa->init];	/* init is not final!! */
	outdfa->final = newfinal(outdfa->highest_state);
	outdfa->delta = newddelta(outdfa->alphabet_size, outdfa->highest_state);
	outdfa->final[0] = TRUE;	/* only one final state */
	/* every transition leaving the final state enters the dead state */
	for (s = 1; s <= outdfa->alphabet_size; s++) {
		outdfa->delta[s][0] = dead;
		outdfa->delta[s][dead] = dead;
	}
	for (i = 0; i <= indfa->highest_state; i++)
		if(!indfa->final[i])
			for (s = 1; s <= outdfa->alphabet_size; s++)
				outdfa->delta[s][old2new[i]] = old2new[indfa->delta[s][i]];
	return (outdfa);
}				/* minL */


dfa maxL(dfa indfa)
{
	register dfa outdfa;
	boole pos, neg;
	posint q1, dead, letter;
	outdfa = newdfa();
	deadsearch(indfa, &q1, &dead, &pos, &neg);
	if(!neg)
		return simpledfa(FALSE, indfa->alphabet_size);
	outdfa->highest_state = indfa->highest_state;
	outdfa->alphabet_size = indfa->alphabet_size;
	outdfa->minimal = FALSE;
	outdfa->init = indfa->init;
	outdfa->final = newfinal(outdfa->highest_state);
	outdfa->delta = newddelta(outdfa->alphabet_size, outdfa->highest_state);
	/* copy transitions */
	for (q1 = 0; q1 <= outdfa->highest_state; q1++)
		for (letter = 1; letter <= outdfa->alphabet_size; letter++)
			outdfa->delta[letter][q1] = indfa->delta[letter][q1];
	/* compute final 
	 * q is final iff
	 * q is final in indfa and all transitions from q must enter the deadstate
	 */
	for (q1 = 0; q1 <= outdfa->highest_state; q1++)
		if(indfa->final[q1]) {
			outdfa->final[q1] = TRUE;
			for (letter = 1; letter <= outdfa->alphabet_size; letter++)
				if(indfa->delta[letter][q1] != dead) {
					outdfa->final[q1] = FALSE;
					break;
				}
		}
	return (outdfa);
}				/* maxL() */


dfa pref(dfa indfa)
{
	dfa outdfa;
	boole pos, neg;
	posint q1, dead, letter;
	outdfa = newdfa();
	deadsearch(indfa, &q1, &dead, &pos, &neg);
	if(!neg)
		return simpledfa(TRUE, indfa->alphabet_size);
	outdfa->highest_state = indfa->highest_state;
	outdfa->alphabet_size = indfa->alphabet_size;
	outdfa->minimal = FALSE;
	outdfa->init = indfa->init;
	outdfa->final = newfinal(outdfa->highest_state);
	outdfa->delta = newddelta(outdfa->alphabet_size, outdfa->highest_state);
	/* copy transitions */
	for (q1 = 0; q1 <= outdfa->highest_state; q1++)
		for (letter = 1; letter <= outdfa->alphabet_size; letter++)
			outdfa->delta[letter][q1] = indfa->delta[letter][q1];
	/*  all states except the deadstate are final */
	for (q1 = 0; q1 <= outdfa->highest_state; q1++)
		outdfa->final[q1] = TRUE;
	outdfa->final[dead] = FALSE;
	return (outdfa);
}				/* pref() */


nfa suff(indfa)
dfa indfa;
{
	register nfa outnfa;
	posint state, letter;
	outnfa = newnfa();
	outnfa->highest_state = indfa->highest_state;
	outnfa->alphabet_size = indfa->alphabet_size;
	outnfa->minimal = FALSE;
	outnfa->is_eps = FALSE;
	outnfa->infin = newfinal(outnfa->highest_state);
	outnfa->delta = newndelta(outnfa->alphabet_size, outnfa->highest_state);
	/* copy transitions */
	for (state = 0; state <= outnfa->highest_state; state++) {
		setinit(outnfa->infin[state]);
		setfinal(outnfa->infin[state], indfa->final[state]);
		for (letter = 1; letter <= outnfa->alphabet_size; letter++)
			connect(outnfa->delta, letter, state, indfa->delta[letter][state]);
	}
	return (outnfa);	/* inserted by oma 24.April 95 */
}				/* suff() */


static dfa simpledfa(boole full, posint alphabet_size)
{
	dfa a;
	a = newdfa();
	a->highest_state = 0;
	a->alphabet_size = alphabet_size;
	a->delta = newddelta(alphabet_size, 0);
	a->final = newfinal(0);
	a->final[0] = full;
	a->minimal = TRUE;
	a->init = 0;
	return (a);
}
