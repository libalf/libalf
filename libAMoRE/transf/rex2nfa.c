/*
 *  Copyright (c) ?    - 2000 Lehrstuhl fuer Informatik VII, RWTH Aachen
 *  Copyright (c) 2000 - 2002 Burak Emir
 *  Copyright (c) 2008, 2009 - David R. Piegdon <david-i2@piegdon.de>, Chair of Computer Science 2 and 7, RWTH-Aachen
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

/* rexnfa.c 3.21 (KIEL) 12.96 */
/* 04.Dez. 96:  bug fixed concerning empty string */

#include <amore/rex2nfa.h>

#include <amore/parser_types.h>

/* ****************************************************************/
/* functions included:
 *	static boole complast()
 *	static boole compfirst()
 *	static void getexpr()
 *	static void compff()
 *		nfa rex2nfa()
 */
/* ****************************************************************/

#define skipch		'!'
#define unskipch 	'?'

/** Modification by oma:  rex used to be just an abbreviation in older 
 *   versions. Now it is auxiliary storage.  rex[] (prerex[], resp.) 
 *  contains the postfix-notation ( prefix- , resp.) of the 
 *  expression, where every letter in replaced by the dummy 'a'.   
 */
static char* rex, *prerex;
/* postfix (ab.) and prefix (.ab) form of input string (a.b) */

/** state to which compfirst connects */
static posint constate;
/** length of expressions */
static posint length;
/** oma: lenght of rex */
static posint rexlen;

/** transformation table for letter -> state */
static posint *posch, *rposch;
/** transformation letter  ->  original letter */
static posint *poslt, *rposlt;
/* .. introduced by oma, Jan.96  */
/* if the postfix notation rex[] (prefix notation prerex[], resp.) of the regular
   expression holds the char 'a' at position i, then poslt[i] (rposlt[i], resp.)
   gives the actual number of the letter and posch[i] (rposch[i], resp.) gives the
   corresponding state.

   In previous versions of this file, the letter was stored directly in rex[i] 
   ( prerex[i], resp.), e.g. rex[i]='c' instead of rex[i]='a' and poslt[i]=3.

   Note that all the functions in this file work appropriately even if the 
   notations a1,a2,a3 etc and a,b,c etc are mixed. */
static posint *skipval;
static posint hlp;		/* store values, counter */
static posint alphabet_size;		/* size of alphabet */

/* variables which run through rex and prerex */
static posint lastrex, lastprerex;
static posint firstprerex;
static posint getrex, getprerex;
static posint ffrex, ffprerex;


/** computes { a | A*a intersection L(rex) != emptyset } in buf
 * RETURN: TRUE iff the emptyword is in L(act_rex)
 *         static variable prerex: prefix-form of rex
 *	   static var rposch: posch for prerex
 * recursiv 
 * scans rex form left to right (lastrex), constructs prerex (lastprerex) and
 * rposch and rposlt and marks last states in buf "final"
 *
 *		last(e1 union e2) = last(e1) union last(e2)
 *		last(e1 concat e2) = if (eps in e2) last(e1) union last(e2)
 *				     else last(e2)
 *		last(e star) = last(e)
 */

static boole complast(mrkfin buf)
{
	char op;
	boole epslbl1, epslbl2;	/* return values for eps-property */

	op = rex[lastrex--];
	switch (op) {
	case unionch:
		epslbl2 = complast(buf);
		epslbl1 = complast(buf);
		prerex[--lastprerex] = op;
		return (epslbl1 || epslbl2);
	case plusch:
		epslbl1 = complast(buf);
		prerex[--lastprerex] = op;
		return (epslbl1);
	case starch:
		(void) complast(buf);
	case epsch:
		prerex[--lastprerex] = op;
		return (TRUE);
	case 'A':
	case 'a':
		if(buf != NULL) {
			setfinalT(buf[posch[lastrex + 1]]);
		}
		/* copy the posch- and poslt-values for the symbol */
		rposlt[lastprerex - 1] = poslt[lastrex + 1];
		rposch[lastprerex - 1] = posch[lastrex + 1];
	case langch:
		prerex[--lastprerex] = op;
		return (FALSE);
	case concatch:		/* scan e2 first and if eps not in e2 skip over e1 */
		if(complast(buf))
			epslbl1 = complast(buf);
		else {
			(void) complast((mrkfin) NULL);
			epslbl1 = FALSE;
		}
		prerex[--lastprerex] = op;
		return (epslbl1);
	default:
		errex(SWERR, "complast");
	}
	/* NOTREACHED */
}				/* complast */

/* ****************************************************************/

/** computes { a | aA* intersection L(rex) != emptyset } in buf
 * RETURN: TRUE iff the emptyword is in L(act_rex)
 * recursiv 
 * scans prerex from left to right (firstprerex) and connect the first states
 * to constate in buf
 *
 *		first(e1 union e2) = first(e1) union first(e2)
 *		first(e1 concat e2) = if (eps in e1) first(e1) union first(e2)
 *				      else first(e1)
 *		first(e star) = first(e)
 */

