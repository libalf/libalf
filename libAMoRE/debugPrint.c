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

/* debugPrint.c
 */

#include <amore/debugPrint.h>

#include <stdio.h>		/* for printf */

#include <amore/nfa.h>

/** ( global ), for transition arrays */
extern char itoc[28];

static char t[] = "TRUE ";
static char f[] = "FALSE";

#define DP_BUFSIZE 100

static char buf[DP_BUFSIZE];

/** takes array containing init state / final state flags,
 *  prints list of init states to buf
 */
static void printInitStates(mrkfin f, int max)
{
	register int i;
	char *b = buf;

	*b = '\0';

	for (i = 0; i <= max; i++) {
		if(isinit(f[i])) {

			sprintf(b, "q%d ", i);

			while(*b++);	/* advance to end of string */

			if(b - buf > DP_BUFSIZE - 4) {
				/* buffer is full... break */
				sprintf(b, "...");
				i = max + 1;
			}
		}
	}
	if(b > buf)
		*--b = '\0';	/* delete last 2 space */
}

static void printFinalStates(mrkfin f, int max)
{
	int i;
	char *b = buf;

	*b = '\0';

	for (i = 0; i <= max; i++) {
		if(isfinal(f[i])) {
			sprintf(b, "q%d ", i);
			while(*b) {
				b++;
			}	/* advance to end of string */
			if(b - buf > DP_BUFSIZE - 4) {
				sprintf(b, "...");
				i = max + 1; /* break */ ;
			}
		}
	}
	if(b > buf)
		*--b = '\0';	/* delete last 2 space */
}

static void printConnectedStates(ndelta delta, int max, int from, int letter)
{
	int k;
	char *b = buf;

	*b = '\0';
	for (k = 0; k <= max; k++) {	/* for all possible to states */
		if(testcon(delta, letter, from, k)) {

			if(b == buf) {	/* first connection? print arrow */
				sprintf(b, " %c -> {", itoc[letter]);
				while(*b) {
					b++;
				}	/* advance to end of string */
			}

			sprintf(b, "q%d ", k);	/* print target */
			/* printf("\n!q%d b-buf:%d!! buf:X%sX\n",k,b-buf,buf); */
			while(*b) {
				b++;
			}	/* advance to end of string */
			if(b - buf > DP_BUFSIZE - 5) {	/* check range */
				sprintf(buf - DP_BUFSIZE, "...}");
				k = max + 1; /* break */ ;
				b = buf;	/* ensure that no additional closed brace is written */
			}
		}
	}

	if(b > buf)
		sprintf(b - 1, "}");	/*close brace */
}


void debugPrintNfa(nfa n)
{
	int i, j;
	printf("<nfa at %p >", n);
	printf("\nqno:  %d", n->highest_state);
	printf("  alphabet_size:     %d", n->alphabet_size);
	printf("  minimal: %s", n->minimal ? t : f);
	printf("  is_eps:  %s", n->is_eps ? t : f);
	/* init and final */
	printInitStates(n->infin, n->highest_state);
	printf("\ninit states: { %s }  ", buf);
	printFinalStates(n->infin, n->highest_state);
	printf("final states: { %s }\n", buf);

	/* display delta (nondet) */
	printf("%s", "\ndelta:\n");
	fflush(stdout);
	for (i = 0; i <= n->highest_state; i++) {	/* i = from state */
		printf("q%d  ", i);
		fflush(stdout);
		for (j = n->is_eps ? 0 : 1; j <= n->alphabet_size; j++) {	/* j = letter     */

			printConnectedStates(n->delta, n->highest_state, i, j);
			printf("%s", buf);
			/*
			   printf("[ %c ->",itoc[j]);
			   fflush(stdout);
			   foer( k=0; k<=n->highest_state; k++ ) {/ k = to state   /
			   fflush(stdout);
			   if( testcon(n->delta,j,i,k ))
			   printf("q%d ",k);
			   }
			   printf("]");
			 */
		}
		printf("\n");
	}
	printf("\n");
}				/* end debugPrintNfa */

void debugPrintDfa(dfa d)
{
	int i, j;
	printf("<dfa at %p >", d);
	printf("\nqno:     %d", d->highest_state);
	printf("     alphabet_size:     %d", d->alphabet_size);
	printf("\nminimal: %s", d->minimal ? t : f);
	printf("\ninit:    %d", d->init);
	/* final */
	printFinalStates(d->final, d->highest_state);
	printf("\nfinal states: { %s }\n", buf);

	printf("\ndelta:\n");

	/* display delta (det) */
	for (i = 0; i <= d->highest_state; i++) {	/* from state */
		printf("q%d  ", i);
		for (j = 1; j <= d->alphabet_size; j++) {	/* letter */
			printf("[%c ->", itoc[j]);
			printf("q%d ]", d->delta[j][i]);
		}
		printf("\n");
	}
}				/* end debugPrintDfa */

void debugPrintRegExp(regex r)
{
	printf("<regexp at %p >", r);
	printf("generalized ? %s", (r->grex ? t : f));
	printf("     alphabet_size:     %d", r->alphabet_size);
	printf("\nas String (infix) \"%s\"", r->rex);
	printf("\nexpanded (postfix) \"%s\"\n", r->exprex);
}

void debugPrintMonoid(monoid m)
{
	printf("<monoid at %p >", m);
	printf("highest_state (nstates):   %3d  alphabet_size (nletters):    %3d", m->highest_state, m->alphabet_size);
	printf("mno (nelements): %3d  gno (ngenereators) %3d", m->mno, m->gno);
}
