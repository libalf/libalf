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

/* nfa2mnfa_sol.c
 */

#include <amore/nfa2mnfa_sol.h>
#include <amore/unaryB.h>	/* revnfa */

/***************************************************************************/
/* Procedures included:                                                    */
/* nfa calc_sol                                                            */
/***************************************************************************/
static void calculate_initial_terms();
static void calculate_other_terms();
static void define_term();
static void correct_list();
static boole compare_term();
static void delete_term();
static void append_term();
static void init_termlist();
static void copy_list();
static ptset calculate_minimal_set();
static boole find_next_partial_solution();
/* This function might caus an error.  If this happens, the function
   returns FALSE.  You consider this dirty?  Find another way to get along! 
   oma, 20.09.96 */
static boole speed_up();
static boole cut_is_empty();
static boole jump_back();
/* ditto, oma, 20.09.96 */
static void go_back();
static void clear_level();
static void deletevar();
static void insertvar();
static nfa calculate_d();
static boole grid_is_covered();
/***************************************************************************/
/* The following variables are used for calculating the      */
/* boolean sums.                                             */
static array term_array;
    /* Term_array is used for storing the uncomplemented     */
    /* variables of the current term.                        */
static posint actcomplemented;
    /* If the term currently worked with  has a complemented */
    /* variable then actcomplemented is set to the number of */
    /* the variable. Otherwise, actcomplemented is equal to  */
    /* GITTER.                                               */
static termlist first_term;
    /* The last term having another complemented variable    */
    /* than actcomplemented or no complemented variable at   */
    /* all is first_term.                                    */
static posint actcount;
    /* The number of terms containing the current            */
    /* complemented variable (its number is actcomplemented) */
    /* is actcount.                                          */
static posint nv;
    /* The number of uncomplemented variables of the term    */
    /* currently worked with is nv.                          */
static term help_term;
    /* The boolean sum currently worked with is stored in    */
    /* help_term. The number of the only complemented        */
    /* variable that might be necessary is help_term->k. The */
    /* numbers of the uncomplemented variables are stored in */
    /* help_term->grset; their number is help_term->ngrset.  */
    /* If testelem(i,help_term->grset) then the (i+1)-th     */
    /* non-essential grid is an element of the sum (through  */
    /* its uncomplemented variable).                         */

/* The following variables are used for finding a minimal    */
/* solution in calculate_minimal_set.                        */
static arrayofarray T;
    /* T is used for storing the variables of the boolean    */
    /* sums in a matrix. T[i][j](==k) is the (j+1)-th        */
    /* variable of the i-th term. It is k<REST since only    */
    /* the uncomplemented variables are stored in T. The     */
    /* complemented variable is indirectly stored in start.  */
    /* T[i][0] first variable                                */
static array NV;
    /* NV[i] is the number of uncomplemented variables of    */
    /* the i-th boolean sum.                                 */
static array start;
    /* The first term with complement of i is start[i]. The  */
    /* terms 1 to start[0]-1 are the only ones without a     */
    /* complemented variable. To find the end start[REST] is */
    /* set equal to number+1.                                */
static array nextlevel;
    /* Nextlevel[i] is the i-th term a variable has to be    */
    /* taken from.                                           */
static array startlevel;
    /* If startlevel[i]==0 then the variable i is not part   */
    /* of the partial solution. If startlevel[i]==k>0 then   */
    /* the variable i is part of the partial solution and    */
    /* has first been taken from the (nextlevel[i])-th term. */
static array lock;
    /* If lock[i]!=0, then the  variable represented by i    */
    /* must not be used when trying to find a new minimal    */
    /* solution. The variable has been locked by sum lock[i] */
    /* because a variable with a lower index has been added  */
    /* from that term to the partial solution. If lock[i]==0 */
    /* then variable i is not locked and can be used for the */
    /* solution.                                             */
static posint maxlevel;
    /* maxlevel is the first free position in nextlevel.     */
static posint actlevel;
    /* nextlevel[actlevel] is the next term to deal with.    */
static posint novar;
    /* novar is the number of variables in the partial       */
    /* solution.                                             */
static array collection;
    /* When trying to find a solution, collection is used    */
    /* for storing the current pattern of the variables of   */
    /* the boolean sums. Thus, collection[i] represents the  */
    /* variable taken the i-th boolean sum looked at. If     */
    /* collection[i]==0 then no new variable has been taken  */
    /* from that term. If collection[i]==k>0 then the k-th   */
    /* variable has been added from the term to the partial  */
    /* solution.                                             */

static posint ai, am;		/* Needed for add & sub. */
static posint number, minimal, nbr, nbg, GITTER, NECESSARY, REST;
    /* Number is the number of boolean sums;                 */
    /* minimal is the number of uncomplemented variables in  */
    /* the best solution;                                    */
    /* nb.=number of bytes to form a set with . elements;    */
    /* GITTER is the number of grids;                        */
    /* NECESSARY is the number of essential grids;           */
    /* REST=GITTER-NECESSARY;                                */
static termlist first, last, run;
    /* Pointer to elements of the list of boolean sums.      */
static ptset solution;
    /* The set of solution is used for storing the numbers   */
    /* of the uncomplemented variables of a minimal          */
    /* solution. These variables have numbers from 0 to REST */
    /* - 1 because only grids that are not necessary are     */
    /* represented in the boolean sums.                      */

