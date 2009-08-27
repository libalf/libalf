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

/* dfamdfa.c 3.0 (KIEL) 11.94 */

#include <amore/dfamdfa.h>

/* functions included
 * static void  delstates()
 * static boole initstatic()
 * static void  computeinverse()
 * static void  makenewautomaton()
 * static void  mindfa()
 *        dfa   dfamdfa()
 *        dfa   dfammdfa()
 */
/*******************************    types       ****************************************/
typedef struct w {
	posint letter;
	posint bno;
	struct w *next;
} *wlist;

typedef array_of_int_array *array_of_int_arrayofarray;

/*******************************  abbreviations ****************************************/
#define newwlist() (wlist)newbuf((posint)1,(posint)sizeof(struct w))
#define newarray_of_int_arrayofarray(A) (array_of_int_arrayofarray)newbuf((posint)A,(posint)sizeof(array_of_int_array))

/************  static variables ******************/

static dfa stadfa;
static dfa outdfa;
static ddelta d;

static wlist waiting, wlast;	/* waiting list */
static arrayofb_array inwaiting;	/* 0 .. numberofblocks , 0 ..alphabet_size */
static posint sizeofwaiting;	/* size of list waiting */

static posint numberofblocks;	/* number of blocks -1 */
static array_of_int blocksize;		/* 0 .. highest_state */
static array_of_int inblock;		/* 0 .. highest_state */
static array_of_int startblock;	/* 0 .. highest_state -> block */
static array_of_int nextinblock;	/* 0 .. highest_state -> block */
static array_of_int previnblock;	/* 0 .. highest_state -> block */

static array_of_int_arrayofarray inverse;	/* inverse of delta  0 .. alphabet_size,0 .. highest_state, 0 .. sizeofinverse */
				      /* state1 in array_of_int inverse[letter][state2] iff 
				       * d[letter][state1]=state2 
				       */
static array_of_int_array sizeofinverse;	/* 0 .. alphabet_size, 0 .. highest_state */

static array_of_int sizeofintersection;	/* 0 .. highest_state -> size of intersection block and inverse */
static array_of_int startintersection;	/* 0 .. highest_state -> start of intersection */
static array_of_int nextintersection;	/* 0 .. highest_state -> next in intersection */

static array_of_int jlist;		/* 0 .. highest_state */
static posint sizeofjlist;

static boole with;		/* return map: old state -> new state 
				 * only for automata where all states are reachable
				 * used for computing a minimal nfa
				 */
static boole statesdeleted;	/* in delstates a state has been deleted 
				 * in this case stadfa!=indfa after leaving delstates 
				 */
static boole freeaut;		/* input can be destroyed */
/************************************************************************************/
static void delstates()
/* delete all nonreachable states O(|Q'||A|) where Q' is the set of reachable states */
{
	mrkfin ffinal;
	ddelta fdelta;
	array_of_int map;
	b_array mark;		/* mark reachable states */
	posint state, letter, state2;
	/* variables for stack */
	posint high, next;
	array_of_int stack;
	mark = newb_array(stadfa->highest_state + 1);
	mark[stadfa->init] = TRUE;
	stack = newarray_of_int(stadfa->highest_state + 1);
	stack[0] = stadfa->init;
	high = 1;
	next = 0;
	while(high != next) {
		state = stack[next++];
		for (letter = 1; letter <= stadfa->alphabet_size; letter++) {
			state2 = d[letter][state];
			if(!mark[state2]) {
				stack[high++] = state2;
				mark[state2] = TRUE;
			}
		}
	}

	/* all reachable states are marked with TRUE 
	 * number of reachable states is high
	 */

	if(!(high == (stadfa->highest_state + 1))) {
		map = newarray_of_int(stadfa->highest_state + 1);	/* map : old - ->new */
		for (state = 0; state < high; state++)
			map[stack[state]] = state;
		high--;		/* high is highest state */
		ffinal = newfinal(high);
		fdelta = newddelta(stadfa->alphabet_size, high);
		for (state = 0; state <= high; state++) {
			ffinal[state] = stadfa->final[stack[state]];
			for (letter = 1; letter <= stadfa->alphabet_size; letter++)
				fdelta[letter][state] = map[d[letter][stack[state]]];
		}
		outdfa = newdfa();
		outdfa->init = map[stadfa->init];
		outdfa->final = ffinal;
		outdfa->delta = fdelta;
		outdfa->highest_state = high;
		outdfa->alphabet_size = stadfa->alphabet_size;
		if(freeaut) {
			freedfa(stadfa);
			dispose(stadfa);
		}
		stadfa = outdfa;
		d = fdelta;
		statesdeleted = TRUE;
	} else
		statesdeleted = FALSE;
	freebuf();
}