static boole compfirst(ndelta buf)
{
	char op;
	boole epslbl1, epslbl2;	/* return values for eps-property */
	switch (op = prerex[firstprerex++]) {
	case skipch:
		firstprerex = skipval[firstprerex - 1];
	case unskipch:
		return (compfirst(buf));
	case unionch:
		epslbl2 = compfirst(buf);
		epslbl1 = compfirst(buf);
		return (epslbl1 || epslbl2);
	case plusch:
		epslbl1 = compfirst(buf);
		return (epslbl1);
	case starch:
		(void) compfirst(buf);
	case epsch:
		return (TRUE);
	case 'A':
		if(buf != NULL)
			for (hlp = 1; hlp <= alphabet_size; hlp++)
				connect(buf, hlp, constate, rposch[firstprerex - 1]);
		return (FALSE);
	case 'a':
		if(buf != NULL)	/* modified by oma, Jan.96 */
			connect(buf, rposlt[firstprerex - 1], constate, rposch[firstprerex - 1]);
		return (FALSE);
	case langch:
		return (FALSE);
	case concatch:		/* skip first(e2) iff eps not in e1 */
		if(compfirst(buf))
			return (compfirst(buf));
		else {
			(void) compfirst((ndelta) NULL);
			return (FALSE);
		};
	}
	/* NOTREACHED */
	return (FALSE);
}				/* compfirst */

/** gets an expression from rex (getrex) and
 * iff copy == TRUE : copies it to prerex (getprerex)
 * else             : count the number of symbols
 */

static void getexpr(copy)
{
	char op;		/* act. symbol */
	switch (op = rex[getrex--]) {
	case unionch:
	case concatch:
		getexpr(copy);
	case plusch:
	case starch:
		getexpr(copy);
		if(copy)
			prerex[--getprerex] = op;
		else
			length += 2;
		break;
	case 'A':
	case 'a':		/* oma */
		if(copy) {
			prerex[--getprerex] = op;
			rposlt[getprerex] = poslt[getrex + 1];
			rposch[getprerex] = posch[getrex + 1];
		} else
			length++;
		break;
	case epsch:
	case langch:
		if(copy)
			prerex[--getprerex] = op;
		else
			length++;
		break;
	default:
		errex(SWERR, "getexpr");
	}
}				/* getexpr */

/* ****************************************************************/

/** compute the follow sets
 * scans rex from left to right (ffrex), and copies some information
 * to prerex (for compfirst) with ffprerex
 *		define X by
 *		X(langch,f) = X(epsch,f) = emptyset
 *		X(letter,f) = {letter}xfirst(f)
 *		X(e1 union e2,f) = X(e1,f) union X(e2,f)
 *		X(e1 con e2,f) = X(e1,e2 con f) union X(e2,f)
 *		X(e star,f) = X(e, e union f)
 *
 *		then
 *		X(e,f) = follow(e) union last(e)xfirst(f)
 *
 *	we have: X == compff
 *               e == rex
 *               f == prerex+ffprerex
 */

static void compff(posint f, ndelta buf)
{
	char ch;
	posint oldffprerex,	/* to store ffprerex */
	 skippos,		/* the last skip position */
	 conletter;

	ch = rex[ffrex--];
	while(isdigit(ch)) {
		prerex[--ffprerex] = ch;

		ch = rex[ffrex--];
	}
	switch (ch) {
	case unionch:		/* call compff two times starting at f */
		prerex[--ffprerex] = skipch;
		skippos = ffprerex;
		skipval[skippos] = f;
		compff(f, buf);
		compff(f, buf);
		prerex[--ffprerex] = unionch;
		prerex[skippos] = unskipch;
		break;
	case concatch:		/* call compff once from f and then from ffprerex */
		prerex[--ffprerex] = skipch;
		skippos = ffprerex;
		skipval[skippos] = f;
		compff(f, buf);
		prerex[--ffprerex] = concatch;	/* e2 concat f */
		oldffprerex = ffprerex;
		compff(ffprerex, buf);
		prerex[--ffprerex] = ch;
		prerex[skippos] = unskipch;
		prerex[oldffprerex] = unskipch;
		break;
	case starch:
	case plusch:
		/* skip multiple stars and pluses (but a star overwrites
		 * a plus)
		 */
		while((rex[ffrex] == starch) || (rex[ffrex] == plusch)) {
			if(rex[ffrex--] == starch)
				ch = starch;

		}
		prerex[--ffprerex] = skipch;
		skippos = ffprerex;
		skipval[skippos] = f;
		/* get the expression under the star */
		getrex = ffrex;
		getprerex = ffprerex;
		getexpr(TRUE);
		ffprerex = getprerex;
		prerex[--ffprerex] = unionch;	/* e union f */
		/* notice the union and the following stuff must
		 * be deleted after X(e, e union f) has been
		 * computed (the union-sign must become a star or plus)
		 */
		oldffprerex = ffprerex;
		compff(ffprerex, buf);
		ffprerex = oldffprerex;
		prerex[ffprerex] = ch;
		prerex[skippos] = unskipch;
		break;
	case 'A':
	case 'a':
		constate = posch[ffrex + 1];
		conletter = poslt[ffrex + 1];	/* modifie by oma, Jan.96 */
		firstprerex = f;
		(void) compfirst(buf);
		prerex[--ffprerex] = ch;
		rposlt[ffprerex] = conletter;
		rposch[ffprerex] = constate;
		break;
	case epsch:
	case langch:
		prerex[--ffprerex] = ch;
		break;
	default:
		errex(SWERR, "compff");
	}
}				/* compff */

