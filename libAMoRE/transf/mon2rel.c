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

/* monrel.c 3.0 (KIEL) 11.94 */

#include <amore/mon2rel.h>

/* functions included:
 *   functions for mon2rel 
 *      static posint revlex()
 *      static boole inrel()
 *             void  mon2rel()
 */


#define newreltree()	(reltree)newbuf((posint)1,(posint)sizeof(struct reltrees))
typedef struct reltrees {
	posint left;		/* left side of relation = element of monoid */
	posint gen;		/* generator */
	struct reltrees *lsucc, *rsucc;
	struct reltrees *next;
} *reltree;

static monoid stamon;

/* static variables for the computation of relations */
static boole emptytree;
static reltree relroot;		/* root of ordered tree */
static reltree inprel;		/* pointer to the node in the tree where relation 
				   is inserted */

/* a relation rel1 ua=v is greater than relation rel2 wb=x if
 * b^rev(w) is lexicographically greater than a^rev(u)
 *
 * rel1->left must be longer than rel2->left
 *
 * 1 if relation rel2 implies relation rel1  
 * 0 if  rel1 is greater rel2 
 * 2 if  rel2 is greater rel1 
 */
static posint revlex(rel1, rel2)
reltree rel1, rel2;
{
	posint pre1, pre2;
	array last;		/* abbreviation */
	if(rel1->gen < rel2->gen)
		return (2);
	else if(rel1->gen > rel2->gen)
		return (0);
	pre1 = rel1->left;
	pre2 = rel2->left;
	last = stamon->lastletter;
	while((pre2) && (last[pre1] == last[pre2]))
		/* if pre2==0 then word is empty */
	{
		pre1 = stamon->gensucc[pre1][0];
		pre2 = stamon->gensucc[pre2][0];
	}
	if(!pre2)
		return (1);
	if(last[pre1] < last[pre2])
		return (2);
	else
		return (0);
}

static boole inrel(rel)
/* create new node if relation is new and return TRUE 
 * static variable inprel points to the new node 
 */
reltree rel;
{
	posint j;
	reltree search;
	if(emptytree) {		/* init tree */
		relroot = newreltree();
		relroot->left = rel->left;
		relroot->gen = rel->gen;
		inprel = relroot;
		emptytree = FALSE;
		return (TRUE);
	} else {
		search = relroot;
		for (;;) {
			j = revlex(rel, search);
			if(j == 1)
				return (FALSE);	/* new relation is an implication of an old one */
			else if(j == 0) {	/* search at left successor  */
				if(search->lsucc != NULL)
					search = search->lsucc;	/* continue */
				else {	/* new relation */
					inprel = newreltree();
					inprel->left = rel->left;
					inprel->gen = rel->gen;
					search->lsucc = inprel;
					return (TRUE);
				}
			} else {	/* search at right successor */
				if(search->rsucc != NULL)
					search = search->rsucc;	/* continue */
				else {	/* new relation */
					inprel = newreltree();
					inprel->left = rel->left;
					inprel->gen = rel->gen;
					search->rsucc = inprel;
					return (TRUE);
				}
			}
		}
	}
}


/* compute defining relations of monoid stamon
 *    i.e. equation of the form w1 = w2
 *    where w1 and w2 are representatives of the same monoidelelement
 *    and w1 is lexicographical greater than w2
 * possible relations are stored in list relfirst
 *   this list is computed in dfa2mon during the computation of the monoid
 * there are many relations which follow from other relations
 *   for example relations of the form uv.a = uw with v.a=w is also a relation
 *   relations like this are deleted
 * relations of the form zeroa=zero and azero=zero are deleted 
 * relations are ordered in tree 
 */
void mon2rel(monoid mon)
{
	boole new;
	posint i, gen, zero;
	posint countrel = 0;	/* count number of relations in list relfirst */
	arrayofarray succ = mon->gensucc;
	reltree relfirst, rellast;	/* first and last relation in list of relations */
	reltree rel;
	stamon = mon;
	stamon->rs = newrstruct();
	emptytree = TRUE;	/* emptytree = TRUE  iff  tree is empty */
	/* build tree of relations */
	rel = newreltree();
	/* insert zero-relation at the beginning of the list */
	zero = stamon->zero;
	if(zero != stamon->mno) {
		countrel = 1;
		rel->left = succ[zero][0];
		rel->gen = stamon->lastletter[zero];
		relfirst = rel;
		rellast = rel;
		rel = newreltree();
	}
	for (i = 0; i < stamon->mno; i++)
		for (gen = 1; gen <= stamon->gno; gen++) {
			new = FALSE;
			if(succ[succ[i][gen]][0] != i)
				new = TRUE;	/* pred(i*gen)!= i */
			else if(stamon->lastletter[succ[i][gen]] != gen)
				new = TRUE;	/* i*gen=i*gen' with gen'< gen */
			if(new) {
				rel->gen = gen;
				rel->left = i;
				if(zero == succ[i][gen]) {
					if((rel->left == zero) || (rel->gen == zero))
						new = FALSE;	/*left=zero,gen=zero */
					else if(revlex(rel, relfirst) == 1)
						new = FALSE;	/*left*gen=gen'*zero */
				}
				if(new)
					if(inrel(rel)) {
						if(countrel) {	/* insert relation at the end */
							rellast->next = inprel;
							rellast = rellast->next;
						} else {
							relfirst = inprel;
							rellast = relfirst;
						}
						countrel++;	/*tick(); */
					}
			}
		}
/* read out relations */
	stamon->rs->rno = countrel;
	stamon->rs->lside = newar(countrel);
	stamon->rs->rside = newar(countrel);
	for (i = 0; i < countrel; i++) {
		stamon->rs->rside[i] = relfirst->gen;
		stamon->rs->lside[i] = relfirst->left;
		relfirst = relfirst->next;
	}
	stamon->relationcomputed = TRUE;
	freebuf();
}

/* preout() editrel(), prtrel() deleted ! look in old version*/