/************************************************************************************/

static boole initstatic()
/* TRUE iff dfa has only final states or only nonfinal states */
{
	posint bno, letter, state, state2;
	array_of_int last;
	posint numberofstates = stadfa->highest_state + 1;	/* abbreviation */
	posint numberofletters = stadfa->alphabet_size + 1;	/* abbreviation */

	/* init blocks O(|Q|) */
	last = newarray_of_int(2);
	if(with)
		inblock = newar(numberofstates);
	else
		inblock = newarray_of_int(numberofstates);
	blocksize = newarray_of_int(numberofstates);
	startblock = newarray_of_int(numberofstates);
	nextinblock = newarray_of_int(numberofstates);
	previnblock = newarray_of_int(numberofstates);
	for (state = 0; state < numberofstates; state++) {
		bno = (stadfa->final[state]) ? 0 : 1;	/* block 0 final states */
		inblock[state] = bno;
		if(blocksize[bno]++) {
			nextinblock[last[bno]] = state;
			previnblock[state] = last[bno];
		} else
			startblock[bno] = state;
		last[bno] = state;
	}
	if((!blocksize[0]) || (!blocksize[1]))	/* A* or 0  */
		return (TRUE);
	for (bno = 0; bno <= 1; bno++) {
		nextinblock[last[bno]] = startblock[bno];
		previnblock[startblock[bno]] = last[bno];
	}
	/* init inverse  O(|A||Q|) */

	inverse = newarray_of_int_arrayofarray(numberofletters);
	for (letter = 1; letter <= stadfa->alphabet_size; letter++)
		inverse[letter] = newarray_of_int_array(numberofstates);
	sizeofinverse = newarray_of_int_array(numberofletters);
	for (letter = 1; letter <= stadfa->alphabet_size; letter++)
		sizeofinverse[letter] = newarray_of_int(numberofstates);
	for (letter = 1; letter <= stadfa->alphabet_size; letter++)
		for (state = 0; state <= stadfa->highest_state; state++)
			sizeofinverse[letter][d[letter][state]]++;
	for (letter = 1; letter <= stadfa->alphabet_size; letter++)
		for (state = 0; state <= stadfa->highest_state; state++) {
			inverse[letter][state] = newarray_of_int(sizeofinverse[letter][state]);
			sizeofinverse[letter][state] = 0;
		}
	for (letter = 1; letter <= stadfa->alphabet_size; letter++)
		for (state = 0; state <= stadfa->highest_state; state++) {
			state2 = d[letter][state];
			inverse[letter][state2][sizeofinverse[letter][state2]++] = state;
		}

	/* init intersection O(1) */

	sizeofintersection = newarray_of_int(numberofstates);
	startintersection = newarray_of_int(numberofstates);
	nextintersection = newarray_of_int(numberofstates);

/* init waiting  O(|A|) */

	inwaiting = newarrayofb_array(numberofstates);
	inwaiting[0] = newb_array(numberofletters);
	inwaiting[1] = newb_array(numberofletters);
	waiting = newwlist();	/* dummy */
	wlast = waiting;
	for (letter = 1; letter <= stadfa->alphabet_size; letter++)
		for (bno = 0; bno <= 1; bno++) {
			wlast->next = newwlist();
			wlast = wlast->next;
			wlast->bno = bno;
			wlast->letter = letter;
			inwaiting[bno][letter] = TRUE;
		}
	waiting = waiting->next;	/* delete dummy */
	wlast->next = waiting;
	sizeofwaiting = stadfa->alphabet_size * 2;


/* init jlist O(1) */

	jlist = newarray_of_int(numberofstates);

	return (FALSE);
}