/***************************************************************************/
/* calc_sol                                                                */
/***************************************************************************/
/* This procedure tries to find a nfa result with less states than f and   */
/* L(result)==L(na) (see nfamnfa.c). This is done by searching for a       */
/* minimal closed cover as a subset of the grids.                          */
/***************************************************************************/
nfa calc_sol(delta, version, fI, fII, G, N, R, MAXI, f, i_gr, f_gr)
ptset **delta;			/* Delta of the fundamental table.          */
boole version;			/* Version iff calculation with rows;       */
		 /* !version iff calculation with columns.   */
		 /* This is only relevant for the automaton  */
		 /* to be returned.                          */
gridlist fI, fII;		/* Pointer to grids; fI->info is the first  */
		 /* grid and fII->info the first non-        */
		 /* essential grid.                          */
posint G, N, R, MAXI;		/* G is the number of grids;               */
		 /* N is the number of essential grids;      */
		 /* R=GITTER-NECESSARY;                      */
		 /* MAXI is the number of states of the best  */
		 /* automaton so far - 1.                    */
nfa f;				/* f is the minimal dfa with additional     */
		 /* initial states.                          */
ptset i_gr, f_gr;		/* Testelem(i,i_gr) iff grid i considered   */
		 /* as a state of the fundamental table is   */
		 /* an initial state.                        */
		 /* Testelem(i,f_gr) iff grid i considered   */
		 /* as a state of the fundamental table is a */
		 /* final state.                             */
{
	nfa result = NULL;
	nbr = R / SIZEOFBYTE + 1;
	nbg = G / SIZEOFBYTE + 1;
	number = 0;
	GITTER = G;
	NECESSARY = N;
	REST = R;
	term_array = newarray(REST);
	start = newarray(REST + 1);
	new_term(help_term);
	actcomplemented = GITTER;
	/* Calculate list of boolean sums. */
	calculate_initial_terms(fI, f, i_gr);
	calculate_other_terms(delta, f->alphabet_size, fI);
	/* The list is calculated. Its first elements are terms    */
	/* with no complemented variables, terms with complemented */
	/* variables following only thereafter.                    */
	if(number == 0) {	/* Only essential grids are needed. */
		newpattern(solution, nbr);
	} else {
		minimal = MAXI + 1 - NECESSARY;
		/* Minimal is the number of non-essential grids to beat. */
		copy_list();
		solution = calculate_minimal_set();
	}


	if(minimal == (MAXI + 1 - NECESSARY))
		/* It is not necessary to calculate the automaton        */
		/* defined by the solution because there exists an       */
		/* automaton that is already known and has an equal      */
		/* number of states.                                     */
		return (result);
	else {			/* The automaton has to be calculated. */
		result = calculate_d(delta, i_gr, f_gr, fI, f);
		if(version) {	/* Otherwise dual language.   */
			freebuf();
			return (nfa) revnfa(result);
		} else {
			freebuf();
			return result;
		}
	}
}

/***************************************************************************/
/* calculate_initial_terms                                                 */
/***************************************************************************/
/* Calculate_initial_terms defines the sum(s) of boolean variables formed  */
/* by the grids covering the initial state(s) of the fundamental table.    */
/***************************************************************************/
static void calculate_initial_terms(fI, f, i_gr)
gridlist fI;
nfa f;
ptset i_gr;
{
	gridlist check;
	posint i, j;
	boole test;

	for (i = 0; i <= f->highest_state; i++)
		if(isfinal(f->infin[i])) {	/* I is an initial state of the dual automaton to f.   */
			/* Now try to find a necessary grid j with             */
			/* testelem(j,i_gr) that covers i.                     */
			for (j = 0, check = fI, test = FALSE; (j < NECESSARY) && (!test); j++, check = check->next)
				if(testelem(j, i_gr->grset) && testelem(i, check->info->col))
					test = TRUE;	/* There exists such a grid. Then no    */
			/* term has to be defined.              */
			if(!test) {	/* A new term has to be defined. Check->info is the  */
				/* first non-essential grid (that is check==fII) and */
				/* j==NECESSARY.                                     */
				/* Now find the grids j with testelem(j,i_gr) that   */
				/* cover i.                                          */
				help_term->k = GITTER;	/* The term has no complemented */
				/* variable.                    */
				for ( /* j=NECESSARY,check=fII */ ; j < GITTER; j++, check = check->next)
					if((testelem(j, i_gr->grset)) && (testelem(i, check->info->col))) {	/* Found a new variable for the sum. */
						add((j - NECESSARY), help_term->set);
						help_term->nset++;
					}
				/* If possible or necessary, help_term is added to the list. */
				correct_list();
			}
		}
}

