/*
 *  Copyright (c) ?    - 2000 Lehrstuhl fuer Informatik VII, RWTH Aachen
 *  Copyright (c) 2000 - 2002 Burak Emir
 *                2008, 2009 David R. Piegdon <david-i2@piegdon.de>
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

/* nfa2dfa.c
 */
#include <amore/nfa2dfa.h>

/* functions included:
 *	static void push()
 *	static treenode pop()
 *	static boolx cmp()
 *	static boole search()
 *	static posint compnewstates()
 *	static void cpinfo()
 *		dfa nfa2dfa()
 */

/******************************************************************/

/* EQUAL, SMALL, GREAT values for comparisions */
#define  EQUAL (0)
#define SMALL (1)
#define GREAT (2)

/* newdstate: creates memory for new dfa-state
 * newstack : creates a new stackelement
 * setelem  : joins nfa-state A to set B (e.g. dfa-state B)
 * testelem : tests if set B contains nfa-state A
 */
#define newdstate(A)	{ A = (treenode)newbuf(1,sizeof(struct treeelem)); \
			if(with) A->dfastate = (char*)calloc(1,nb);\
                        else A->dfastate = (char *)newbuf(1,nb); \
			A->delta = (trans)newbuf(na->alphabet_size+1,sizeof(posint));}
#define newstack()      (stacknode)newbuf(1,sizeof(struct stackelem));
#define setelem(A,B)	{ ai = A / SIZEOFBYTE; am = 0x1 << (A % SIZEOFBYTE); \
			  if (!(B->dfastate[ai] & am)) B->dfastate[ai] += am; }
#define testelem(A,B)  (B->dfastate[A / SIZEOFBYTE] & (0x1 << (A % SIZEOFBYTE)))

static boole with;		/* new2old needed */
static char **new2old;		/* new2old : show the set of old states that build a new state */
static posint ai, am;		/* needed for setelem */
static posint nb;		/* # of Bytes needed to form a set of all nfa-states */

typedef struct treeelem {
	char *dfastate;		/* info:pt to dfa-state */
	posint number;		/* name as dfa-state */
	trans delta;		/* transformation[letter]=state */
	boole final;		/* TRUE iff final state */
	char bal;		/* balance factor (0,1,2) */
	struct treeelem *tr, *tl;
} *treenode;
typedef struct stackelem {
	treenode info;		/* info: a treenode */
	struct stackelem *next;	/* next element */
} *stacknode;

static stacknode bottom, top;	/* bottom and top of the stack */
static stacknode freestack, freebot;	/* list of (now) unused stackmemory */
static treenode baltree;	/* root of balanced tree */
static treenode knownelem;	/* element found in baltree */

/* we use two data-structures:
 *     a stack for the created dfa-states which have to be concerned next
 *     a balanced tree for all created dfa-states to determine which have
 *        been reached
 * operations on the stack         : push and pop
 * operations on the balanced tree : search-insert
 */

/******************************************************************/
/******************************************************************/
static void push(treenode q)

/* push element q on the stack */
{
	stacknode hlp;
	if(freestack == freebot) {
		hlp = newstack();
	} /* create new element */
	else {
		hlp = freestack;
		freestack = hlp->next;
	}			/* use old element */
	hlp->info = q;
	hlp->next = top;	/* link to top */
	top = hlp;		/* new top */
}				/* push */

/******************************************************************/
static treenode pop()
/* returns the top of the stack */
{
	stacknode hlp;
	hlp = top;		/* save the top element */
	top = top->next;	/* new top */
	hlp->next = freestack;
	freestack = hlp;	/* put hlp on top of freestack */
	return hlp->info;	/* return the information */
}				/* pop */


/******************************************************************/
static boolx cmp(treenode a, treenode b)
{

/* compares the dfa-states of a and b
 * RETURN: EQUAL iff a->dfastate == b->dfastate
 *	   SMALL iff a->dfastate < b->dfastate
 *         GREAT iff a->dfastate > b->dfastate
 */
	posint i;
	for (i = 0; i < nb; i++)
		if(a->dfastate[i] != b->dfastate[i])
			return (a->dfastate[i] > b->dfastate[i]) ? GREAT : SMALL;
	return EQUAL;
}				/* cmp */