/************************************************************************************/

static void computeinverse(bno, letter)
/* compute all blocks that contain a state with: d[letter][state] in block bno
 * sizeofintersection[b] = | intersection[b] |
 * states in the intersection are stored using arrays nextintersection,startintersection
 */
/* O(|block bno| + |INVERSE|) */
posint bno, letter;
{
	posint state, state2;
	posint bhelp, i, j;
	sizeofjlist = 0;
	state = startblock[bno];
	for (i = 0; i < blocksize[bno]; i++) {
		for (j = 0; j < sizeofinverse[letter][state];) {
			state2 = inverse[letter][state][j++];
			bhelp = inblock[state2];
			if(sizeofintersection[bhelp]++) {
				nextintersection[state2] = startintersection[bhelp];
				startintersection[bhelp] = state2;
			} else {
				/* insert bhelp in jlist */
				jlist[sizeofjlist++] = bhelp;
				startintersection[bhelp] = state2;
			}
		}
		state = nextinblock[state];
	}
}

/************************************************************************************/

static void split(bno)
     /* block numberofblocks  = INVERSE && block bno
      * block bno  = block bno \ INVERSE
      */
     /* O( |INVERSE && block bno| ) */
posint bno;
{
	posint i, state;
	posint last;
	last = startintersection[bno];
	state = last;
	startblock[numberofblocks] = last;	/* first element in new block bno */
	for (i = 0; i < sizeofintersection[bno]; i++) {
		/* delete state in block bno */
		if(state == startblock[bno])
			startblock[bno] = nextinblock[state];
		nextinblock[previnblock[state]] = nextinblock[state];
		previnblock[nextinblock[state]] = previnblock[state];
		/* insert run in block numberofblocks */
		nextinblock[last] = state;
		previnblock[state] = last;
		last = state;
		inblock[state] = numberofblocks;
		state = nextintersection[state];
	}
	nextinblock[last] = startblock[numberofblocks];
	previnblock[startblock[numberofblocks]] = last;
	blocksize[numberofblocks] = sizeofintersection[bno];
	blocksize[bno] -= sizeofintersection[bno];
	inwaiting[numberofblocks] = newb_array(stadfa->alphabet_size + 1);
}

/************************************************************************************/

static void putinwaiting(letter, bno)
/* O(1) */
posint letter, bno;
{
	wlist whelp;
	if(!sizeofwaiting) {
		waiting->letter = letter;
		waiting->bno = bno;
		wlast = waiting;
	} else if(wlast->next == waiting) {
		whelp = newwlist();
		whelp->letter = letter;
		whelp->bno = bno;
		whelp->next = waiting;
		wlast->next = whelp;
		wlast = wlast->next;
	} else {
		wlast = wlast->next;
		wlast->letter = letter;
		wlast->bno = bno;
	}
	inwaiting[bno][letter] = TRUE;
	sizeofwaiting++;
}

/************************************************************************************/

static void makenewautomaton(simple)
/* compute new automaton using blocks 
 * O(|Q||A|) 
 */