/***************************************************************************/
/* calculate_other_terms                                                   */
/***************************************************************************/
/* Calculate_other_terms tests for each grid i and each letter j if a      */
/* boolean sum could be added to the list. For each grid it is checked if  */
/* the grid's columns are a subset of delta[j][i]->grset. If so, the grid  */
/* is added to helpII->grset. Then for each k with testelem(k,delta[j][i]  */
/* ->grset) it is tested if there is a necessary grid l with testelem(l,   */
/* helpII->grset) and l containing column k. If so, no term has to be      */
/* defined. Otherwise define_term is called.                               */
/***************************************************************************/
static void calculate_other_terms(delta, le, fI)
ptset **delta;
posint le;			/* Le is the number of letters of f. */
gridlist fI;
{
	posint i, j, k, l, s, t;
	gridlist check;
	boole test;
	ptset helpII;		/* For covered grids. */
	boole go_on;

	go_on = TRUE;
	newpattern(helpII, nbg);
	for (i = 0; (i < GITTER) && go_on; i++)
		if((i == NECESSARY) && (number == 0))
			go_on = FALSE;
	/* The terms that would be defined from   */
	/* now on contain a complemented variable. */
	/* Since the list of boolean sums is      */
	/* empty so far, the empty set is the     */
	/* minimal solution. This cannot be       */
	/* changed by calculating the other terms */
	/* and searching for a better solution.   */
		else
			for (j = 0; j < le; j++)
				if((t = delta[j][i]->ngrset) != 0) {	/* There are grids that can be covered. */
					for (l = 0, check = fI, s = 0; l != GITTER; l++, check = check->next) {
						if(grid_is_covered(delta, check->info, j, i)) {	/* Found a grid whose columns are  */
							/* a subset of delta[j][i]->grset. */
							add(l, helpII->grset);
							helpII->ngrset++;
						}
						if(l == NECESSARY - 1)
							s = helpII->ngrset;	/* S is the number of        */
						/* essential grids in helpII. */
					}
					s = helpII->ngrset - s;	/* S is the number of          */
					/* non-essential grids in      */
					/* helpII.                     */
					/* Now, for each k with testelem(k,                */
					/* delta[j][i]->grset) it is tested if there       */
					/* exists a necessary grid l with l's columns      */
					/* being a subset of delta[j][i]->grset and k an   */
					/* element of l's columns. If so, no term has to   */
					/* be defined.                                     */
					if(s != 0)
						/* HelpII->grset contains s non-essential grids. */
						for (k = 0; t != 0; k++)
							if(testelem(k, delta[j][i]->grset)) {
								t--;
								for (l = 0, check = fI, test = FALSE; ((!test) && (l < NECESSARY));
								     l++, check = check->next)
									if(testelem(l, helpII->grset))
										if(testelem(k, check->info->col))
											test = TRUE;
								/* No term has to be defined, because  */
								/* an essential grid for which the     */
								/* above condition holds is found.     */
								if(!test)
									define_term(check, helpII, i, k, s);
								/* Define a new term and (possibly) add it   */
								/* to the list.                              */
							}
					/* Now clear helpII for the next combination of a  */
					/* grid and a letter.                              */
					for (k = 0; helpII->ngrset != 0; k++)
						if(testelem(k, helpII->grset)) {
							helpII->ngrset--;
							sub(k, helpII->grset);
						}
				}	/* End of if ((t=delta[j][i]->ngrset)!=0). */
}

/***************************************************************************/
/* define_term                                                             */
/***************************************************************************/
/* The procedure defines a boolean term. This term has a variable for each */
/* grid that is an element of helpII and covers column k. These variables  */
/* are uncomplemented. A complemented variable is added if i>=NECESSARY,   */
/* that is if the grid i is a non-essential grid. If variable i appears    */
/* uncomplemented also no sum is necessary and the computation is stopped. */
/* Otherwise, the procedure correct_list is called so that the sum can be  */
/* added to the list.                                                      */
/***************************************************************************/
static void define_term(check, helpII, i, k, s)
gridlist check;			/* Check=fII.                      */
ptset helpII;			/* List of covered grids.          */
posint i, k, s;			/* i is the number of the grid; k is the     */
		/* number of the state to cover; s is the    */
		/* number of non-essential grids in helpII.  */
{
	posint l, m, variable;
	boole test;		/* Test iff the sum would contain the number */
	/* of the variable corresponding to grid i   */
	/* both complemented and uncomplemented.     */
	/* Then no term is necessary and the         */
	/* calculation is stopped.                   */

	if(i >= NECESSARY)
		help_term->k = i - NECESSARY;
	else
		help_term->k = GITTER;
	/* If a complemented variable is necessary, its number is  */
	/* stored in help_term->k as the number of the             */
	/* corresponding grid starting to count from the first     */
	/* non-essential grid. For example, if the grid is the     */
	/* first non-essential grid then help_term->k is set to 0. */
	/* Otherwise, help_term->k is set to GITTER, because the   */
	/* number of grids is GITTER - 1 and so no non-essential   */
	/* grid has the number GITTER.                             */
	for (l = NECESSARY, m = s, test = FALSE; (m != 0) && !test; l++, check = check->next)
		if(testelem(l, helpII->grset)) {
			m--;
			/* Check->info is a grid whose columns are covered by  */
			/* delta[j][i]->grset: compare to calculate_other_terms */
			if(testelem(k, check->info->col)) {	/* The grid covers column k and its variable is to */
				/* be added to the sum.                            */
				add((l - NECESSARY), help_term->set);
				help_term->nset++;
				if(help_term->k == l - NECESSARY)
					test = TRUE;
				/* No further calculation is necessary, because  */
				/* the sum would contain a variable and its      */
				/* complement.                                   */
			}
		}
	if(!test)
		correct_list();
	/* Try if the sum should be added to the list. */
	else
		/* Clear help_term for the next boolean sum. */
		for (variable = 0; help_term->nset != 0; variable++)
			if(testelem(variable, help_term->set)) {
				help_term->nset--;
				sub(variable, help_term->set);
			}
}

