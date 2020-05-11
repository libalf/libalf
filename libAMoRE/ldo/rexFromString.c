/*
 *  Copyright (c) ?    - 2000 Lehrstuhl fuer Informatik VII, RWTH Aachen
 *  Copyright (c) 2000 - 2002 Burak Emir
 *  Copyright (c) 2008, 2009 - David R. Piegdon <david-i2@piegdon.de>, Chair of Computer Science 2, RWTH-Aachen
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

/* rexFromString.c
 */

#include <amore/rexFromString.h>
//#include <error.h>

#include <amore/parse.h>

#include <amore/global.h>	/* pi2s */

#define MAXCHAR 255

static char op_list[] = "( ) ~ + * U . & - ? $";
/* endch ($) must be included here !! */
/* identch(?) must be included here !! */


/** rules of our regular expression grammar */
static char rule1[] = "E->?|(E)|E&E|E.E|E-E|EUE|E*|E+|~E";

/* The precedence table */
static posint prec[11][11] = {
	/*         U        -        &        .        ~        +        *    */
	/*         (        )        ?        $                               */
	{GREATER, LESS, LESS, LESS, LESS, LESS, LESS,	/* U */
	 LESS, GREATER, LESS, GREATER},

	{GREATER, GREATER, LESS, LESS, LESS, LESS, LESS,	/* - */
	 LESS, GREATER, LESS, GREATER},

	{GREATER, GREATER, GREATER, LESS, LESS, LESS, LESS,	/* & */
	 LESS, GREATER, LESS, GREATER},

	{GREATER, GREATER, GREATER, GREATER, LESS, LESS, LESS,	/* . */
	 LESS, GREATER, LESS, GREATER},

	{GREATER, GREATER, GREATER, GREATER, LESS, LESS, LESS,	/* ~ */
	 LESS, GREATER, LESS, GREATER},

	{GREATER, GREATER, GREATER, GREATER, GREATER, GREATER, GREATER,	/* + */
	 UNUSED, GREATER, UNUSED, GREATER},

	{GREATER, GREATER, GREATER, GREATER, GREATER, GREATER, GREATER,	/* * */
	 UNUSED, GREATER, UNUSED, GREATER},

	{LESS, LESS, LESS, LESS, LESS, LESS, LESS,	/* ( */
	 LESS, EQUAL, LESS, UNBALBR},

	{GREATER, GREATER, GREATER, GREATER, UNUSED, GREATER, GREATER,	/* ) */
	 UNUSED, GREATER, UNUSED, GREATER},

	{GREATER, GREATER, GREATER, GREATER, GREATER, GREATER, GREATER,	/* ? */
	 UNUSED, GREATER, UNUSED, GREATER},

	{LESS, LESS, LESS, LESS, LESS, LESS, LESS,	/* $ */
	 LESS, UNBARBR, LESS, ENDTEXT},

};

/* Abbreviations in postfixnotation -- unused */
EXPRESS *AbbsInPostfix;


/** Initialize the mapping {operators}->|{operators}| 
 * The mapping is only used for the precedence-table 
 * If this mapping is changed, the precedence table must be changed too 
 */

static void init_index(array_of_int index)
{
	index[unionch] = 0;
	index[minusch] = 1;
	index[insecch] = 2;
	index[concatch] = 3;
	index[complch] = 4;
	index[plusch] = 5;
	index[starch] = 6;
	index[leftbrch] = 7;
	index[rightbrch] = 8;
	index[identch] = 9;
	index[endch] = 10;
	return;
}

/** populates identifier line.
 *  returning "a0 a1 ... a10 ... a100 ..."
 */