/** computes a nondeterministic automaton from a given
 * regular expression (must not be generalize!, i.e. no
 * complement, intersection, ...) 
 * a "fast" algorithm is used:
 * 	G.Berry, R.Sethi
 *	From Regular Expressions to Deterministic Automata
 *	TCS 48(1986), pp. 117-126
 *      desciption:
 * RETURN: the NFA
 * 
 * always: state 0 is only final state !!! used in genrexnf.c !!!
 */

nfa rex2nfa(regex re)
{
      /** number of states in the NFA 
       *  (one more than #letters in the reg. expr.) */
	posint stateno;

      /** length of prerex = length of rex + # stars in rex + # plus in rex
       *                                   + # concat in rex */
	posint prerexlen;
	nfa result;
	register posint i;

	char ch;		/* oma */
	posint num = 0;

	posint concats = 0;	/* number of concats in re */

	/* init static vars */
	/* Aenderungen von oma   :  11.01.96  */
	rex = newarray_of_char(re->erexl);
	/* war :  rex = re->exprex;  */

	posch = newarray_of_int(re->erexl);
	poslt = newarray_of_int(re->erexl);	/* oma */
	alphabet_size = re->alphabet_size;

	/* known values of the result */
	result = newnfa();
	result->alphabet_size = re->alphabet_size;
	result->is_eps = FALSE;
	result->minimal = FALSE;

	/* compute the number of states */
	/* oma : and copy postfix-notation re->exprex to rex, replacing 
	   each letter with 'a' */
	stateno = 0;
	length = 0;
	rexlen = 0;
	for (i = 0; i < re->erexl; i++) {

		switch (ch = re->exprex[i]) {
		case 'A':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
			posch[rexlen] = ++stateno;
			poslt[rexlen] = ch - 'a' + 1;	/* oma */
			/* if a number follows, then this value poslt[rexlen] will 
			   have to be overwritten */
			rex[rexlen++] = 'a';	/* oma, 11.01.96 */
			num = 0;	/* oma: convert decimal number representation 
					   into num if a number follows */
			break;
		case plusch:
		case starch:
			rex[rexlen++] = ch;	/* oma */
			getrex = rexlen - 1;
			getexpr(FALSE);
			while((re->exprex[i + 1] == plusch) || (re->exprex[i + 1] == starch)) {
				i++;
			}
			length++;
			break;
		case epsch:	/* oma, 04.12.96  */
		case unionch:
			rex[rexlen++] = ch;	/* oma, 11.01.96 */
			length++;
			break;
		case concatch:
			rex[rexlen++] = ch;	/* oma, 11.01.96 */
			length++;
			concats++;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			num = num * 10 + (ch - '0');
			/* oma: update number */
			poslt[rexlen - 1] = num;
			/* oma: overwrite the value */
			break;
		default:
			/* never reached, I believe.  oma */
			length++;
		}		/* switch */
	}			/* for */

	/* now fill in the missing values */
	result->highest_state = stateno;
	result->infin = newfinal(result->highest_state);
	result->delta = newndelta(result->alphabet_size, result->highest_state);

	/* a special initial state (unique) */
	setinit(result->infin[0]);

	/* first step: the final states and prerex
	 * prepare prerex (length gt length of rex: we need
	 * some place to copy star-expressions more than once)
	 * define rposch and skipval
	 * complast returns TRUE: 
	 *  the empty word belongs to the language 
	 * the only initial state must marked final
	 * complast marks the last symbols final in 
	 * result->infin
	 */
	prerexlen = rexlen + length + concats;	/* oma */
	/* the prefixnotation will temprarily become  longer than the postfix
	   notation because intermediate symbols are introduced (and deleted or
	   skipped afterwards). */
	prerex = newarray_of_char(prerexlen + 1);
	prerex[prerexlen] = '\0';
	/*  deleted by oma : prerex[re->rexl] = '\0';  */
	rposch = newarray_of_int(prerexlen + 1);
	rposlt = newarray_of_int(prerexlen + 1);	/* oma */
	skipval = newarray_of_int(prerexlen + 1);
	lastprerex = rexlen;	/* oma, 11.01.96 */
	lastrex = lastprerex - 1;
	setfinal(result->infin[0], complast(result->infin));

	/* second step: the start state
	 * compfirst connects all first symbols with constate
	 * an puts the connections in result->delta
	 */
	firstprerex = 0;
	constate = 0;
	(void) compfirst(result->delta);

	/* third step: the connections
	 * call compff with an epsilon string in prerex
	 */
	ffprerex = prerexlen;
	ffrex = rexlen - 1;
	prerex[ffprerex] = epsch;
	compff(ffprerex, result->delta);

	freebuf();
	return (result);
}				/* rexnfa */