/***************************************************************************/
/* correct_list                                                            */
/***************************************************************************/
/* This procedure checks if the boolean sum help_term should be added to   */
/* the list of terms or if the list has to be altered. If so, this is done.*/
/***************************************************************************/
static void correct_list()
{
	posint count, variable, oldnumber;
	termlist run;		/* Run is the term help_term is currently   */
	/* compared with.                           */
	boole test, found;

	nv = help_term->nset;	/* Number of uncomplemented variables. */
	help_term->nset = 0;	/* Clear help_term.                    */
	/* Now copy information from help_term to term_array.      */
	count = 0;
	variable = 0;
	while(count < nv) {
		while(!(testelem(variable, help_term->set)))
			variable++;	/* Found a variable. */
		sub(variable, help_term->set);	/* Clear help_term. */
		term_array[count] = variable;
		variable++;
		count++;
	}
	if(help_term->k == GITTER) {	/* The current term has no complemented variable. So far */
		/* only terms with no complemented variables exist. The  */
		/* current one is compared with each other one and, if   */
		/* necessary, the list of terms is altered.              */
		run = first;
		found = FALSE;
		oldnumber = number;	/* Oldnumber is used for storing the   */
		/* old number of elements of the list  */
		/* of boolean sums. Thus it is easier  */
		/* to find the end of the list.        */
		for (count = 0; ((!found) && (count < oldnumber)); count++, run = run->next) {	/* Compare term_array with run->info. */
			if(nv < run->nv) {
				test = compare_term(term_array, run->info, nv, run->nv);
				if(test)
					delete_term(run);	/* The term run->info can be     */
				/* deleted because every         */
				/* variable of the term          */
				/* term_array is also part of    */
				/* run->info.                    */
			} else
				found = compare_term(run->info, term_array, run->nv, nv);
			/* Found iff run->info is equal */
			/* or smaller than term_array.  */
		}
		if(!found) {	/* Term_array is a new term and has to be added to the */
			/* list.                                               */
			if(number == 0)
				init_termlist();	/* First element.    */
			else
				append_term();	/* Some other exist. */
		}
	} else {		/* Since help->k!=GITTER the currently worked with term  */
		/* has a complemented variable.                          */
		if(help_term->k != actcomplemented) {	/* The term has a new complemented variable. */
			if(actcomplemented == GITTER) {	/* No complemented variable so far. */
				for (count = 0; count <= help_term->k; count++)
					start[count] = number + 1;
			} else {
				for (count = actcomplemented + 1; count <= help_term->k; count++)
					start[count] = number + 1;
			}
			actcount = 0;
			actcomplemented = help_term->k;
			/* The number of the complemented variable is stored   */
			/* in actcomplemented. There are actcount terms in the */
			/* list with this complemented variable.               */
		}
		/* Now compare term_array with all terms without         */
		/* a complemented variable.                              */
		run = first;
		found = FALSE;
		for (count = 1; ((!found) && (count < start[0])); count++, run = run->next) {
			if(run->nv <= nv)
				found = compare_term(run->info, term_array, run->nv, nv);
			/* Found iff term_array can be ignored. */
		}
		if(!found) {	/* Compare term_array with the other terms with the    */
			/* same complemented variable.                         */
			if(actcount == 0) {	/* No other term with the same complemented variable */
				/* so far. Simply add term_array to the list.        */
				first_term = last;	/* Last term with other or no       */
				/* complemented variable.           */
				append_term();
				actcount = 1;
			} else {	/* There are some other terms with the same          */
				/* complemented variable term_array can be compared  */
				/* with.                                             */
				oldnumber = actcount;	/* To find the end. */
				run = first_term->next;	/* Run is now the first term   */
				/* with the complemented       */
				/* variable in question.       */
				found = FALSE;
				for (count = 0; ((!found) && (count < oldnumber)); count++, run = run->next) {
					if(nv < run->nv) {
						test = compare_term(term_array, run->info, nv, run->nv);
						if(test) {	/* The term run is to be deleted. */
							delete_term(run);
							actcount--;
						}
					} else
						found = compare_term(run->info, term_array, run->nv, nv);
				}
				if(!found) {	/* The term term_array is unknown and added to the */
					/* list.                                           */
					append_term();
					actcount++;
				}
			}
		}		/* End of (!found).                  */
	}			/* End of else (help_term->k==GITTER). */
	help_term->k = 0;	/* Clear help_term.      */
}

/***************************************************************************/
/* compare_term                                                            */
/***************************************************************************/
/* This function returns TRUE iff array1 is a subset of array2.            */
/***************************************************************************/
static boole compare_term(array1, array2, NV1, NV2)
array array1, array2;
posint NV1, NV2;		/* Number of variables in arrays. */
{
	posint count1, count2, diff;
	diff = NV2 - NV1 + 1;	/* Number of variables in array2-array1 +1. */
	for (count1 = 0, count2 = 0; (count1 < NV1) && (count2 < NV2); count2++) {
		if(diff == 0)
			return FALSE;	/* Not enough variables left in array2.  */
		if(array1[count1] < array2[count2])
			return FALSE;	/* Array1 has a variable that array2     */
		/* doesn't contain.                      */
		else {
			if(array1[count1] == array2[count2])
				count1++;	/* Found an element that is in both      */
			/* arrays.                               */
			else
				diff--;	/* Array2[count2]<array1[count1] means   */
			/* that array2 has a variable that       */
			/* array1 hasn't.                        */
		}
	}
	if(count1 == NV1)
		return TRUE;	/* Array1 is a subset of array2.         */
	else
		return FALSE;
}

