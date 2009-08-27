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

#include <amore/mon.h>

static void compword(monoid mon, posint no);

void freemon(monoid mon)
{
	register posint i;
	dispose(mon->generator);
	dispose(mon->let2gen);
	for (i = 0; i < mon->mno;) {
		dispose(mon->no2trans[i]);
		dispose(mon->gensucc[i++]);
	}
	dispose(mon->gensucc);
	dispose(mon->no2trans);
	dispose(mon->lastletter);
	dispose(mon->no2length);
	dispose(mon->word);
	if(mon->relationcomputed) {
		dispose(mon->rs->rside);
		dispose(mon->rs->lside);
		dispose(mon->rs);
	}
	if(mon->dclassiscomputed)
		freedcl(mon->ds, mon->ds->dno);
}				/* freemon */

monoid newmon()
{
	monoid hlp;
	hlp = (monoid) calloc(1, sizeof(struct mono));
	if(hlp == NULL) {
#ifdef DEBUG
		perror("newmon 1");
#endif
		OUT_OF_MEM();
	}
	return hlp;
}				/* newmon */

void freedcl(dstruct dcl, posint i)
{
	register posint j;
	for (j = 0; j < i; j++) {
		dispose(dcl->dclassarray[j]->lrep);
		dispose(dcl->dclassarray[j]->rrep);
		dispose(dcl->dclassarray[j]->hclass);
	}
	for (j = 0; j < dcl->dno; j++)
		dispose(dcl->dclassarray[j]);
	dispose(dcl->dclassarray);
	dispose(dcl);
}				/* freedcl */

d_class newdclass()
{
	d_class hlp;
	hlp = (d_class) calloc(1, sizeof(struct dclass));
	if(hlp == NULL) {
#ifdef DEBUG
		perror("newdclass 1");
#endif
		OUT_OF_MEM();
	}
	return hlp;
}				/* newdclass */


dstruct newdstruct()
{
	dstruct hlp;
	hlp = (dstruct) calloc(1, sizeof(struct d_struct));
	if(hlp == NULL) {
#ifdef DEBUG
		perror("newdstruct 1");
#endif
		OUT_OF_MEM();
	}
	return hlp;
}				/* newdstruct */

rstruct newrstruct()
{
	rstruct hlp;
	hlp = (rstruct) calloc(1, sizeof(struct r_struct));
	if(hlp == NULL) {
#ifdef DEBUG
		perror("newrstruct 1");
#endif
		OUT_OF_MEM();
	}
	return hlp;
}				/* newrstruct */

darray newdarray(posint a)
{
	darray hlp;
	hlp = (darray) calloc(a, sizeof(d_class));
	if(hlp == NULL) {
#ifdef DEBUG
		perror("newdarray 1");
#endif
		OUT_OF_MEM();
	}
	return hlp;
}				/* newdarray */



#define newmonoid()		(monoid)newbuf((posint)1,(posint)sizeof(struct mono))

char *prword1(char **word, boole copy, posint * length, posint no, monoid mon, boole with, boole zeroone)
     /* char **word;
        boole copy,with,zeroone;
        posint *length;
        posint no;
        monoid mon; */
{
	posint l, start;
	char dummy[2];
	compword(mon, no);
	dummy[1] = '\0';
	for (l = 0; mon->repr[l] != '\0'; l++)
		mon->repr[l] = '\0';
	start = (with) ? 1 : 0;
	if(with) {
		if(no == mult(mon, no, no)) {
			mon->repr[0] = '*';
		} else {
			mon->repr[0] = ' ';
		}
	}
	if(no == 0) {		/* special case identity */
		if(zeroone)
			mon->repr[start] = '1';
	} else if((no == mon->zero) && (zeroone))	/* special case zero */
		mon->repr[start] = '0';
	else if(mon->alphabet_size > 27)
		for (l = 0; l < mon->no2length[no]; l++) {
			(void) strcat(mon->repr, "a");
			(void) strcat(mon->repr, pi2a(mon->generator[mon->word[l]]));
	} else
		for (l = 0; l < mon->no2length[no]; l++) {
			dummy[0] = itoc[mon->generator[mon->word[l]]];
			(void) strcat(mon->repr, dummy);
		}
	/* copy mon->repr if copy is set */
	if(copy) {		/* compute length of string */
		l = 0;
		while(mon->repr[l] != '\0')
			l++;
		*length = ++l;
		(*word) = newstring(l);
		for (l = 0; l < (*length); l++)
			(*word)[l] = mon->repr[l];
	}
	return mon->repr;
}				/* prword1 */


