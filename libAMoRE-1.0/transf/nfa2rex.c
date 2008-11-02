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

/* nfa2rex.c 3.0 (KIEL) 11.94 */

#include <amore/nfa2rex.h>
#include <amore/parser_types.h>
#include <amore/regexp-infix.h>


typedef struct {
	string s;
	posint l;
} edgelbl;

static edgelbl **area;
static posint qst, qno;

/* create, copy, concat */
#define CCC(R,L,A,B) {R=(string)newbuf(L,sizeof(char));R=strcpy(R,A);R=strcat(R,B);}
#define CCS(R,L,A,B,F) {R=(string)newbuf(L,sizeof(char));R=strcpy(R,A);F=strcat(R,B);}

#define test(A)  (strcmp((A),streps))

/******************************************************************/
/******************************************************************/
/* copies the information from the nfa to the field area */

static void init(nfa na)
{
	posint i, j, k, l;
	char str[PILEN + 3];
	char *strno = &(str[1]);
	char *ent, *hlp;
	boole first, eps;
	boole alph = (na->sno > ALPHSIZE);
	/* memory allocation */
	area = (edgelbl **) newbuf(qno + 1, sizeof(edgelbl *));
	for (i = 0; i <= qno;)
		area[i++] = (edgelbl *) newbuf(qno + 1, sizeof(edgelbl));

	if(alph)
		str[0] = 'a';
	else
		str[2] = '\0';

	for (j = 0; j <= na->qno; j++)
		for (i = 0; i <= na->qno; i++) {
			first = TRUE;
			ent = "";
			l = 0;
			for (k = 1; k <= na->sno; k++)
				if(testcon(na->delta, k, i, j)) {
					if(!alph) {
						str[0] = itoc[k];
						if(!first)
							str[1] = unionch;
						else
							str[1] = '\0';
					} else {
						strno = strcpy(strno, pi2a(k));
						if(!first)
							strno = strcat(strno, strunion);
					}
					first = FALSE;
					hlp = ent;
					l += (posint) strlen(str);
					CCC(ent, l + 1, hlp, str);
				}
			area[i][j].l = l;
			if(l) {
				area[i][j].s = (string) newbuf(l + 1, sizeof(char));
				area[i][j].s = strcpy(area[i][j].s, ent);
			}
		}

	/* epsilon transformation from new initial state to the old ones */
	for (i = 0, eps = FALSE; i <= na->qno; i++)
		if(isinit(na->infin[i])) {
			area[na->qno + 1][i].s = streps;
			area[na->qno + 1][i].l = 1;
			if(isfinal(na->infin[i]))
				eps = TRUE;
		}

	/* epsilon transformation from the old final states 
	 * to the new final state
	 */
	if(eps) {
		area[na->qno + 1][na->qno + 2].s = streps;
		area[na->qno + 1][na->qno + 2].l = 1;
	}
	for (i = 0; i <= na->qno; i++)
		if(isfinal(na->infin[i])) {
			area[i][na->qno + 2].s = streps;
			area[i][na->qno + 2].l = 1;
		}
}				/* init */

/* destroys all transformations from a state to itself
 * the situation       qi--re1-->qi--re2-->qj
 * is transformed to   qi--re1<starch>re2<concatch>-->qj
 */

static void destroyloops()
{
	posint i, j, l;
	char *str;
	/*for(i=qst; i < qno; i++) */ i = qst;
	if(area[i][i].l) {	/* a loop */
		l = area[i][i].l + 1;
		area[i][i].l = 0;
		if(test(area[i][i].s))	/* @* == @, and @.re2 == re2 */
			for (j = qst; j <= qno; j++)
				if(area[i][j].l) {
					if(test(area[i][j].s)) {	/* re2 != @ */
						if(!strcmp(area[i][i].s, area[i][j].s)) {	/* use <plusch> */
							area[i][j].l++;
							CCS(str, area[i][j].l + 1, area[i][i].s, strplus, area[i][j].s);
						} else {
							area[i][j].l += (l + 1);
							CCC(str, area[i][j].l + 1, area[i][i].s, strstar);
							str = strcat(str, area[i][j].s);
							area[i][j].s = strcat(str, strconcat);
						}
					} else {	/* re2 == @ */
						area[i][j].l = l;
						CCS(str, area[i][j].l + 1, area[i][i].s, strstar, area[i][j].s);
					}
				}
		area[i][i].s = "";
	}
}				/* destroyloops */

/* deletes node qst which is incremented
 * the situation             qi--re1-->delnod--re2-->qj, qi--re3-->qj
 * is transformed to	     qi--re1re2<concatch>re3<unionch>-->qj
 * RETURN: TRUE iff qst reaches the new initial state
 */