/***************************************************************************/
/* append_term                                                             */
/***************************************************************************/
/* Append_term adds the new term term_array to the list.                   */
/***************************************************************************/
static void append_term()
{
	posint count;
	new_termlist(last->next);	/* New last element. */
	last->next->bef = last;
	last = last->next;
	last->info = newarray(nv);
	/* Now copy information. */
	for (count = 0; count < nv; count++)
		last->info[count] = term_array[count];
	last->nv = nv;
	number++;		/* One element more. */
}

/***************************************************************************/
/* init_termlist                                                           */
/***************************************************************************/
/* Init_termlist defines a new termlist with term term_array as its first  */
/* (and only) element.                                                     */
/***************************************************************************/
static void init_termlist()
{
	posint count;
	number = 1;
	new_termlist(first);
	last = first;
	first->info = newarray(nv);
	/* Now copy information. */
	first->nv = nv;
	for (count = 0; count < nv; count++)
		first->info[count] = term_array[count];
}

/***************************************************************************/
/* delete_term                                                             */
/***************************************************************************/
/* Delete_term removes the termlist run from the list of terms.            */
/***************************************************************************/
static void delete_term(run)
termlist run;
{
	if(number != 1) {
		if(run->next == NULL) {	/* Run is the last element.  */
			last = last->bef;
			last->next = NULL;
		} else if(run->bef == NULL) {	/* Run is the first element. */
			first = first->next;
			first->bef = NULL;
		} else {	/* Run has a termlist before and after it. */
			run->next->bef = run->bef;
			run->bef->next = run->next;
		}
	}
	number--;		/*  One element less.  */
}

/***************************************************************************/
/* copy_list                                                               */
/***************************************************************************/
/* This procedure is used for copying the terms in the list of terms into  */
/* the matrix T and the array NV. This eases the calculation of a minimal  */
/* solution.                                                               */
/***************************************************************************/
static void copy_list()
{
	posint count;
	T = newarrayofarray(number + 1);
	NV = newarray(number + 1);
	run = first;
	/* Now copy information. */
	for (count = 1; count <= number; count++, run = run->next) {
		NV[count] = run->nv;
		T[count] = run->info;
	}
	/* The array start has to be altered. */
	if(actcomplemented == GITTER)
		start[0] = number + 1;
	/* There are no terms with a         */
	/* complemented variable. Start[i]   */
	/* with i>0 will not be used.        */
	else
		for (count = actcomplemented + 1; count <= REST; count++)
			start[count] = number + 1;
}

/***************************************************************************/
/* calculate_minimal_set                                                   */
/***************************************************************************/
/* Calculate_minimal_set finds a minimal solution for the product of the   */
/* boolean sums and returns the set of uncomplemented variables.           */
/***************************************************************************/
/* Modified by oma, 20.09.96  */
static ptset calculate_minimal_set()
{
	posint variable, i;
	ptset current_best;	/* Used for storing the best solution so far. */
	lock = newarray(REST);
	startlevel = newarray(REST);
	nextlevel = newarray(number + 2);
	collection = newarray(number + 2);
	novar = 0;
	actlevel = 1;
	/* start with all terms without complemented variable */
	for (i = 1; i < start[0]; i++)
		nextlevel[i] = i;
	maxlevel = start[0];
	newpattern(current_best, nbr);
	while(actlevel > 0)
		/* Look at the current term and alter the partial        */
		/* solution if necessary.                                */
	{
		if(!find_next_partial_solution())
			return (current_best);
		/*  find_next_partial_solution() did not work properly since jump_back
		   tried to jump back to level 0, which does not exists.  Since oma does
		   not understand this, we simply stop the algorithm and take the currently
		   best solution. */
		/* this is a dirty trick added by oma on 20.09.96. */

		/* no problem occured, so proceed as Kahlert implemented his algorithm */
		if(actlevel == maxlevel) {	/* Found a new best so far solution. Now save it. */
			minimal = current_best->ngrset = novar;
			for (variable = 0; variable < REST; variable++)
				if(startlevel[variable]) {
					add(variable, current_best->grset);
				} else {
					sub(variable, current_best->grset);
				}
			if(minimal == 1)
				return current_best;	/* Nothing better to find. */
			else {	/* Start searching for another solution. Therefore  */
				/* remove one variable from the solution and find   */
				/* the term the second-from-last one came from.     */
				while(collection[actlevel] == 0)
					actlevel--;
				clear_level();	/* Found the first variable. Now go  */
				/* back further.                     */
				actlevel--;
				while(collection[actlevel] == 0)
					actlevel--;
				/* Found the the term the second-from-last variable */
				/* has been taken from.                             */
			}
		} else {	/* There are still terms variables have to be taken    */
			/* from. These are terms with a complemented variable  */
			/* that is part of the partial solution or terms left  */
			/* without a complemented variable.                    */
			if((minimal - 1) == novar) {	/* Try to find a solution by not adding another     */
				/* (uncomplemented) variable to the solution.       */
				if(speed_up()) {	/* Found a new best so far solution. Save it.    */
					minimal = current_best->ngrset = novar;
					for (variable = 0; variable < REST; variable++)
						if(startlevel[variable]) {
							add(variable, current_best->grset);
						} else {
							sub(variable, current_best->grset);
						}
					if(minimal == 1)
						return current_best;
					else
						/* Start searching for another solution.       */
						/* Therefore remove one variable from the      */
						/* solution and find the term the second-from- */
						/* last one came from.                         */
					{
						actlevel--;
						clear_level();	/* Found the first variable.   */
						/* Now go back further.        */
						actlevel--;
						while(collection[actlevel] == 0)
							actlevel--;
						/* Found the term the second-from-last */
						/* variable has been taken from.       */
					}
				} else
					actlevel--;	/* Found nothing better. Simply go   */
				/* back one level.                   */
			}
		}
	}
	return current_best;
}