char *prword(posint no, monoid mon, boole with)
{
	return (prword1((char **) NULL, FALSE, (posint *) NULL, no, mon, with, TRUE));
}

/** store representative of no (as sequence of generators) in mon->word */

static void compword(monoid mon, posint no)
{
	posint letter, elem;
	elem = no;
	/* no2length = number of pred */
	for (letter = mon->no2length[no]; letter != 0;) {
		mon->word[--letter] = mon->lastletter[elem];
		elem = mon->gensucc[elem][0];
	}
}


posint mult(monoid mon, posint a, posint b)
{
	register posint i;
	register posint help = a;
	compword(mon, b);
	for (i = 0; i < mon->no2length[b]; i++)
		help = mon->gensucc[help][mon->word[i]];
	return (help);
}


void comprestofmon(mon, indfa)
/* known gensucc 
 * computes no2trans,gensucc[][0],no2length,lastletter for a computed monoid
 * O(mon->mno*mon->gno+mon->mno*mon->highest_state)
 */
monoid mon;
dfa indfa;
{
	posint state, elem, gen, run;
	/* get memory */
	mon->no2length = newar(mon->mno);
	mon->lastletter = newar(mon->mno);
	/* create the identity */
	for (state = 0; state <= mon->highest_state; state++)
		mon->no2trans[0][state] = state;
	mon->gensucc[0][0] = 0;
	mon->lastletter[0] = 0;
	mon->no2length[0] = 0;
	/* compute the transformations */
	run = 1;
	for (elem = 0; elem < mon->mno; elem++)
		for (gen = 1; gen <= mon->gno; gen++)
			if(mon->gensucc[elem][gen] == run) {	/* elem is pred of run */
				mon->gensucc[run][0] = elem;
				mon->lastletter[run] = gen;
				/* compute transformation */
				for (state = 0; state <= mon->highest_state; state++)
					mon->no2trans[run][state] = indfa->delta[mon->generator[gen]][mon->no2trans[elem][state]];
				mon->no2length[run] = mon->no2length[elem] + 1;
				run++;
			}
	mon->word = newar(mon->no2length[mon->mno - 1]);
	gen = mon->no2length[mon->mno - 1] + 1;	/* number of letters of longest element +1 */
	gen *= (mon->alphabet_size <= 27) ? 1 : strlen(pi2a(mon->alphabet_size)) + 1;
	mon->repr = (string) calloc(gen, sizeof(char));
	for (state = 0; state < gen;)
		mon->repr[state++] = '\0';
}

posint monmaxlen(mon)
monoid mon;
{
	posint maxlen, l;
	posint elem, gen;
	boole test;
	char *word;
	if(mon->zero != mon->mno)
		(void) prword1(&word, TRUE, &maxlen, mon->zero, mon, TRUE, FALSE);
	else
		maxlen = 1;
	/* compute all elements which have no successor of greater number */
	for (elem = 0; elem < mon->mno; elem++) {
		test = TRUE;
		for (gen = 1; gen <= mon->gno; gen++)
			if(elem == mon->gensucc[mon->gensucc[elem][gen]][0]) {
				test = FALSE;
				break;
			}
		/* elem is pred of elem.gen in this case |elem.gen|>|elem| */
		if(test) {	/* compute length of elem */
			l = strlen(prword(elem, mon, TRUE));
			maxlen = (maxlen > l) ? maxlen : l;
		}
	}
	return (maxlen);
}