static char* init_idline_alphanum(int maxlet)
{
	char* IdLine;		/*< identifier line */
	posint LetLen, i;
	char* LetNum;		/*< letter-number   */
	posint IdLineSize;
			 /** size of the identifier line */
	posint IdListP;		/*< position in IdLine */

	IdLineSize = 9 * 3 + 3 + (MAXLET - 'A' + 1) * 2;

	LetLen = 4;		/* length of "aYY" (YY <= 99) + one spacech character  */
	i = 10;
	do {
		IdLineSize += ((i * 10) < maxlet) ?	/* last time? */
		    /* no  */ (9 * i * LetLen)
		    /* 1 ... 9 */
		    /* yes */
		    : ((maxlet - i + 1) * LetLen);	/* 1 ... x <= 9 */
		++LetLen;
		i *= 10;
	} while(i < maxlet);

	/* allocate IdLine in buffer */
	IdLine = newbuf_string(IdLineSize + 1);
	/* reset position */
	IdListP = 0;

	for (i = 1; i <= maxlet; ++i) {
		IdLine[IdListP++] = 'a';
		LetNum = pi2s(i);
		strcpy(&IdLine[IdListP], LetNum);
		IdListP += strlen(LetNum);
		IdLine[IdListP++] = spacech;
	}
	IdLine[IdListP++] = epsch;
	IdLine[IdListP++] = spacech;
	IdLine[IdListP++] = langch;

	return IdLine;
}

/** populates identifier line.
 *  letters are simply @,a,b,c,d ... 
 */
static char* init_idline_alphabet(int maxlet)
{
	char* IdLine;		/*< identifier line */
	posint IdLineSize;
			 /** size of the identifier line */
	posint IdListP;		/*< position in IdLine */
	posint i;

	IdLineSize = maxlet * 2 + 3 + (MAXLET - 'A' + 1) * 2;
	/* allocate IdLine in buffer */
	IdLine = newbuf_string(IdLineSize + 1);
	/* reset position */
	IdListP = 0;

	/* letters are simply "a b c d ..." */
	for (i = 1; i <= maxlet; ++i) {
		IdLine[IdListP++] = itoc[i];
		IdLine[IdListP++] = spacech;
	}
	IdLine[IdListP++] = epsch;
	IdLine[IdListP++] = spacech;
	IdLine[IdListP++] = langch;

	return IdLine;
}

/** returns a PARSE_INPUT object that contains grammar for AMoRE regular 
 *  expressions if maxlet > ALPHSIZE 
 */
static PARSE_INPUT init_parseinp(int maxlet, char *s)
{
	posint i, j;
	char* IdLine;		/*< identifier line */
	PARSE_INPUT parse_parms = newbuf_parse_input();
	/* expanded regular expression ? */
	/* EXPRESS RexInPostfix; */
	array_of_int_array precedence;
	strstack str = newbuf_strstack();

	/* disabled - be */
	/*
	   AbbsInPostfix = (EXPRESS *) newbuf(NABBR, sizeof(EXPRESS));
	 */

	/* initialize the variables */
	/* initre(re); */

	/* populate IdLine with identifiers */
	IdLine = (maxlet > ALPHSIZE) ? init_idline_alphanum(maxlet)	/* "a0 a1 ..." */
	    : init_idline_alphabet(maxlet);	/* "a b c ..." */

	/* copying ... (unnecessary ?) */
	parse_parms->op_list = newbuf_string(strlen(op_list) + 1);
	strcpy(parse_parms->op_list, op_list);

	parse_parms->id_list = IdLine;

	parse_parms->rules = newbuf_strar(1);
	parse_parms->rules[0] = newbuf_string(strlen(rule1) + 1);
	strcpy(parse_parms->rules[0], rule1);

	parse_parms->maxrule = 9;	/* Nine rules in the grammar (count 'em) */
	parse_parms->startsymbol = 'E';


	precedence = (array_of_int_array) newbuf(11, sizeof(array_of_int));

	for (i = 0; i < 11; ++i) {
		precedence[i] = (array_of_int) newbuf(11, sizeof(posint));
		for (j = 0; j < 11; ++j)
			precedence[i][j] = prec[i][j];
	}
	parse_parms->precedence = precedence;

	parse_parms->index = newbuf_array(MAXCHAR);

	init_index(parse_parms->index);

	/* strstack rubbish */
	str->info = strdup(s);
	str->lastpos = strlen(str->info) - 1;

	parse_parms->buf = str;

	return parse_parms;
}				/* end MYinit */

