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

/* dfa.c
 */

#include <amore/global.h>
#include <amore/dfa.h>

dfa newdfa()
{
	dfa hlp;
	hlp = (dfa) calloc(1, sizeof(struct dfauto));
	if(hlp == NULL) {
#ifdef DEBUG
		perror("newdfa 1");
#endif
		OUT_OF_MEM();
	}
	return hlp;
}				/* newdfa */


dfa clonedfa(dfa p)
{
	dfa dest = newdfa();
	int state, letter;

	dest->alphabet_size = p->alphabet_size;
	dest->highest_state = p->highest_state;
	dest->init = p->init;
	dest->minimal = p->minimal;
	dest->final = newfinal(dest->highest_state);
	/* copy final state flags */
	for (state = 0; state <= dest->highest_state; state++) {
		setfinal(dest->final[state], p->final[state]);
	}
	/* copy transition table */
	dest->delta = newddelta(dest->alphabet_size, dest->highest_state);
	for (letter = 1; letter <= dest->alphabet_size; letter++)
		for (state = 0; state <= dest->highest_state; state++)
			dest->delta[letter][state] = p->delta[letter][state];

	return dest;
}				/* clonedfa */

void freedfa(dfa da)
{
	register posint i;
	dispose(da->final);
	for (i = 1; i <= da->alphabet_size;)
		dispose(da->delta[i++]);
	dispose(da->delta);
}				/* freedfa */

ddelta newddelta(posint s, posint q)
{
	ddelta hlp;
	register posint i, j;
	hlp = (ddelta) (calloc(s + 1, sizeof(trans)));
	if(hlp == NULL) {
#ifdef DEBUG
		perror("newddelta 1");
#endif
		OUT_OF_MEM();
	}
	for (i = 1; i <= s;) {
		hlp[i] = (trans) (calloc(q + 1, sizeof(posint)));
		if(hlp[i++] == NULL) {
#ifdef DEBUG
			perror("newddelta 2");
#endif
			for (j = i - 1; --j;)
				dispose(hlp[j]);
			dispose(hlp);
			OUT_OF_MEM();
		}
	}
	return hlp;
}				/* newddelta */
