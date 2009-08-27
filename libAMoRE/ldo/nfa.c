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

/* nfa.c
 */

#include <amore/global.h>
#include <amore/nfa.h>

nfa newnfa()
{
	nfa hlp;
	hlp = (nfa) calloc(1, sizeof(struct nfauto));
	if(hlp == NULL) {
#ifdef DEBUG
		perror("newnfa 1");
#endif
		OUT_OF_MEM();
	}
	return hlp;
}				/* newnfa */

nfa clonenfa(nfa p)
{
	nfa dest = newnfa();
	int state, letter;
	int k;

	dest->alphabet_size = p->alphabet_size;
	dest->highest_state = p->highest_state;
	dest->infin = newfinal(dest->highest_state);
	for (state = 0; state <= dest->highest_state; state++) {
		dest->infin[state] = p->infin[state];
	}
	if(p->is_eps) {
		dest->is_eps = TRUE;
		dest->delta = newendelta(dest->alphabet_size, dest->highest_state);
		for (letter = 0; letter <= dest->alphabet_size; letter++)
			for (state = 0; state <= dest->highest_state; state++)
//former:			dest->delta[letter][state] = p->delta[letter][state];
				for(k = 0; k <= lastdelta(p->highest_state); k++)
					dest->delta[letter][state][k] = p->delta[letter][state][k];
	} else {
		dest->is_eps = FALSE;
		dest->delta = newndelta(dest->alphabet_size, dest->highest_state);
		for (letter = 1; letter <= dest->alphabet_size; letter++)
			for (state = 0; state <= dest->highest_state; state++)
//former:			dest->delta[letter][state] = p->delta[letter][state];
				for(k = 0; k <= lastdelta(p->highest_state); k++)
					dest->delta[letter][state][k] = p->delta[letter][state][k];
	}
	return dest;
}

void freenfa(nfa na)
{
	posint i, j;
	dispose(na->infin);
	for (j = (na->is_eps) ? 0 : 1; j <= na->alphabet_size;) {
		for (i = 0; i <= na->highest_state;)
			dispose(na->delta[j][i++]);
		dispose(na->delta[j++]);
	}
	dispose(na->delta);
}				/* freenfa */


/** allocate memory for the transition relation of a nondeterministic automaton
 * [s0...s][0...q]
 * s, q are the maximal letter and the maximal state 
 * s0 first letter
 */

static ndelta newnndelta(posint s, posint q, int s0)
{
	ndelta hlp;
	register posint k, j, i, l;
	// sicheres delta = new ntrans[s+1]
	hlp = (ndelta) (calloc(s + 1, sizeof(ntrans)));
	if(hlp == NULL) {
#ifdef DEBUG
		perror("newndelta 1");
#endif
		// im Falle eines Fehlers, bisher allozierten Speicher freigeben
		OUT_OF_MEM();
	}
	for (i = s0; i <= s; i++)
		// sicheres delta[i] = new qlist[q+1]
	{
		hlp[i] = (ntrans) (calloc(q + 1, sizeof(qlist)));
		if(hlp[i] == NULL) {
#ifdef DEBUG
			perror("newndelta 2");
#endif
			// falls Fehler, freigeben...
			for (k = s0; k < i; k++)
				dispose(hlp[k]);
			dispose(hlp);
			OUT_OF_MEM();
		}
		// k = q+1 / 8
		k = lastdelta(q);
		for (j = 0; j <= q; j++)
			// sicheres delta[i][j] = byte[k+1]
		{
			hlp[i][j] = (qlist) (calloc(k + 1, sizeof(char)));
			if(hlp[i][j] == NULL) {
#ifdef DEBUG
				perror("newndelta 3");
#endif
				// falls Fehler, freigeben...
				for (k = j; k--;)
					dispose(hlp[i][k]);
				dispose(hlp[i]);
				for (l = s0; l < i; l++) {
					for (k = q + 1; k--;)
						dispose(hlp[l][k]);
					dispose(hlp[l]);
				}
				dispose(hlp);
				OUT_OF_MEM();
			}
		}
	}
	return hlp;
}				/* newnndelta */


ndelta newendelta(posint s, posint q)
{
	return (newnndelta(s, q, 0));
}

ndelta newndelta(int s, int q)
{
	return (newnndelta(s, q, 1));
}