/******************************************************************/
static EXPRESS tree2string(t_elem Root)
{
	char* OutString;
	t_elem Node = Root;
	posint Count = 0, Length = 0;
	char FirstLet;
	EXPRESS RetVal = (EXPRESS) newbuf(1, sizeof(struct str_express));

	while(Node->sons_no != 0)
		Node = Node->son[0];

	while(Node != NULL) {
		FirstLet = Node->expr[0];
		switch (Node->sons_no) {
		case 0:
			if((FirstLet >= 'A') && (FirstLet <= MAXLET))
				Length += AbbsInPostfix[FirstLet - 'A']->length;
			else
				Length += strlen(Node->expr);
			Node = Node->father;
			break;

		case 1:
			switch (FirstLet) {
			case leftbrch:
				break;
			default:
				++Length;
				break;
			}
			Node = Node->father;
			break;
		case 2:
			switch (Node->son_passed) {
			case 0:
				Node->son_passed = 1;
				Node = Node->son[1];
				while(Node->sons_no != 0)
					Node = Node->son[0];
				break;
			case 1:
				Node->son_passed = 0;
				++Length;
				Node = Node->father;
				break;
			default:;
			}
			break;
		default:;
		}
	}
	Node = Root;
	OutString = newbuf_string(Length + 1);

	RetVal->length = Length;

	while(Node->sons_no != 0)
		Node = Node->son[0];

	while(Node != NULL) {
		FirstLet = Node->expr[0];
		switch (Node->sons_no) {
		case 0:
			if((FirstLet >= 'A') && (FirstLet <= MAXLET)) {
				strcpy(&OutString[Count], AbbsInPostfix[FirstLet - 'A']->expr);
				Count += AbbsInPostfix[FirstLet - 'A']->length;
			} else {
				strcpy(&OutString[Count], Node->expr);
				Count += strlen(Node->expr);
			}
			Node = Node->father;
			break;

		case 1:
			switch (FirstLet) {
			case leftbrch:
				break;
			default:
				OutString[Count++] = FirstLet;
				break;
			}
			Node = Node->father;
			break;
		case 2:
			switch (Node->son_passed) {
			case 0:
				Node->son_passed = 1;
				Node = Node->son[1];
				while(Node->sons_no != 0)
					Node = Node->son[0];
				break;
			case 1:
				Node->son_passed = 0;
				OutString[Count++] = FirstLet;
				Node = Node->father;
				break;
			default:;
			}
			break;
		default:;
		}
	}
	RetVal->expr = OutString;

	return RetVal;
}

regex rexFromString(int alphabet_size, char *str)
{
	PARSE_INPUT pinp = newbuf_parse_input();
	PARSE_RESULT pr;
	EXPRESS e;
	regex re;
	boole isreg;
	posint i;


	pinp = init_parseinp(alphabet_size, str);

	pr = parser(pinp);

	if(pr->tree == NULL) {	// error handling
#ifndef JAVAEXCEPT
		liberror("rexFromString, parser ", pr->error_message);
		exit(-1);	// otherwise segmentation fault!
#else
		//fprintf(stderr,"\nparse error, throwing RegExpParseException\n");
		errorParseResult = pr;
		return (regex) 0;
#endif
	}

	e = tree2string(pr->tree);


	re = newrex();
	re->alphabet_size = alphabet_size;
	re->rexl = strlen(str);
	re->rex = strdup(str);

	re->erexl = e->length;
	re->exprex = strdup(e->expr);

	re->useda = -1;
	/* for abbrevs
	   re->useda = maxdef - 'A';
	   for(i=0; i <= re->useda; i++) {
	   strl = ablist[i]->length;
	   re->abbr[i] = newrexstr(strl);
	   re->abbl[i] = strl;
	   re->abbr[i][strl] ='\0';
	   strl = 0;
	   pt = ablist[i]->info;
	   while (pt != NULL) {
	   for(j=0; j <= pt->lastpos;)
	   re->abbr[i][strl++] = pt->info[j++];
	   pt = pt->next;
	   }
	   }
	 */

	/* test whether expression is regular (without ~,-,&) */
	isreg = TRUE;
	for (i = 0; (i < re->erexl) && isreg; ++i)
		if((re->exprex[i] == minusch) || (re->exprex[i] == complch) || (re->exprex[i] == insecch))
			isreg = FALSE;
	re->grex = (!isreg);

	free(pinp->buf->info);
	freebuf();
	return re;
}