/******************************************************************/
static boole search(treenode q)
{
/* inserts element q in the balanced tree (see D.E.KNUTH Vol.III pp.455-457)
 * RETURN: TRUE iff q in balanced tree
 */
	treenode father, rebal, mark, act;
	/* father is the father of act and rebal marks position where rebalancing
	 * may be necessary, mark is a node for rotation process
	 * knownelem contains the place where q is found or inserted
	 */
	boolx t;		/* store compare-values */
	char newbal;

	father = baltree;
	rebal = father->tr;
	act = father->tr;
	/* set links of q */
	q->tl = NULL;
	q->tr = NULL;
	q->bal = 0;
	knownelem = q;
	for (;;) {
		t = cmp(q, act);
		if(t == EQUAL) {	/* the bal. tree contains q */
			knownelem = act;
			return TRUE;
		}
		if(t == SMALL) {	/* q->dfastate is smaller than act->dfastate */
			if(act->tl != NULL) {
				if(act->tl->bal) {
					father = act;
					rebal = act->tl;
				}
				act = act->tl;
			} else {	/* insert q */
				act->tl = q;
				break;
			}
		} else {	/* q->dfastate is greater than act->dfastate */
			if(act->tr != NULL) {
				if(act->tr->bal) {
					father = act;
					rebal = act->tr;
				}
				act = act->tr;
			} else {	/* insert q */
				act->tr = q;
				break;
			}
		}
	}

	/* adjust balance factors between rebal and q */
	if(cmp(q, rebal) == SMALL) {
		mark = rebal->tl;
		newbal = 2;
	} else {
		mark = rebal->tr;
		newbal = 1;
	}
	act = mark;
	for (t = cmp(act, q); t != EQUAL; t = cmp(act, q))
		if(t == SMALL) {
			act->bal = 1;
			act = act->tr;
		} else {
			act->bal = 2;
			act = act->tl;
		}

	/* balancing act */
	if(!rebal->bal)		/* tree has grown */
		rebal->bal = newbal;
	else if(rebal->bal != newbal)	/* tree has gotten more balanced */
		rebal->bal = 0;
	else {			/* tree is out of balance */
		if(mark->bal == newbal) {	/* single rotation */
			act = mark;
			if(newbal == 1) {
				rebal->tr = mark->tl;
				mark->tl = rebal;
			} else {
				rebal->tl = mark->tr;
				mark->tr = rebal;
			}
			rebal->bal = 0;
			mark->bal = 0;
		} else {	/* double rotation */
			if(newbal == 1) {
				act = mark->tl;
				mark->tl = act->tr;
				act->tr = mark;
				rebal->tr = act->tl;
				act->tl = rebal;
			} else {
				act = mark->tr;
				mark->tr = act->tl;
				act->tl = mark;
				rebal->tl = act->tr;
				act->tr = rebal;
			}
			rebal->bal = 0;
			mark->bal = 0;
			if(act->bal == newbal)
				rebal->bal = 3 - newbal;
			if(act->bal == 3 - newbal)
				mark->bal = newbal;
			act->bal = 0;
		}
		/* finishing touch */
		if(father->tr == rebal)
			father->tr = act;
		else
			father->tl = act;
	}
	return FALSE;
}				/* search */