static boole deletenode()
{
	posint i, j, l1, l2, l3, w1, w2, w3, delnod;
	string str;
	delnod = qst++;
	for (i = qst; i <= qno; i++)
		if(area[i][delnod].l) {	/* re1 != "" */
			l1 = area[i][delnod].l;
			if(test(area[i][delnod].s))
				w1 = 2;
			else
				w1 = 1;

			for (j = qst; j <= qno; j++)
				if(area[delnod][j].l) {	/* re2 != "" */
					l2 = area[delnod][j].l;
					if(test(area[delnod][j].s))
						w2 = 2;
					else
						w2 = 1;

					l3 = area[i][j].l;
					if(area[i][j].l) {
						if(test(area[i][j].s))
							w3 = 2;
						else
							w3 = 1;
					} else
						w3 = 0;
					switch (9 * w1 + 3 * w2 + w3) {
						/* two cases: re1,re2 == @ or not; re3 in {"",@,else} */
					case 26:	/* re1,re2 != @; re3 else */
						area[i][j].l = l1 + l2 + l3 + 2;
						CCC(str, area[i][j].l + 1, area[i][delnod].s, area[delnod][j].s);
						str = strcat(str, strconcat);
						str = strcat(str, area[i][j].s);
						area[i][j].s = strcat(str, strunion);
						break;
					case 25:	/* re1, re2 != @; re3 == @ */
						area[i][j].l = l1 + l2 + 3;
						CCC(str, area[i][j].l + 1, area[i][delnod].s, area[delnod][j].s);
						area[i][j].s = strcat(str, strcataddeps);
						break;
					case 24:	/* re1, re2 != @; re3 == "" */
						area[i][j].l = l1 + l2 + 1;
						CCC(str, area[i][j].l + 1, area[i][delnod].s, area[delnod][j].s);
						area[i][j].s = strcat(str, strconcat);
						break;
					case 23:	/* re1 != @, re2 == @; re3 else */
					case 22:	/* re1 != @, re2 == @; re3 == @ */
						area[i][j].l = l1 + l3 + 1;
						CCC(str, area[i][j].l + 1, area[i][delnod].s, area[i][j].s);
						area[i][j].s = strcat(str, strunion);
						break;
					case 21:	/* re1 != @, re2 == @; re3 == "" */
						area[i][j].l = l1;
						str = (string) newbuf(l1 + 1, sizeof(char));
						area[i][j].s = strcpy(str, area[i][delnod].s);
						break;
					case 17:	/* re1 == @, re2 != @; re3 else */
					case 16:	/* re1 == @, re2 != @; re3 == @ */
						area[i][j].l = l2 + l3 + 1;
						CCC(str, area[i][j].l + 1, area[delnod][j].s, area[i][j].s);
						area[i][j].s = strcat(str, strunion);
						break;
					case 15:	/* re1 == @, re2 != @; re3 == "" */
						area[i][j].l = l2;
						str = (string) newbuf(l2 + 1, sizeof(char));
						area[i][j].s = strcpy(str, area[delnod][j].s);
						break;
					case 14:	/* re1, re2 == @; re3 else */
						area[i][j].l = l3 + 2;
						CCS(str, area[i][j].l + 1, area[i][j].s, straddeps, area[i][j].s);
						/* re3 and @ commute */
						break;
					case 13:	/* re1, re2 == @; re3 == @ */
						break;
					case 12:	/* re1, re2 == @; re3 == "" */
						area[i][j].l = 1;
						str = (string) newbuf(2, sizeof(char));
						area[i][j].s = strcpy(str, streps);
						break;
					}
				}
		}
	return (qst + 1 < qno);
}				/* deletenode */

/******************************************************************/
/******************************************************************/
regex nfa2rex(nfa na)
{

	regex re;
	qst = 0;
	qno = na->qno + 2;
	/* two more states: a new initial and a new final state
	 * the nfa turns into an epsilon nfa
	 */
	init(na);

	do
		destroyloops();
	while(deletenode());
	destroyloops();

	/* the reg. expr. is the label of the transformation (qst,qno) */
	re = newrex();
	re->sno = na->sno;
	re->grex = FALSE;
	re->useda = -1;
	re->rexl = 0;
	if(area[qst][qno].l) {
		re->erexl = area[qst][qno].l;
		re->exprex = newrexstr(re->erexl);
		re->exprex = strcpy(re->exprex, area[qst][qno].s);
	} else {		/* the empty set */
		re->erexl = 1;
		re->exprex = emptysetstr;
		re->rexl = 1;
		re->rex = emptysetstr;
	}
	re->rex = infix(re, FALSE, &(re->rexl));
	re->grex = FALSE;

	freebuf();
	return re;
}				/* nfa2rex */
