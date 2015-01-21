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

/* libtest.c
 */

#include <stdio.h>

/* #include "obj.h" */
#include <amore/rexFromString.h>

#include <amore/buffer.h>

#include <string.h>
#include <amore/nfa2dfa.h>
#include <amore/rex2nfa.h>
#include <amore/genrex2nfa.h>
#include <amore/dfamdfa.h>
#include <amore/dfa2mon.h>


#include <amore/debugPrint.h>

//char *rexstr = "aba*ab";
char *rexstr = "(aba*)-(abaaa)";
// test suite 1


extern PARSE_RESULT errorParseResult;

regex r1;

int constr_rexFromString()
{
	printf("\nconstructing RegExp r:");
	r1 = rexFromString(2, rexstr);
	printf("result: %s\n", r1->exprex);
	return 1;
}

nfa n1;

int transf_rex2nfa(regex r)
{
	printf("\n  transforming Rex to Nfa");
	n1 = rex2nfa(r);
	printf("\n  result:");
	debugPrintNfa(n1);
	return 1;
}

int transf_genrex2nfa(regex r)
{
	printf("\n  transforming GenRex to Nfa");
	n1 = genrex2nfa(r);
	printf("\n  result:");
	debugPrintNfa(n1);
	return 1;
}

dfa d1;

int transf_nfa2dfa()
{
	printf("\n  transforming to Dfa");
	d1 = nfa2dfa(n1);
	printf("\n  result:");
	debugPrintDfa(d1);
	return 1;
}

void transf_dfa2mindfa()
{
	printf("\n   minimizing dfa");
	d1 = dfamdfa(d1, TRUE);
	printf("\n  result:");
	debugPrintDfa(d1);
}

monoid m;

void transf_dfa2mon()
{
	printf("\n  transforming dfa to monoid:");

	m = dfa2mon(d1);
	printf("\n  done.\n");
}

// cloning

int clone_dfa(dfa d)
{
	dfa p;
	printf("Testing clonedfa...\n");
	p = clonedfa(d);
	printf("result:\n");
	debugPrintDfa(p);
	return 1;
}

int clone_nfa(nfa n)
{
	nfa p;
	printf("Testing clonenfa...\n");
	p = clonenfa(n1);
	printf("result:\n");
	debugPrintNfa(p);
	return 1;
}

void regexp_errorMess()
{
	regex a, b, c;

	a = rexFromString(2, "abcdefg");	// only a, b allowed
	b = rexFromString(2, "aUbUc[;'{");	// illegal char
	c = rexFromString(2, "Uab");	//illegal syntax
}

int main(int argc, char **argv)
{

	initbuf();
	printf("libtest\n");

	constr_rexFromString();
	transf_genrex2nfa(r1);
	transf_nfa2dfa();
	transf_dfa2mindfa();
	transf_dfa2mon();

	/*
	   clone_nfa( n1 );
	   clone_dfa( d1 );

	   regexp_errorMess();
	 */
	return 0;
	// todo: freeing
}
