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

static dfa simpledfa(boole full, posint sno);


nfa starnfa(nfa na, boole plus)
{
	nfa result;
	posint letter, to, from, ini;
	boole finalinit;
	/* result has a new and unique initial state 
	 * new transitions (p,a,q) iff p final  
	 */
	result = newnfa();
	result->qno = na->qno + 1;
	/* states 0 .. result->qno are states of na 
	 *        result->qno+1    new initial state
	 */
	result->sno = na->sno;
	result->minimal = FALSE;
	result->is_eps = na->is_eps;
	result->infin = newfinal(result->qno);
	result->delta = newndelta(result->sno, result->qno);
	if(plus) {		/* search for a state which is initial and final 
				 * in this case L* = L+ 
				 * and the new initil state must be final state 
				 */
		finalinit = FALSE;
		for (ini = 0; ((ini <= na->qno) && (!finalinit)); ini++)
			if(isinit(na->infin[ini]))
				finalinit = isfinal(na->infin[ini]);
	} else
		finalinit = TRUE;
	setfinal(result->infin[result->qno], finalinit);
	setinit(result->infin[result->qno]);

	/* now copy all known transitions and final state marks */
	for (letter = (na->is_eps) ? 0 : 1; letter <= na->sno; letter++)
		for (from = 0; from <= na->qno; from++) {
			setfinal(result->infin[from], isfinal(na->infin[from]));
			for (to = 0; to <= na->qno; to++)
				cpdelta(result->delta, letter, from, to, na->delta, letter, from, to);
		}

	/* add transitions (q,a,q') 
	 *      iff 
	 *  q is the new initial state or an old final state of na
	 *  and there ex. a transition (ini,a,q') for an old initial state of na 
	 */
	for (ini = 0; ini <= na->qno; ini++)
		if(isinit(na->infin[ini]))
			for (letter = (na->is_eps) ? 0 : 1; letter <= na->sno; letter++)
				for (to = 0; to <= na->qno; to++) {
					cpdelta(result->delta, letter, result->qno, to, na->delta, letter, ini, to);
					for (from = 0; from <= na->qno; from++)
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
	result->qno = da->qno;
	result->sno = da->sno;
	result->init = da->init;
	result->minimal = da->minimal;
	result->final = newfinal(result->qno);
	result->delta = newddelta(result->sno, result->qno);
	for (j = 0; j <= result->qno; j++) {
		result->final[j] = !da->final[j];
		for (i = 1; i <= result->sno; i++)
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
	result->qno = na->qno;
	result->sno = na->sno;
	result->minimal = na->minimal;
	result->is_eps = na->is_eps;
	result->infin = newfinal(result->qno);
	result->delta = newndelta(result->sno, result->qno);
	for (i = 0; i <= result->qno; i++) {
		if(isfinal(na->infin[i]))
			setinit(result->infin[i]);
		setfinal(result->infin[i], isinit(na->infin[i]));
		for (s = (na->is_eps) ? 0 : 1; s <= result->sno; s++)
			for (j = 0; j <= result->qno; j++)
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
		outdfa->qno = 1;
		outdfa->sno = indfa->sno;
		outdfa->minimal = TRUE;
		outdfa->init = 0;
		outdfa->final = newfinal(outdfa->qno);
		outdfa->delta = newddelta(outdfa->sno, outdfa->qno);
		outdfa->final[0] = TRUE;	/* only one final state */
		outdfa->final[1] = FALSE;
		for (i = 0; i <= 1; i++)
			for (s = 1; s <= outdfa->sno; s++)
				outdfa->delta[s][i] = 1;
		return (outdfa);
	}
	/* every transition leaving the final state enters the dead state 
	 * count nonfinal states and compute transformation
	 * old2new[q] = 0 iff q is a final state in indfa
	 * old2new[q] = k iff q is the kth nonfinal state in indfa
	 */
	old2new = newarray(indfa->qno + 1);
	for (i = 0; i <= indfa->qno; i++)
		if(!indfa->final[i])
			old2new[i] = ++count;
	/* state 0 is the only final state
	 * states 1 .. count are the nonfinal states
	 * state count+1 is a dead state
	 */
	dead = count + 1;
	outdfa->qno = dead;
	outdfa->sno = indfa->sno;
	outdfa->minimal = FALSE;
	outdfa->init = old2new[indfa->init];	/* init is not final!! */
	outdfa->final = newfinal(outdfa->qno);
	outdfa->delta = newddelta(outdfa->sno, outdfa->qno);
	outdfa->final[0] = TRUE;	/* only one final state */
	/* every transition leaving the final state enters the dead state */
	for (s = 1; s <= outdfa->sno; s++) {
		outdfa->delta[s][0] = dead;
		outdfa->delta[s][dead] = dead;
	}
	for (i = 0; i <= indfa->qno; i++)
		if(!indfa->final[i])
			for (s = 1; s <= outdfa->sno; s++)
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
		return simpledfa(FALSE, indfa->sno);
	outdfa->qno = indfa->qno;
	outdfa->sno = indfa->sno;
	outdfa->minimal = FALSE;
	outdfa->init = indfa->init;
	outdfa->final = newfinal(outdfa->qno);
	outdfa->delta = newddelta(outdfa->sno, outdfa->qno);
	/* copy transitions */
	for (q1 = 0; q1 <= outdfa->qno; q1++)
		for (letter = 1; letter <= outdfa->sno; letter++)
			outdfa->delta[letter][q1] = indfa->delta[letter][q1];
	/* compute final 
	 * q is final iff
	 * q is final in indfa and all transitions from q must enter the deadstate
	 */
	for (q1 = 0; q1 <= outdfa->qno; q1++)
		if(indfa->final[q1]) {
			outdfa->final[q1] = TRUE;
			for (letter = 1; letter <= outdfa->sno; letter++)
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
		return simpledfa(TRUE, indfa->sno);
	outdfa->qno = indfa->qno;
	outdfa->sno = indfa->sno;
	outdfa->minimal = FALSE;
	outdfa->init = indfa->init;
	outdfa->final = newfinal(outdfa->qno);
	outdfa->delta = newddelta(outdfa->sno, outdfa->qno);
	/* copy transitions */
	for (q1 = 0; q1 <= outdfa->qno; q1++)
		for (letter = 1; letter <= outdfa->sno; letter++)
			outdfa->delta[letter][q1] = indfa->delta[letter][q1];
	/*  all states except the deadstate are final */
	for (q1 = 0; q1 <= outdfa->qno; q1++)
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
	outnfa->qno = indfa->qno;
	outnfa->sno = indfa->sno;
	outnfa->minimal = FALSE;
	outnfa->is_eps = FALSE;
	outnfa->infin = newfinal(outnfa->qno);
	outnfa->delta = newndelta(outnfa->sno, outnfa->qno);
	/* copy transitions */
	for (state = 0; state <= outnfa->qno; state++) {
		setinit(outnfa->infin[state]);
		setfinal(outnfa->infin[state], indfa->final[state]);
		for (letter = 1; letter <= outnfa->sno; letter++)
			connect(outnfa->delta, letter, state, indfa->delta[letter][state]);
	}
	return (outnfa);	/* inserted by oma 24.April 95 */
}				/* suff() */


static dfa simpledfa(boole full, posint sno)
{
	dfa a;
	a = newdfa();
	a->qno = 0;
	a->sno = sno;
	a->delta = newddelta(sno, 0);
	a->final = newfinal(0);
	a->final[0] = full;
	a->minimal = TRUE;
	a->init = 0;
	return (a);
}
