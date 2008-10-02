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

/* regexp-infix.c
 */

#include "regexp-infix.h"
#include "parser_types.h"


#define STAR	2
#define CONC	1
#define UNIO	0

static string estr;	/* infix expression */
static regex currex;	/* reg. expr. which is transformed */
static posint ini,outi; /* index for in- and output */
static boole doconcat;
static void inexpr();

/* find a factor
 * flag is the last op-symbol
 */
static void
infac(flag)
boolx flag;
/* find a factor
 * flag is the last op-symbol
 */
{ register char ch = currex->exprex[ini];
	if ((ch == starch) || (ch == plusch)) {
		while ((ch == starch) || (ch == plusch)) {
			estr[outi++] = currex->exprex[ini--];
			ch = currex->exprex[ini];
		}
		switch (ch) {
		case unionch:
		case concatch: inexpr(STAR); break;
		default: inexpr(flag);
		}
	} else  {
		while (isdigit(currex->exprex[ini])) {
			estr[outi++] = currex->exprex[ini--];
		}
		estr[outi++] = currex->exprex[ini--];
	}
} /* infac */

/******************************************************************/
static void
interm(flag)
boolx flag;
/* find a term
 * flag is the last op-symbol
 */
{ char op;	/* the operation */
  boolx newflag;
	op = currex->exprex[ini--];
	switch(op) {
	case unionch: newflag = UNIO; break;
	case concatch:newflag = CONC; break;
	}

	if (newflag < flag) {
		estr[outi++] = ')';
		inexpr(newflag);
		if (op == unionch) estr[outi++] = spacech;
		if ((op != concatch) || (doconcat)) estr[outi++] = op;
		if (op == unionch) estr[outi++] = spacech;
		inexpr(newflag);
		estr[outi++] = '(';
	} else {
		inexpr(newflag);
		if (op == unionch) estr[outi++] = spacech;
		if ((op != concatch) || (doconcat)) estr[outi++] = op;
		if (op == unionch) estr[outi++] = spacech;
		inexpr(newflag);
	}
} /* interm */

/******************************************************************/
static void
inexpr(flag)
boolx flag;
/* find an expression
 * flag is the last op-symbol
 */
{
	switch(currex->exprex[ini]){
	case unionch:
	case concatch: interm(flag); break;
	default: infac(flag); /* a star or a letter */
	}
} /* inexpr */

/******************************************************************/
/******************************************************************/
string
infix(re,dc,reslen)
regex re;  /* input is re->exprex */
boole dc;  /* TRUE iff concatch should be displayed */
posint *reslen;
/* produce a infix-expression of the postfix-expression re->exprex
 * RETURN: the infix-string and the length of this string in reslen
 */
{ posint count = 0;
  register string result;
	doconcat = dc;
	/* compute the max. number of brackets and spaces*/
	for(ini=0; ini < re->erexl;)
		switch(re->exprex[ini++]){
		case plusch:
		case starch: count++; break;
		case concatch: if (doconcat) count++; break;
		case unionch: count += 3; break; /* two spaces */
		default:;
		}
	estr = (string)newbuf(re->erexl + 2*count, sizeof(char));

	/* initialize the static vars and start the recursiv procedure */
	ini = re->erexl-1; outi = 0;
	currex = re;
	inexpr(UNIO);

	/* copy the infix-string to result */
	*reslen = outi;
	result = newrexstr(outi);
	result[outi] = '\0';
	for(ini = 0; outi--;) result[ini++] = estr[outi];

	freebuf();
	return result;
} /* infix */