/***************************************************************************/
/* find_next_partial_solution                                              */
/***************************************************************************/
/* Find_next_partial_solution tries to add a variable from the current     */
/* term to the partial solution stored in the array startlevel.            */
/***************************************************************************/
/*  Note by oma:  This procedure (or any one else) has a problem, which occurs 
    for example when treating the language
    (a a A A A) U (a b A A a) U (b A A a a)   */
/* This error occurs inside jump_back().  When this happens, we exit 
   find_next_partial_solution() and signal this situation by returning FALSE */
/* This is a dirty trick added by oma on 20.09.96 */
static boole find_next_partial_solution()
{
	boole notfound = TRUE;
	posint position, variable, level;
	level = nextlevel[actlevel];	/* The level-th term is worked  */
	/* with. It is the actlevel-th  */
	/* one looked at for this       */
	/* partial solution.            */
	if(collection[actlevel] == 0) {	/* No variable has been taken from the term. */
		if(!cut_is_empty(level))
			actlevel++;
		/* No new variable has to be added to the  */
		/* list. Simply go to the next level.      */
		else {
			position = 0;
			while((position < NV[level]) && (notfound))
				if(!lock[T[level][position]])
					notfound = FALSE;
			/* The variable T[level][position]       */
			/* can be added to the partial solution. */
				else
					position++;	/* Try next variable.              */
			if(notfound) {
				if(!jump_back())
					return FALSE;
				/* Go back as far as necessary and */
				/* alter the solution.             */
				/* .. and exit if jump_back() caused problems */
				/* this was added by oma, 20.09.96 */
			} else {	/* Add T[level][position] to the partial solution.   */
				variable = T[level][position];
				insertvar(variable);	/* The terms with the           */
				/* complement of the variable   */
				/* have to be taken care of and */
				/* the variable is added to the */
				/* partial solution.            */
				position++;
				collection[actlevel] = position;
				for (; position < NV[level]; position++)
					if(lock[T[level][position]] == 0)
						lock[T[level][position]] = actlevel;
				/* A variable had to be locked. */
				actlevel++;	/* Now look at next term. */
			}
		}
	} else {		/* The variable T[level][collection[actlevel]-1] is part */
		/* of the partial solution and has been taken from the   */
		/* term in question. Now look for a new variable.        */
		position = collection[actlevel];
		while((position < NV[level]) && (notfound))
			if((lock[T[level][position]]) && (lock[T[level][position]] != actlevel))
				position++;	/* The variable is locked - look at next one. */
			else
				notfound = FALSE;	/* Found a variable to add to the partial solution. */
		if(notfound)
			go_back();	/* Go back just one level. */
		else {		/* Add the variable T[level][collection[actlevel]-1]      */
			/* to the partial solution after first eliminating the    */
			/* old one taken from the current term.                   */
			variable = T[level][collection[actlevel] - 1];
			deletevar(variable);	/* There is no need to look at       */
			/* the terms with the complement     */
			/* of the variable to be removed.    */
			variable = T[level][position];
			insertvar(variable);	/* The terms with the complement     */
			/* of the variable have to be        */
			/* taken care of and the variable    */
			/* is added to the partial solution. */
			lock[variable] = 0;	/* Unlock the variable. */
			collection[actlevel] = position + 1;
			actlevel++;	/* Next level. */
		}
	}
	return TRUE;		/* added by oma to signal that nothing is wrong */
}

/***************************************************************************/
/* speed_up                                                                */
/***************************************************************************/
/* This function is called when the partial solution has minimal-1         */
/* variables. To find a better solution than the one already known no      */
/* further variable must be added to the partial solution. If this is      */
/* possible, speed_up returns TRUE, and FALSE, otherwise.                  */
/***************************************************************************/
static boole speed_up()
{
	posint i;
	/* First test all terms left with a complemented variable. */
	/* The numbers of these terms are saved in nextlevel.      */
	for (i = actlevel; i < maxlevel; i++)
		if(cut_is_empty(nextlevel[i]))
			return FALSE;
	return TRUE;		/* A better solution has been found. */
}

/***************************************************************************/
/* cut_is_empty                                                            */
/***************************************************************************/
/* This function returns TRUE iff the term with number term and the        */
/* partial solution have no variable in common.                            */
/***************************************************************************/
static boole cut_is_empty(terme)
posint terme;
{
	posint position;
	for (position = 0; position < NV[terme]; position++)
		if(startlevel[T[terme][position]])
			return FALSE;
	return TRUE;
}

/***************************************************************************/
/* jump_back                                                               */
/***************************************************************************/
/* This procedure is called when all the variables of the term currently   */
/* worked with are locked und could thus not be added to the partial       */
/* solution. Jump_back goes back to the first term which locked one of     */
/* these variables or from which the complemented variable of the current  */
/* term has taken from in its uncomplemented form. Then the terms between  */
/* the found term and the one started with are cleared and the partial     */
/* solution is altered. If necessary, the locked variable is added to the  */
/* partial solution.                                                       */
/***************************************************************************/
/* Note by oma, 20.09.96: If jump_back() causes problems, we leave it and
   signal this by returning FALSE.  If nothing goes wrong, we signal this 
   by returning TRUE.  */