boole simple;
{
	posint state, letter, state2;
	if(simple) {		/* only one state */
		outdfa = newdfa();
		outdfa->final = newfinal(0);
		outdfa->delta = newddelta(stadfa->alphabet_size, 0);
		outdfa->highest_state = 0;
		outdfa->alphabet_size = stadfa->alphabet_size;
		outdfa->init = 0;
		for (letter = 1; letter <= outdfa->alphabet_size; letter++)
			outdfa->delta[letter][0] = 0;
		outdfa->final[0] = stadfa->final[0];
		if(with)
			for (state = 0; state <= stadfa->highest_state; state++)
				inblock[state] = 0;
		if(freeaut || (statesdeleted)) {
			freedfa(stadfa);
			dispose(stadfa);
		}
	} else if(numberofblocks == stadfa->highest_state) {	/* no changes */
		if(with)
			for (state = 0; state <= stadfa->highest_state; state++)
				inblock[state] = state;
		if((!freeaut) && (!statesdeleted)) {	/* stadfa is the original dfa, copy stadfa to outdfa */
			outdfa = newdfa();
			outdfa->highest_state = stadfa->highest_state;
			outdfa->alphabet_size = stadfa->alphabet_size;
			outdfa->init = stadfa->init;
			outdfa->final = newfinal(outdfa->highest_state);
			outdfa->delta = newddelta(outdfa->alphabet_size, outdfa->highest_state);
			for (state = 0; state <= outdfa->highest_state; state++)
				outdfa->final[state] = stadfa->final[state];
			for (letter = 1; letter <= outdfa->alphabet_size; letter++)
				for (state = 0; state <= outdfa->highest_state; state++)
					outdfa->delta[letter][state] = stadfa->delta[letter][state];
		} else
			outdfa = stadfa;
	} else {
		outdfa = newdfa();
		outdfa->final = newfinal(numberofblocks);
		outdfa->delta = newddelta(stadfa->alphabet_size, numberofblocks);
		outdfa->alphabet_size = stadfa->alphabet_size;
		outdfa->highest_state = numberofblocks;
		outdfa->init = inblock[stadfa->init];
		for (state = 0; state <= outdfa->highest_state; state++) {
			state2 = startblock[state];	/* representative of block state */
			outdfa->final[state] = stadfa->final[state2];
			for (letter = 1; letter <= outdfa->alphabet_size; letter++)
				outdfa->delta[letter][state] = inblock[stadfa->delta[letter][state2]];
		}
		if(freeaut || (statesdeleted)) {
			freedfa(stadfa);
			dispose(stadfa);
		}
	}
	outdfa->minimal = TRUE;
}

/************************************************************************************/

static void mindfa()
{
	posint letter, bno;
	posint bl, j;
	d = stadfa->delta;
	if(!with)
		delstates();	/* delete non-reachable states */
	if(stadfa->highest_state == 0) {
		stadfa->minimal = TRUE;
		outdfa = stadfa;
		return;
	}
	if(initstatic())	/*  1 get memory for arrays and initialize static variables */
		makenewautomaton(TRUE);	/* automaton is simple |Q| = 1 */
	else {
		numberofblocks = 1;	/* 2 */
		while(sizeofwaiting) {	/* 3 */
			bno = waiting->bno;	/* 4 */
			letter = waiting->letter;
			inwaiting[bno][letter] = FALSE;
			waiting = waiting->next;
			sizeofwaiting--;
			computeinverse(bno, letter);	/* 5 */
			for (j = 0; j < sizeofjlist;) {	/* 6 */
				bl = jlist[j++];
				if(!(sizeofintersection[bl] == blocksize[bl])) {
					numberofblocks++;	/* tick(); *//* 7 */
					split(bl);	/*9,10 */
					for (letter = 1; letter <= stadfa->alphabet_size; letter++)
						if(inwaiting[bl][letter])
							putinwaiting(letter, numberofblocks);
						else if(blocksize[bl] >= blocksize[numberofblocks])
							putinwaiting(letter, numberofblocks);
						else
							putinwaiting(letter, bl);
				}
				sizeofintersection[bl] = 0;
			}
		}
		makenewautomaton(FALSE);
	}
	freebuf();
}

/* return minimal automaton
 * if free then the memory of indfa is free after the procedure
 * if not free indfa isn't changed after computation 
 */
dfa dfamdfa(dfa indfa, boole free)
{
	stadfa = indfa;
	with = FALSE;
	freeaut = free;
	mindfa();
	return (outdfa);
}

/* this function does not seem to be called anywhere ... Burak
dfa dfammdfa(indfa,transformation)
dfa indfa;
array_of_int *transformation;
{
 stadfa=indfa;
 with=TRUE;
 freeaut=FALSE;
 statesdeleted=FALSE;
 / * copy automaton * /
 mindfa();
 *transformation=inblock;
 return(outdfa);
}

*/
