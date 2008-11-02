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

	dest->sno = p->sno;
	dest->qno = p->qno;
	dest->init = p->init;
	dest->minimal = p->minimal;
	dest->final = newfinal(dest->qno);
	/* copy final state flags */
	for (state = 0; state <= dest->qno; state++) {
		setfinal(dest->final[state], p->final[state]);
	}
	/* copy transition table */
	dest->delta = newddelta(dest->sno, dest->qno);
	for (letter = 1; letter <= dest->sno; letter++)
		for (state = 0; state <= dest->qno; state++)
			dest->delta[letter][state] = p->delta[letter][state];

	return dest;
}				/* clonedfa */

void freedfa(dfa da)
{
	register posint i;
	dispose(da->final);
	for (i = 1; i <= da->sno;)
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