static boole jump_back()
{
	posint position, actterm, maximum, newvariable;
	posint variable, oldvariable, level, helplevel;
	boole islocked;
	maximum = 0;
	level = nextlevel[actlevel];
	islocked = TRUE;
	/* At first the term with the highest index locking a      */
	/* variable of the level-th term is searched for.          */
	for (position = 0; position < NV[level]; position++) {
		actterm = lock[T[level][position]];
		/* The actterm-th term locks the variable                */
		/* T[level][position] and makes it thus not available    */
		/* for the partial solution.                             */
		if(maximum < actterm) {	/* Found a term with a higher number. */
			maximum = actterm;
			newvariable = T[level][position];
		}
	}
	if(nextlevel[actlevel] >= start[0])
		/* The term started with has a complemented variable.    */
		/* Now test if a term above the maximum-th one exists    */
		/* from which the uncomplemented variable has been added */
		/* to the partial solution.                              */
		for (helplevel = actlevel - 1; helplevel > maximum; helplevel--)
			if(collection[helplevel] != 0) {
				variable = T[nextlevel[helplevel]][collection[helplevel] - 1];
				if((start[variable] <= actlevel) && (actlevel < start[variable + 1])) {
					maximum = helplevel;	/* Found such a term. */
					islocked = FALSE;
				}
			}
	/* Now go back to the found term. */
	for (; actlevel > maximum; actlevel--)
		clear_level();
	if(islocked) {		/* Take a new variable (the locked one) from the         */
		/* level-th term.                                        */
		position = collection[actlevel];
		level = nextlevel[actlevel];
		/* Modification by oma on 20.09.96: if level==0, something went wrong. */
		/* we leave jump_back() and signal this error by returning FALSE */
		if(!level)
			return FALSE;
		/* Now we proceed as usual, oma */
		oldvariable = T[level][position - 1];
		deletevar(oldvariable);
		insertvar(newvariable);
		while(T[level][position] != newvariable) {
			if(lock[T[level][position]] == actlevel)
				lock[T[level][position]] = 0;	/* Unlock a variable. */
			position++;
		}
		lock[newvariable] = 0;	/* Unlock the variable added to the */
		/* partial solution.                */
		collection[actlevel] = position + 1;
		actlevel++;	/* Next term. */
	}
	return TRUE;		/* signal that nothing went wrong, oma */
}

/***************************************************************************/
/* go_back                                                                 */
/***************************************************************************/
/* Go_back clears the current term, then goes back and tests if the term   */
/* looked at contains a variable that could be added to the partial        */
/* solution. The procedure goes back as far as necessary to find a new     */
/* starting point for finding a (new) solution.                            */
/***************************************************************************/
static void go_back()
{
	posint position, variable, level;
	boole no_stop;
	level = nextlevel[actlevel];	/* The current term is the level-th one. */
	no_stop = TRUE;
	/* First unlock the variables of the term currently worked  with. */
	for (position = collection[actlevel]; position < NV[level]; position++)
		if(lock[T[level][position]] == actlevel)
			lock[T[level][position]] = 0;	/* A variable has been unlocked. */
	/* Now delete the variable from the partial solution. */
	variable = T[level][collection[actlevel] - 1];
	deletevar(variable);
	collection[actlevel] = 0;
	actlevel--;		/* Go back to the next term. */
	while((actlevel > 0) && no_stop)
		if(collection[actlevel] == 0)
			actlevel--;	/* Nothing to do because no new variable   */
	/* has been taken from the term.           */
		else {
			level = nextlevel[actlevel];
			if(collection[actlevel] == NV[level]) {	/* No new variable to be found in the level-th term. */
				/* Delete the variable taken from the term from the  */
				/* partial solution and go back one level.           */
				variable = T[level][NV[level] - 1];
				deletevar(variable);	/* Delete a few terms from the  */
				/* list and the variable var    */
				/* from the partial solution.   */
				collection[actlevel] = 0;
				actlevel--;	/* Go back one level. */
			} else
				no_stop = FALSE;	/* The further calculation will now   */
			/* be done in the other procedures.   */
		}
}

/***************************************************************************/
/* clear_level                                                             */
/***************************************************************************/
/* clear_level clears the partial solution stored in the array startlevel  */
/* by eliminating the variable taken from the nextlevel[actlevel]-th term. */
/* Furthermore lock, collection and maxlevel are accustomed.               */
/***************************************************************************/
static void clear_level()
{
	posint position, level;
	if(collection[actlevel] != 0) {	/* A variable has been taken from the nextlevel[actlevel]-th term. */
		level = nextlevel[actlevel];
		position = collection[actlevel] - 1;
		deletevar(T[level][position]);
		/* Delete the variable from the partial solution.        */
		position++;
		/* unlock variables if locked from this term             */
		for (; position < NV[level]; position++)
			if(lock[T[level][position]] == actlevel)
				lock[T[level][position]] = 0;
		collection[actlevel] = 0;	/* No variable from the term is  */
		/* now part of the solution.     */
	}
}

/***************************************************************************/
/* deletevar                                                               */
/***************************************************************************/
/* This procedure deletes all the terms in nextlevel above startlevel[var] */
/* and the variable var from the partial solution.                         */
/***************************************************************************/
static void deletevar(var)
posint var;
{
	novar--;		/* One variable less. */
	maxlevel = startlevel[var];	/* next free position */
	startlevel[var] = 0;	/* The variable var is not part of the  */
	/* partial solution anymore.            */
}