/******************************************************************/
static posint compnewstates(nfa na)
/* computes the sets of nfa-states which form the dfa-states */
{
	treenode actq, nxtq;	/* dfa-states */
	posint nodstates;	/* counts the dfa-states */
	posint l, k, q, p;	/* indices for letters and states */

	nodstates = 0;
	bottom = newstack();
	top = bottom;
	freebot = newstack();
	freestack = freebot;
	newdstate(baltree);

	/* first step:  put set of initial states on the stack
	 * second step: while stack non-empty fetch an element Q
	 *              and start a loop over letters 1...na->alphabet_size
	 *              in which R(act_l) = union(q in Q) na->delta(q,act_l)
	 *              is computed 
	 * third step: check if R(act_l) is known,
	 *             if not: put (R(act_l),first_letter) on the stack
	 */
	/* first step */
	newdstate(actq);
	for (q = 0; q <= na->highest_state; q++)
		if(isinit(na->infin[q])) {
			setelem(q, actq);
			if(isfinal(na->infin[q]))
				actq->final = TRUE;	/* init is final */
		}
	push(actq);
	baltree->tr = actq;	/* initial state set in the tree with number 0 */

	/* second step */
	newdstate(nxtq);
	do {
		actq = pop();
		for (l = 1; l <= na->alphabet_size; l++) {	/* loop: letters */
			for (q = 0; q <= na->highest_state; q++)
				if(testelem(q, actq))	/* loop: q in actq */
					for (p = 0; p <= na->highest_state; p++)	/* union */
						if(testcon(na->delta, l, q, p)) {	/* p reached */
							setelem(p, nxtq);
							if(isfinal(na->infin[p]))
								nxtq->final = TRUE;
						}
			/* third step */
			if(!search(nxtq)) {	/* nxtq is not known */
				nxtq->number = ++nodstates;
				/* tick(); */
				push(nxtq);
				newdstate(nxtq);
			} else {	/* nxtq loeschen */
				for (ai = 0; ai < nb; nxtq->dfastate[ai++] = 0);
				for (k = 1; k <= na->alphabet_size; nxtq->delta[k++] = 0);
				nxtq->final = FALSE;
			}
			actq->delta[l] = knownelem->number;
		}
	} while(bottom != top);
	return nodstates;
}				/* compnewstates */

/* copies the information of baltree to the dfa da
 *  RETURN: nothing
 */

static void cpinfo(dfa da)
{
	treenode actq;		/* runs through the tree */
	posint l;		/* index for letters in da->delta */
	/* traverse baltree and copy  information */
	actq = baltree->tr;	/* baltree->tr points to the balanced tree */
	for (;;) {		/* end of for by return */
		while(actq != NULL) {	/* go left first (depth first) */
			push(actq);
			actq = actq->tl;
		}
		if(bottom != top) {	/* stack is nonempty */
			actq = pop();	/* visit actq */
			if(with)
				new2old[actq->number] = actq->dfastate;
			da->final[actq->number] = actq->final;	/* final mark */
			for (l = 1; l <= da->alphabet_size; l++)
				da->delta[l][actq->number] = actq->delta[l];
			actq = actq->tr;	/* now go right */
		} else
			return;	/* stack is empty, algorithm terminates */
	}
}				/* cpinfo */

static dfa nnfa2dfa(nfa na)
{
	dfa da;
	posint q;
	boole empty = TRUE;	/* test: empty set of initial states */
	nb = (na->highest_state + 1) / SIZEOFBYTE + 1;
	/* first step: initialize known data of the dfa */
	da = newdfa();
	da->init = 0;
	da->alphabet_size = na->alphabet_size;
	da->minimal = FALSE;

	for (q = 0; (q <= na->highest_state) && empty; q++)
		if(isinit(na->infin[q]))
			empty = FALSE;

	if(empty)
		da->highest_state = 0;
	else
		da->highest_state = compnewstates(na);

	da->final = newfinal(da->highest_state);
	da->delta = newddelta(da->alphabet_size, da->highest_state);
	if(with) {
		new2old = (char **) calloc(da->highest_state + 1, sizeof(char *));
		for (q = 0; q <= da->highest_state; q++)
			new2old[q] = (char *) calloc(1, nb);
	}
	if(!empty)
		cpinfo(da);
	freebuf();
	return da;
}				/* nnfa2dfa */

dfa nfa2dfa(nfa na)
{
	with = FALSE;
	return nnfa2dfa(na);
}

dfa modnfa2dfa(nfa na, char ***transformation)
{
	dfa da;
	with = TRUE;
	da = nnfa2dfa(na);
	*transformation = new2old;
	return da;
}