/***************************************************************************/
/* insertvar                                                               */
/***************************************************************************/
/* Insertvar adds the variable var to the partial solution and the terms   */
/* with the complement of the variable var to the list of terms            */
/* represented by nextlevel.                                               */
/***************************************************************************/
static void insertvar(var)
posint var;
{
	posint i;
	novar++;		/* one more variable. */
	startlevel[var] = maxlevel;
	/* maxlevel is the number of the term the variable has been taken from.  */
	/* The terms with the complement of the variable var  have to be looked  */
	/* at. This is done by adding them to the array nextlevel.               */
	for (i = start[var]; i < start[var + 1]; i++, maxlevel++)
		nextlevel[maxlevel] = i;
}

/***************************************************************************/
/* calculate_d                                                             */
/***************************************************************************/
/* Calculate_d computes the nfa defined by the solution and the essential  */
/* grids.                                                                  */
/***************************************************************************/
static nfa calculate_d(delta, i_gr, f_gr, fI, f)
ptset **delta;
ptset i_gr, f_gr;
gridlist fI;
nfa f;
{
	posint i, j, k, l, m;
	array old2new;		/* Array of number of non-essential grids */
	/* to number of grid in the solution.     */
	gridlist start;
	nfa result;
	result = newnfa();
	result->alphabet_size = f->alphabet_size;
	j = solution->ngrset;
	/* j is the number of states corresponding  */
	/* to a non-essential grid in the solution. */
	result->highest_state = j + NECESSARY - 1;
	result->delta = newndelta(result->alphabet_size, result->highest_state);
	result->infin = newfinal(result->highest_state);
	old2new = newarray(REST);
	i = 0;
	k = NECESSARY;
	while(j != 0) {
		if(testelem(i, solution->grset)) {
			old2new[i] = k++;
			j--;
		}
		i++;
	}
	result->is_eps = FALSE;
	result->minimal = TRUE;
	/* Calculation of initial and final states of */
	/* the necessary grids.                       */
	for (j = 0; j < NECESSARY; j++) {
		if(testelem(j, i_gr->grset))	/* Found an initial state. */
			setinit(result->infin[j]);
		if(testelem(j, f_gr->grset))	/* Found a final state. */
			setfinalT(result->infin[j]);
	}
	/* Calculation of initial and final states of */
	/* the necessary grids.                       */
	j = NECESSARY;
	k = solution->ngrset;
	while(k != 0) {
		if(testelem((j - NECESSARY), solution->grset)) {
			k--;
			if(testelem(j, i_gr->grset))	/* Found an initial state. */
				setinit(result->infin[old2new[(j - NECESSARY)]]);
			if(testelem(j, f_gr->grset))	/* Found a final state. */
				setfinalT(result->infin[old2new[(j - NECESSARY)]]);
		}
		j++;
	}
	/* Calculation of delta for necessary grids. */
	for (i = 0; i < NECESSARY; i++)
		for (j = 1; j <= result->alphabet_size; j++) {
			start = fI;
			k = 0;
			/* First try if essential grids are to be */
			/* joined to delta.                       */
			while(k < NECESSARY) {
				if(grid_is_covered(delta, start->info, (j - 1), i))
					connect(result->delta, j, i, k);
				start = start->next;	/* Next grid. */
				k++;
			}
			/* Now other grids. */
			l = solution->ngrset;
			while((k < GITTER) && (l != 0)) {
				if(testelem((k - NECESSARY), solution->grset)) {
					l--;
					if(grid_is_covered(delta, start->info, (j - 1), i))
						connect(result->delta, j, i, old2new[(k - NECESSARY)]);
				}
				start = start->next;	/* Next grid. */
				k++;
			}
		}
	/* Calculation of delta for other grids. */
	l = solution->ngrset;
	i = NECESSARY;
	while(l != 0) {
		if(testelem((i - NECESSARY), solution->grset)) {	/* Found a grid that is an element of the solution. */
			l--;
			for (j = 1; j <= result->alphabet_size; j++) {
				start = fI;
				k = 0;
				/* First try if essential grids are to be */
				/* joined to delta.                       */
				while(k < NECESSARY) {
					if(grid_is_covered(delta, start->info, (j - 1), i))
						connect(result->delta, j, old2new[(i - NECESSARY)], k);
					start = start->next;	/* Next grid. */
					k++;
				}
				/* Now other grids. */
				m = solution->ngrset;
				while((k < GITTER) && (m != 0)) {
					if(testelem((k - NECESSARY), solution->grset)) {
						m--;
						if(grid_is_covered(delta, start->info, (j - 1), i))
							connect(result->delta, j, old2new[(i - NECESSARY)],
								old2new[(k - NECESSARY)]);
					}
					start = start->next;	/* Next grid. */
					k++;
				}
			}
		}
		i++;
	}
	return result;
}

/***************************************************************************/
/* grid_is_covered                                                         */
/***************************************************************************/
/* Grid_is_covered returns TRUE iff the help's columns are a subset of     */
/* delta[letter][grid]->grset.                                             */
/***************************************************************************/
static boole grid_is_covered(delta, help, letter, gridno)
ptset **delta;
grid help;
posint letter, gridno;
{
	posint i, j = help->ncol;
	if(j > delta[letter][gridno]->ngrset)
		return FALSE;
	for (i = 0; j != 0; i++)
		if(testelem(i, help->col)) {
			j--;
			if(!testelem(i, delta[letter][gridno]->grset))
				return FALSE;
			/* i is a column in help but not an         */
			/* element of delta[letter][gridno] ->grset */
		}
	return TRUE;
}
