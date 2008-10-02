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

#include "fileIO.h"

static FILE *amrp;

/** header line           V0             V1.0      */
static char *versiontext[] = {  "\n",  " VERSION 1.0\n",};

#ifdef DEBUG
#include <stdio.h>
#endif

/** header lines of parts */
static char *text[] = {
/*  0 */"REG.EXP.: #ABB., #SYM0, #SYM1, #ALPHABET\n",
	"REG.EXP.: LENGTH OF REX, EXPANDED REX\n",
	"REG.EXP.: REX KNOWN, GENERALIZED REX\n",
	"REG.EXP.: LENGTH OF ABBREVIATIONS\n",
	"REG.EXP.: ABBREVIATIONS\n",
	"REG.EXP.: REX, EXPANDED REX\n",
/*  6 */"DFA: #STATES\n",
	"DFA: ",
	"DFA: INITIAL STATE\n",
	"DFA: ALPHABET\n",
	"DFA: MINIMAL\n",
	"DFA: FINAL STATES\n",
	"DFA: TRANSFORMATION\n",
	"DFA: END\n",
/* 14 */"NFA: #STATES\n",
	"NFA: ",
	"NFA: INITIAL STATE\n",
	"NFA: ALPHABET\n",
	"NFA: EPSILON, MINIMAL\n",
	"NFA: FINAL STATES\n",
	"NFA: TRANSFORMATION\n",
	"NFA: END\n",
/* 22 */"D-CLASS: #DCLASSES\n",
	"D-CLASS: #RCLASSES\n",
	"D-CLASS: #LCLASSES\n",
	"D-CLASS: MINRANG\n",
	"D-CLASS: THE CLASSES\n",
	"D-CLASS-CLASSES: ORG,RNO,LNO,HSIZE,RANG AND REGULAR\n",
	"D-CLASS THE CLASSES: L-REP\n",
	"D-CLASS THE CLASSES: R-REP\n",
	"D-CLASS THE CLASSES: H-CLASS\n",
/* 31 */"MONOID: #STATES\n",
	"MONOID:",
	"MONOID: #LETTERS\n",
	"MONOID: #ELEMENTS\n",
	"MONOID: #GENERATORS\n",
	"MONOID: LIST OF GENERATORS\n",
	"MONOID: LETTER TO GENERATOR WITH SAME TRANSFORMATION\n",
	"MONOID: GENERATOR SUCCESSORS OF ELEMENTS\n",
	"MONOID: ZERO of THE MONOID\n",
	"MONOID:",
	"MONOID:",
	"MONOID: BOOLEAN VALUES (DCLASSISCOMPUTED, MEQUALS)\n",
	"MONOID: END\n",
	"MONOID: RANG\n",
	"MONOID: #RELATIONS, (RIGHT,LEFT)-SIDE\n",
/* 46 */"STARFREE EXPRESSION: LENGTH OF U\n",
	"STARFREE EXPRESSION: LENGTH OF V\n",
	"STARFREE EXPRESSION: LENGTH OF W0\n",
	"STARFREE EXPRESSION: LENGTH OF W1\n",
	"STARFREE EXPRESSION: U\n",
	"STARFREE EXPRESSION: V\n",
	"STARFREE EXPRESSION: W0\n",
	"STARFREE EXPRESSION: W1\n",
	"STARFREE EXPRESSION: COMPUTED\n",
	"STARFREE EXPRESSION: END\n",
/* 56 */"AMORE Language Save File",
	"NAME \n",
	"ALPHABET \n",
	"INPUT\n",
	"BOOLE FLAGS: (BREX,BDFA,BNFA,BENFA,BMON) \n",
	"REGULAR EXPRESSION \n",
	"DETERMINISTIC FINITE AUTOMATON \n",
	"NONDETERMINISTIC FINITE AUTOMATON \n",
	"MONOID \n",
	"STARFREE EXPRESSION \n",
	"EMPTY,FULL,FOLU,SF,LOC.TESTABLE,DEFINITE,REV.D.,GEN.D.,DOTDEPTH,NILPOTENT\n",
};


/** index to file header statements 
 *  starting points for the array text 
 *  old values reestablished by oma */

#define REGNO	0
#define DFANO	6
#define NFANO	14
#define DCLNO	22
#define MONNO	31
#define SFENO   46
#define GLBNO	56

/* CUR_VERSION must be the index of the array versiontext where the
   headline of the current version of amore can be found */
#define CUR_VERSION 1
#define LEN_OF_HEADER 24

/** PATHLEN: max. chars in (full) pathname */
#define PATHLEN	255

/** write to file */
#define fwrite(A) (void)fputs(A,amrp)
/** read line from file to line[ ] */
#define frdin()	  (void)fgets(line,LINELEN1,amrp)
/** write newline to file */
#define new_line	(void)fputs("\n",amrp)


/* functions included:
 *
 *	static	char*	i2a()
 *	      	XXX   char*	pi2a() MOVED TO global.c
 *	static	char*	be2a()
 *	static	char*	bx2a()
 *
 *	static	void	writestr()
 *	static	void	writerex()
 *	static	void	writeform()
 *	static	void	writedfa()
 *	static	void	writenfa()
 *	static	void	writedcl()
 *	static	void	writemon()
 *	static	void	writesf()
 *	      	boolx	writelan()
 */

/* ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ */
/*                   WRITING DATA                                          */
/* ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ */


static char* i2a(int i) {
  (void)sprintf(dummy,IFMT,i);
  return dummy;
} /* i2a */

/* MOVED TO global.c
char* pi2a(posint pi) {
  (void)sprintf(dummy,PIF,pi);
  return dummy;
}  pi2a */

static char* be2a(boole bval) {	
  return (bval)? "T\n" : "F\n";
} /* be2a */

static char* bx2a(boolx bxval) {
  switch (bxval) {
	case TRUE:    return "T\n";
	case FALSE:   return "F\n";
	case UN_KNOWN: return "U\n";
	default     : errex(SWERR,"file.c: bx2a");
	} /* end switch */
	/* NOTREACHED */
} /* bx2a */

static void writestr(register string str,register posint strl) { 
  char ch;
		while (strl > LINELEN) {
			strl -= LINELEN;
			ch = str[LINELEN]; str[LINELEN] = '\0';
			fwrite(str); new_line;
			str[LINELEN] = ch;
			str += LINELEN;
		}
		if (strl) {fwrite(str); new_line; }
		new_line;
} /* writestr */

/* ---------------------------------------------------- write REGEXP */
static void writerex(regex crex) { 
  int i;
	fwrite(text[REGNO]);
	fwrite(pi2a((posint)(crex->useda + 1))); new_line;
	fwrite(pi2a(crex->sno)); new_line;
	fwrite(text[REGNO+1]);
	fwrite(pi2a(crex->rexl)); new_line;
	fwrite(pi2a(crex->erexl)); new_line;
	fwrite(text[REGNO+2]);
	fwrite(be2a(TRUE));
	fwrite(be2a(crex->grex));
	fwrite(text[REGNO+3]);
	for(i=0; i <= crex->useda; i++) {
		fwrite(pi2a(crex->abbl[i])); new_line;
	}
	fwrite(text[REGNO+4]);
	for(i=0; i <= crex->useda; i++) {
		writestr(crex->abbr[i],crex->abbl[i]); new_line;
	}
	fwrite(text[REGNO+5]);
	writestr(crex->rex,crex->rexl);
	new_line;
	writestr(crex->exprex,crex->erexl);
} /* writerex */


/* ---------------------------------------------------- write DFA */
static void
writedfa(cdfa)
dfa cdfa;
{ register posint i,j;
	fwrite(text[DFANO]);
	fwrite(pi2a(cdfa->qno)); new_line;
	fwrite(text[DFANO+2]);
	fwrite(pi2a(cdfa->init)); new_line;
	fwrite(text[DFANO+3]);
	fwrite(pi2a(cdfa->sno)); new_line;
	fwrite(text[DFANO+4]);
	fwrite(be2a(cdfa->minimal));
	fwrite(text[DFANO+5]);
	for(i=0; i<=cdfa->qno; )
		fwrite(bx2a(cdfa->final[i++]));
	fwrite(text[DFANO+6]);
	for(i=1; i<=cdfa->sno; i++)
		for(j=0; j<=cdfa->qno; ) {
			fwrite(pi2a(cdfa->delta[i][j++]));
			new_line;
		}
	fwrite(text[DFANO+7]);
} /* writedfa */


/* ---------------------------------------------------- write NFA */
static void
writenfa(cnfa)
nfa cnfa;
{ register posint i,j,k;
	fwrite(text[NFANO]);
	fwrite(pi2a(cnfa->qno)); new_line;

/*	fwrite(text[NFANO+2]);
	fwrite(pi2a(0)); new_line; */
 
	fwrite(text[NFANO+3]);
	fwrite(pi2a(cnfa->sno)); new_line;
	fwrite(text[NFANO+4]);
	fwrite(be2a(cnfa->is_eps));
/*	fwrite(be2a(FALSE));*/
	fwrite(be2a(cnfa->minimal));
	fwrite(text[NFANO+5]);
	for(i=0; i<=cnfa->qno; )
		fwrite(i2a(cnfa->infin[i++]));
	fwrite(text[NFANO+6]);
	for(k=0; k<=lastdelta(cnfa->qno); k++)
	    for(i=(cnfa->is_eps)?0:1; i<=cnfa->sno; i++)
		for(j=0; j<=cnfa->qno; j++) {
			fwrite(i2a((int)cnfa->delta[i][j][k]));
		}
	fwrite(text[NFANO+7]);
} /* writenfa */


/* ---------------------------------------------------- write DCL */
static void writedcl(dstruct cdcl) { 
  posint i;
  register posint j;
  register darray dc;
	fwrite(text[DCLNO]);
	fwrite(pi2a(cdcl->dno)); new_line;
	fwrite(text[DCLNO+1]);
	fwrite(pi2a(cdcl->rno)); new_line;
	fwrite(text[DCLNO+2]);
	fwrite(pi2a(cdcl->lno)); new_line;

/*	fwrite(text[DCLNO+3]);
	fwrite(pi2a(cdcl->minrang)); new_line; */

	fwrite(text[DCLNO+4]);
	dc = cdcl->dclassarray;
	for(i=0; i < cdcl->dno; i++) {
		fwrite(text[DCLNO+5]);
		fwrite(pi2a(dc[i]->org)); new_line;
		fwrite(pi2a(dc[i]->rno)); new_line;
		fwrite(pi2a(dc[i]->lno)); new_line;
		fwrite(pi2a(dc[i]->hsize)); new_line;
		fwrite(pi2a(dc[i]->rang)); new_line;
		fwrite(pi2a(dc[i]->maxlen)); new_line;
		fwrite(be2a(dc[i]->regular));
		fwrite(text[DCLNO+6]);
		for(j=0; j < dc[i]->lno; ) {
			fwrite(pi2a(dc[i]->lrep[j++])); new_line;
		}
		fwrite(text[DCLNO+7]);
		for(j=0; j < dc[i]->rno; ) {
			fwrite(pi2a(dc[i]->rrep[j++])); new_line;
		}
		fwrite(text[DCLNO+8]);
		for(j=0; j < dc[i]->hsize; ) {
			fwrite(pi2a(dc[i]->hclass[j++])); new_line;
		}
	}
} /* writedcl */


/* ---------------------------------------------------- write MONOID */
static void writemon(register monoid cmon) { 
  register posint i,j;
	fwrite(text[MONNO]);
	fwrite(pi2a(cmon->qno)); new_line;
	fwrite(text[MONNO+2]);
	fwrite(pi2a(cmon->sno)); new_line;
	fwrite(text[MONNO+3]);
	fwrite(pi2a(cmon->mno)); new_line;
	fwrite(text[MONNO+4]);
	fwrite(pi2a(cmon->gno)); new_line;
	fwrite(text[MONNO+8]);
	fwrite(pi2a(cmon->zero)); new_line;
	fwrite(text[MONNO+14]);

  /* oma */
  if (cmon->relationcomputed) 
    { 
      fwrite(pi2a(cmon->rs->rno)); new_line;
      for(i=0; i < cmon->rs->rno; i++) 
	{
	  fwrite(pi2a(cmon->rs->rside[i])); new_line;
	  fwrite(pi2a(cmon->rs->lside[i])); new_line;
	}
    } 
  else 
    {  
      fwrite(pi2a(0)); new_line; /* cmon->rs->rno == 0 indicates that 
				    defining relations have not been
				    computed */
    }
  
	fwrite(text[MONNO+5]);
	for(i=0; i <= cmon->gno; ) {
		fwrite(pi2a(cmon->generator[i++])); new_line;
	}
	fwrite(text[MONNO+6]);
	for(i=0; i <= cmon->sno; ) {
		fwrite(pi2a(cmon->let2gen[i++])); new_line;
	}
	fwrite(text[MONNO+7]);
	for(i=0; i < cmon->mno; i++) {
		for(j=1; j <= cmon->gno; ) {
			fwrite(pi2a(cmon->gensucc[i][j++])); new_line;
		}
		new_line;
	}
	fwrite(text[MONNO+11]);
	fwrite(be2a(cmon->dclassiscomputed));
	fwrite(be2a(cmon->mequals));
	fwrite(text[MONNO+12]);
	if (cmon->dclassiscomputed) {
		fwrite(text[MONNO+13]);
		
/*		for(i=0; i < cmon->mno; i++) {
			fwrite(pi2a(0)); new_line; 
		}
*/
		writedcl(cmon->ds);
	}
} /* writemon */

/* ---------------------------------------------------- write SFEXP */
static void writesf(register starfexp csf, posint mno) { 
  register posint i,j;
	fwrite(text[SFENO]);
	for(i=0; i < mno;) {
		fwrite(pi2a(csf->ulength[i++])); new_line;
	}
	fwrite(text[SFENO+1]);
	for(i=0; i < mno;) {
		fwrite(pi2a(csf->vlength[i++])); new_line;
	}
	fwrite(text[SFENO+2]);
	for(i=0; i < mno;) {
		fwrite(pi2a(csf->w0length[i++])); new_line;
	}
	fwrite(text[SFENO+3]);
	for(i=0; i < mno;) {
		fwrite(pi2a(csf->w1length[i++])); new_line;
	}
	fwrite(text[SFENO+4]);
	for(i=0; i < mno; i++) for(j=0; j < 2*csf->ulength[i];) {
		fwrite(pi2a(csf->u[i][j++])); new_line;
	}
	fwrite(text[SFENO+5]);
	for(i=0; i < mno; i++) for(j=0; j < 2*csf->vlength[i];) {
		fwrite(pi2a(csf->v[i][j++])); new_line;
	}
	fwrite(text[SFENO+6]);
	for(i=0; i < mno; i++) for(j=0; j < 3*csf->w0length[i];) {
		fwrite(pi2a(csf->w0[i][j++])); new_line;
	}
	fwrite(text[SFENO+7]);
	for(i=0; i < mno; i++) for(j=0; j < csf->w1length[i];) {
		fwrite(pi2a(csf->w1[i][j++])); new_line;
	}
	fwrite(text[SFENO+8]);
	for(i=0; i < mno;) fwrite(be2a(csf->computed[i++]));
	fwrite(text[SFENO+9]);
} /* writesf */

/* ---------------------------------------------------- write LANGUAGE */

boolx writelan(language lanout, char *workdir ) {

  char fname[ PATHLEN+NLEN+5+100 ];

  if (workdir != NULL) {
    (void)strcpy(fname,workdir);
    (void)strcat(fname,"/");
    (void)strcat(fname,lanout->name);
  } else {
    (void)strcpy(fname,lanout->name);
  }
  (void)strcat(fname,".amr");
#ifdef DEBUG
  fprintf(stdout, "\nwritelan: writing <%s> to file \"%s\"",lanout->name,fname);
  fflush(stdout);
#endif
  if ((amrp=fopen(fname,"w")) == NULL) {
#ifdef DEBUG
    perror("writelan 1");
#endif
    return FALSE;
  }

    fwrite(text[GLBNO]);fwrite(versiontext[CUR_VERSION]);
    fwrite(text[GLBNO+1]);
    fwrite(lanout->name); new_line;
    fwrite(lanout->ref); new_line;
    fwrite(text[GLBNO+2]);
    fwrite(pi2a(lanout->sizeal)); new_line;
    fwrite(text[GLBNO+3]);
    fwrite(i2a(lanout->inputis));
    
    switch (lanout->inputis) {
	case REXINP:
	case DFAINP:
	case NFAINP:break;
	case OREXINP: writerex(lanout->orex); break;
	case ODFAINP: writedfa(lanout->odfa); break;
	case ONFAINP: writenfa(lanout->onfa); break;
	case OENFAINP:writenfa(lanout->oenfa); break;
	default: errex(SWERR,"WRITELAN");
	}
	fwrite(text[GLBNO+4]);
	fwrite(text[GLBNO+5]);
	fwrite(be2a(lanout->brex));
	if (lanout->brex) writerex( lanout->lrex);
	fwrite(text[GLBNO+6]);
	fwrite(be2a(lanout->bdfa));
	if (lanout->bdfa) writedfa( lanout->ldfa);
	fwrite(text[GLBNO+7]);
	fwrite(be2a(lanout->bnfa));
	if (lanout->bnfa) writenfa( lanout->lnfa);
	fwrite(text[GLBNO+8]);
	fwrite(be2a(lanout->bmon));
	if (lanout->bmon) writemon( lanout->lmon);
	fwrite(text[GLBNO+9]);
	fwrite(be2a(lanout->stariscomp));
	if (lanout->stariscomp) writesf( lanout->sexp,lanout->lmon->mno);
	fwrite(text[GLBNO+10]);
	fwrite(bx2a(lanout->empty));
	fwrite(bx2a(lanout->full));
	fwrite(bx2a(lanout->folu));
	fwrite(bx2a(lanout->starfree));
	fwrite(bx2a(lanout->localtest));
	fwrite(bx2a(lanout->definite));
	fwrite(bx2a(lanout->revdefinite));
	fwrite(bx2a(lanout->gendefinite));
	fwrite(bx2a(lanout->dotdepth1));
	fwrite(bx2a(lanout->piecetest));
	fwrite(bx2a(lanout->nilpotent));
	fwrite(pi2a(lanout->localdegree));

	if (ferror(amrp)) {
		(void)fclose(amrp);
		return UN_KNOWN;
	}
	(void)fclose(amrp);
#ifdef UNIX
        if(chmod(fname,0664) == -1)
             return UN_KNOWN;
#endif
	return TRUE;
} /* writelan */


/* ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ */
/*                   READING DATA                                          */
/* ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ */

/* functions included:
 *
 *	static	int	a2i()
 *		posint	a2pi()
 *	static	boolx	a2bx()
 *	static	boole	a2be()
 *
 *	static	void	readstr()
 *	static	boole	rdinsf()
 *	static	boole	rdinrex()
 *	static	boole	rdinform()
 *	static	boole	rdindfa()
 *	static	boole	rdinnfa()
 *	static	boole	rdindcl()
 *	static	boole	rdinmon()
 *	      	boolx	rdinlan()
 */


/* input buffer */
static char line[LINELEN1];
static posint version; 

/** string to integer. 
 *  <p>uses sscanf and IFMT
 *  @param str string which is read out
 */
static int a2i(char *str) { 
    int i;
    
    (void)sscanf(str,IFMT,&i);
    return i;
} /* a2i */

/** string to positive integer. 
 *  <p>uses sscanf and PIFMT
 *  @param str string which is read out
 */
posint a2pi(char *str) { 
    posint pi;
    
    (void)sscanf(str,PIFMT,&pi);
    return pi;
} /* a2pi */

/** string to boolx type. 
 *  <p> uses sscanf. 
 *  <p> T = TRUE, F = FALSE, else UN_KNOWN
 *  @param str string which is read out
 */
static boolx a2bx(char *str) {	
    if (strcmp(str,"T\n") == 0) 
	return TRUE;
    if (strcmp(str,"F\n") == 0) 
	return FALSE;
    else 
	return UN_KNOWN;
} /* a2bx */

/** string to boole. 
 *  <p>uses sscanf. 
 *  <p>T = TRUE, else FALSE
 *  @param str string which is read out
 */
static boole a2be(char *str) {	
    if (strcmp(str,"T\n") == 0) 
	return TRUE;
    else 
	return FALSE;
} /* a2be */


/** reads string from line[ ]. 
 *  <p>stops also on newline. 
 *  <p>( line[ ] is a global char buffer )
 *  @param str  pointer to string which to fill
 *  @param strl length in bytes
 */
static void readstr(register char *str, posint strl) { 
    posint pos = 0;
    register posint j;
    
    while (strl) {
	frdin();
	for(j=0; line[j] != '\n'; ) {
	    str[pos++] = line[j++];	strl--;
	}	
    }
    frdin();
} /* readstr */

/** read starfree expression 
 *  @param csf pointer to the sexp structure to fill
 */
static boole rdinsf(register starfexp csf, posint mno) { 
    register posint i,j;
    csf->ulength = newar(mno);
    csf->vlength = newar(mno);
    csf->w0length = newar(mno);
    csf->w1length = newar(mno);
    csf->u = newarray1(mno);
    csf->v = newarray1(mno);
    csf->w0 = newarray1(mno);
    csf->w1 = newarray1(mno);
    csf->computed = newbarray(mno);
    
    /* "STARFREE EXPRESSION: LENGTH OF U\n" */
    frdin(); if (strcmp(line,text[SFENO]) != 0) {
	freesf(csf,FALSE,mno); return FALSE; }
    for(i=0; i < mno;) { frdin(); csf->ulength[i++] = a2pi(line); }
    /* "STARFREE EXPRESSION: LENGTH OF V\n" */
    frdin(); if (strcmp(line,text[SFENO+1]) != 0) {
	freesf(csf,FALSE,mno); return FALSE; }
    for(i=0; i < mno;) { frdin(); csf->vlength[i++] = a2pi(line); }
    /* "STARFREE EXPRESSION: LENGTH OF W0\n" */
    frdin(); if (strcmp(line,text[SFENO+2]) != 0) {
	freesf(csf,FALSE,mno); return FALSE; }
    for(i=0; i < mno;) { frdin(); csf->w0length[i++] = a2pi(line); }
    /* "STARFREE EXPRESSION: LENGTH OF W1\n" */
    frdin(); if (strcmp(line,text[SFENO+3]) != 0) {
	freesf(csf,FALSE,mno); return FALSE; }
    for(i=0; i < mno;) { frdin(); csf->w1length[i++] = a2pi(line); }
    
    for(i=0; i < mno; i++) {
	csf->u[i] = newar(2*csf->ulength[i]);
	csf->v[i] = newar(2*csf->vlength[i]);
	csf->w0[i] = newar(3*csf->w0length[i]);
	csf->w1[i] = newar(csf->w1length[i]);
    }
    /* "STARFREE EXPRESSION: U\n"*/
    frdin(); if (strcmp(line,text[SFENO+4]) != 0) {
	freesf(csf,TRUE,mno); return FALSE; }
    for(i=0; i < mno; i++) for(j=0; j < 2*csf->ulength[i];) {
	frdin(); csf->u[i][j++] = a2pi(line); }
    /* "STARFREE EXPRESSION: V\n"*/
    frdin(); if (strcmp(line,text[SFENO+5]) != 0) {
	freesf(csf,TRUE,mno); return FALSE; }
    for(i=0; i < mno; i++) for(j=0; j < 2*csf->vlength[i];) {
	frdin(); csf->v[i][j++] = a2pi(line); }
    /* "STARFREE EXPRESSION: W0\n"*/
    frdin(); if (strcmp(line,text[SFENO+6]) != 0) {
	freesf(csf,TRUE,mno); return FALSE; }
    for(i=0; i < mno; i++) for(j=0; j < 3*csf->w0length[i];) {
	frdin(); csf->w0[i][j++] = a2pi(line); }
    /* "STARFREE EXPRESSION: W1\n"*/
    frdin(); if (strcmp(line,text[SFENO+7]) != 0) {
	freesf(csf,TRUE,mno); return FALSE; }
    for(i=0; i < mno; i++) for(j=0; j < csf->w1length[i];) {
	frdin(); csf->w1[i][j++] = a2pi(line); }
    /* "STARFREE EXPRESSION: COMPUTED\n"*/
    frdin(); if (strcmp(line,text[SFENO+8]) != 0) {
	freesf(csf,TRUE,mno); return FALSE; }
    for(i=0; i < mno;) {frdin(); csf->computed[i++] = a2be(line);}
    /* "STARFREE EXPRESSION: END\n"*/
    frdin(); if (strcmp(line,text[SFENO+9]) != 0) {
	freesf(csf,TRUE,mno); return FALSE; }
    
    return TRUE;
} /* rdinsf */

/** read regular expression 
 *  @param crex pointer to the rex structure to fill
 */
static boole rdinrex(register regex crex) { 
    int i;
    
    /* "REG.EXP.: #ABB., #SYM0, #SYM1, #ALPHABET\n" */
    frdin(); if (strcmp(line,text[REGNO])) return FALSE;
    
    /* read #used abbreviations */
    frdin(); crex->useda = (int)a2pi(line) - 1;
    /* read size of alphabet */
    frdin(); crex->sno = a2pi(line);
    /* "REG.EXP.: LENGTH OF REX, EXPANDED REX\n" */
    frdin(); if (strcmp(line,text[REGNO+1])) return FALSE;
    /* see string above */
    frdin(); crex->rexl = a2pi(line);
    frdin(); crex->erexl = a2pi(line);
    /* "REG.EXP.: REX KNOWN, GENERALIZED REX\n" */
    frdin(); if (strcmp(line,text[REGNO+2])) return FALSE;
    /* ??? skipping "rex known?" */
    frdin();   
    frdin(); crex->grex = a2be(line);
    /* "REG.EXP.: LENGTH OF ABBREVIATIONS\n" */
    frdin(); if (strcmp(line,text[REGNO+3])) return FALSE;
    /* fill abbl(array that holds lengths of abbrs */
    for(i=0; i <= crex->useda; i++) {
	frdin(); crex->abbl[i] = a2pi(line);
    }
    /* "REG.EXP.: ABBREVIATIONS\n" */
    frdin(); if (strcmp(line,text[REGNO+4])) return FALSE;
    /* read abbr = array of strings */
    for(i=0; i<= crex->useda; i++) 
	crex->abbr[i] = newrexstr(crex->abbl[i]);
    crex->rex = newrexstr(crex->rexl);
    crex->exprex = newrexstr(crex->erexl);
    for(i=0; i <= crex->useda; i++) {
	readstr(crex->abbr[i],crex->abbl[i]);
	frdin();
    }
    
    /* "REG.EXP.: REX, EXPANDED REX\n" */
    frdin(); if (strcmp(line,text[REGNO+5])) {
	freerex(crex); return FALSE; }
    /* read in the regular expression ... */
    readstr(crex->rex,crex->rexl);
    /* skip a line */
    frdin();
    /*  and the expanded regular expression */
    readstr(crex->exprex,crex->erexl);
    return TRUE; /* done! */
} /* rdinrex */

/** read dfa
 *  @param cdfa pointer to the dfauto structure to fill
 */
static boole rdindfa(register dfa cdfa) { 
    register posint j,i;
    
    /* "DFA: #STATES\n" */ 
    frdin(); if (strcmp(line,text[DFANO]) != 0) return FALSE;
    frdin(); cdfa->qno = a2pi(line);
    /* "DFA: INITIAL STATE\n"*/
    frdin(); if (strcmp(line,text[DFANO+2]) != 0) return FALSE;
    frdin(); cdfa->init = a2pi(line);  
    /* "DFA: ALPHABET\n" */
    frdin(); if (strcmp(line,text[DFANO+3]) != 0) return FALSE;
    frdin(); cdfa->sno = a2pi(line);
    /* "DFA: MINIMAL\n" */
    frdin(); if (strcmp(line,text[DFANO+4]) != 0) return FALSE;
    frdin(); cdfa->minimal = a2be(line);
    /* "DFA: FINAL STATES\n" */
    frdin(); if (strcmp(line,text[DFANO+5]) != 0) return FALSE;
    /* allocate... */
    cdfa->final = newfinal(cdfa->qno);
    cdfa->delta = newddelta(cdfa->sno,cdfa->qno);
    /* populate final[] */
    for(i=0; i<=cdfa->qno; ) {
	frdin(); cdfa->final[i++] = a2bx(line);}
    /* "DFA: TRANSFORMATION\n" */
    frdin(); if(strcmp(line,text[DFANO+6]) != 0) {
	freedfa(cdfa);
	return FALSE;
    }
    /* read in transitions */
    for(i=1; i<=cdfa->sno; i++)
	for(j=0; j<=cdfa->qno; ) {
	    frdin(); cdfa->delta[i][j++] = a2pi(line); 
	}
    /* "DFA: END\n" */
    frdin(); if(strcmp(line,text[DFANO+7]) != 0) {
	freedfa(cdfa);
	return FALSE;
    }
    return TRUE;
} /* rdindfa */

/** read nfa
 *  @param cnfa pointer to the nfauto structure to fill
 */
static boole rdinnfa(nfa cnfa)
{ 
    register posint k,j,i;
    /* "NFA: #STATES\n" */ 
    frdin(); if (strcmp(line,text[NFANO]) != 0) return FALSE;
    frdin(); cnfa->qno = a2pi(line);
    /* when reading older automata with several initial states, the qno might
       be 1 higher than expected. The additional state will be superflous */
    /* do not worry, it will not cause any troubles */
    
    if (version<1)
	{ 
	    /* "NFA: INITIAL STATE\n" */
	    frdin(); if (strcmp(line,text[NFANO+2]) != 0) return FALSE;
	    frdin(); /* read a dummy */
	}
    /* "NFA: ALPHABET\n" */  
    frdin(); if (strcmp(line,text[NFANO+3]) != 0) return FALSE;
    frdin(); cnfa->sno = a2pi(line);
    /* "NFA: EPSILON, MINIMAL\n" */
    frdin(); if (strcmp(line,text[NFANO+4]) != 0) return FALSE;
    frdin(); cnfa->is_eps = a2be(line);
    if (version<1)
	{
	    frdin(); /* reada dummy */
	}
    frdin(); cnfa->minimal = a2be(line);
    /* "NFA: FINAL STATES\n" */	
    frdin(); if (strcmp(line,text[NFANO+5]) != 0) return FALSE;
    /* allocate ... */
    cnfa->infin = newfinal(cnfa->qno);
    cnfa->delta = newndelta(cnfa->sno,cnfa->qno);
    /* read in labels (initial,final) */
    for(i=0; i<=cnfa->qno; ) {
	frdin(); cnfa->infin[i++] = a2i(line);}
    /* "NFA: TRANSFORMATION\n" */
    frdin(); if(strcmp(line,text[NFANO+6]) != 0) {
	freenfa(cnfa);
	return FALSE;
    }
    /* read in transitions */
    for(k=0; k<=lastdelta(cnfa->qno); k++)
	for(i=(cnfa->is_eps)?0:1; i<=cnfa->sno; i++)
	    for(j=0; j<=cnfa->qno; j++) {
		frdin(); cnfa->delta[i][j][k] = (char)a2i(line); }
    /* "NFA: END\n" */
    frdin(); if(strcmp(line,text[NFANO+7]) != 0) {
	freenfa(cnfa);
	return FALSE;
    }
    return TRUE;
} /* rdinnfa */

/** read dclass
 *  @param c pointer to the dstruct structure to fill
 */
static boole rdindcl(dstruct *c)
{ 
    register posint i,j;
    darray dc;
    (*c) = newdstruct();
    /* "D-CLASS: #DCLASSES\n" */
    frdin(); if (strcmp(line,text[DCLNO]) != 0) {
	freedcl((*c),0); return FALSE; }
    frdin(); (*c)->dno = a2pi(line);
    /* "D-CLASS: #RCLASSES\n" */
    frdin(); if (strcmp(line,text[DCLNO+1]) != 0) {
	freedcl((*c),0); return FALSE; }
    frdin(); (*c)->rno = a2pi(line);
    /* "D-CLASS: #LCLASSES\n" */
    frdin(); if (strcmp(line,text[DCLNO+2]) != 0) {
	freedcl((*c),0); return FALSE; }
    frdin(); (*c)->lno = a2pi(line);
    
    (*c)->dclassarray = newdarray((*c)->dno);
    dc = (*c)->dclassarray;
    for(i=0; i < (*c)->dno; i++) dc[i] = newdclass();
    
    if(version<1) {
	/* "D-CLASS: MINRANG\n" */
	frdin(); if (strcmp(line,text[DCLNO+3]) != 0) {
	    freedcl((*c),0); return FALSE; }
	frdin(); /*dummy */
    }
    
    /* "D-CLASS: THE CLASSES\n" */
    frdin(); if (strcmp(line,text[DCLNO+4]) != 0) {
	freedcl((*c),0); return FALSE; }
    for(i=0; i < (*c)->dno; i++) {
	frdin();
	/* "D-CLASS-CLASSES: ORG,RNO,LNO,HSIZE,RANG AND REGULAR\n" */
	if (strcmp(line,text[DCLNO+5]) != 0) {
	    freedcl((*c),i); return FALSE; }
	frdin(); dc[i]->org = a2pi(line);
	frdin(); dc[i]->rno = a2pi(line);
	frdin(); dc[i]->lno = a2pi(line);
	frdin(); dc[i]->hsize = a2pi(line);
	frdin(); dc[i]->rang = a2pi(line);
	frdin(); dc[i]->maxlen = a2pi(line);
	frdin(); dc[i]->regular = a2be(line);
	dc[i]->lrep = newar(dc[i]->lno);
	dc[i]->hclass = newar(dc[i]->hsize);
	dc[i]->rrep = newar(dc[i]->rno);
	
	/* "D-CLASS THE CLASSES: L-REP\n" */
	frdin(); if (strcmp(line,text[DCLNO+6]) != 0) {
	    freedcl((*c),i+1); return FALSE; }
	for(j=0; j < dc[i]->lno; ) { frdin(); dc[i]->lrep[j++] = a2pi(line); }
	/* "D-CLASS THE CLASSES: R-REP\n" */
	frdin(); if (strcmp(line,text[DCLNO+7]) != 0) {
	    freedcl((*c),i+1); return FALSE; }
	for(j=0; j < dc[i]->rno; ) { frdin(); dc[i]->rrep[j++] = a2pi(line); }
	
	/* "D-CLASS THE CLASSES: H-CLASS\n" */
	frdin(); if (strcmp(line,text[DCLNO+8]) != 0) {
	    freedcl((*c),i+1); return FALSE; }
	for(j=0; j < dc[i]->hsize; ) { frdin(); dc[i]->hclass[j++] = a2pi(line); }
    }
    return TRUE;
} /* rdindcl */

/** read monoid
 *  @param cmon pointer to the mono structure to fill
 */
static boole rdinmon(register monoid cmon)
{ 
    register posint i,j;
    posint rno;
    /* "MONOID: #STATES\n"*/
    frdin(); if (strcmp(line,text[MONNO]) != 0) return FALSE;
    frdin(); cmon->qno = a2pi(line);
    /* "MONOID: #LETTERS\n"*/
    frdin(); if (strcmp(line,text[MONNO+2]) != 0) return FALSE;
    frdin(); cmon->sno = a2pi(line);
    /* "MONOID: #ELEMENTS\n"*/
    frdin(); if (strcmp(line,text[MONNO+3]) != 0) return FALSE;
    frdin(); cmon->mno = a2pi(line);
    /* "MONOID: #GENERATORS\n"*/
    frdin(); if (strcmp(line,text[MONNO+4]) != 0) return FALSE;
    frdin(); cmon->gno = a2pi(line);
    /* "MONOID: ZERO of THE MONOID\n"*/
    frdin(); if (strcmp(line,text[MONNO+8]) != 0) return FALSE;
    frdin(); cmon->zero = a2pi(line);
    /* "MONOID: #RELATIONS, (RIGHT,LEFT)-SIDE\n"*/
    frdin(); if (strcmp(line,text[MONNO+14]) != 0) return FALSE;
    frdin(); rno = a2pi(line); /* if 0 then defining relations have
				  not been computed */
    
    /* oma */
    if (rno != 0) 
	{
	    cmon->relationcomputed = TRUE;
	    cmon->rs = newrstruct();
	    cmon->rs->rno = rno;
	    cmon->rs->rside = newar(rno);
	    cmon->rs->lside = newar(rno);
	    for(i=0; i < rno; i++ )
		{
		    frdin(); cmon->rs->rside[i] = a2pi(line);
		    frdin(); cmon->rs->lside[i] = a2pi(line); 
		}
	}
    else
	{
	    cmon->relationcomputed = FALSE;
	}
    
    cmon->generator = newar(cmon->gno + 1);
    cmon->let2gen = newar(cmon->sno + 1);
    cmon->gensucc = newarray1(cmon->mno);
    cmon->no2trans = newarray1(cmon->mno);
    for(i=0;i<cmon->mno;) {
	cmon->gensucc[i]=newar(cmon->gno);
	cmon->no2trans[i++]=newar(cmon->qno);
    }
    
    /* "MONOID: LIST OF GENERATORS\n"*/
    frdin(); if (strcmp(line,text[MONNO+5]) != 0) return FALSE;
    for(i=0; i <= cmon->gno; ) { frdin(); cmon->generator[i++] = a2pi(line); }
    /* "MONOID: LETTER TO GENERATOR WITH SAME TRANSFORMATION\n"*/
    frdin(); if (strcmp(line,text[MONNO+6]) != 0)  {
	freemon(cmon); return FALSE; }
    for(i=0; i <= cmon->sno; ) { frdin(); cmon->let2gen[i++] = a2pi(line); }
    /* "MONOID: GENERATOR SUCCESSORS OF ELEMENTS\n"*/
    frdin(); if (strcmp(line,text[MONNO+7]) != 0) {
	freemon(cmon); return FALSE; }
    for(i=0; i < cmon->mno; i++) { for(j=1; j <= cmon->gno; ) {
	frdin(); cmon->gensucc[i][j++] = a2pi(line); } frdin();
    }
    /* "MONOID: BOOLEAN VALUES (DCLASSISCOMPUTED, MEQUALS)\n"*/
    frdin(); if (strcmp(line,text[MONNO+11]) != 0) {
	freemon(cmon); return FALSE; }
    frdin(); cmon->dclassiscomputed = a2be(line);
    frdin(); cmon->mequals = a2be(line);
    /* "MONOID: END\n"*/
    frdin(); if (strcmp(line,text[MONNO+12]) != 0) {
	freemon(cmon); return FALSE; }
    
    if (cmon->dclassiscomputed) {
	/* "MONOID: RANG\n" */
	frdin(); if(strcmp(line,text[MONNO+13]) != 0) {
	    freemon(cmon); return FALSE; }
	if(version<1)
	    {
		for(i=0; i < cmon->mno;i++ )
		    frdin(); /* dummy */
	    }
	if (!rdindcl(&(cmon->ds))) 
	    cmon->dclassiscomputed = FALSE;
    }
    return TRUE;
} /* rdinmon */

/** read language
 *  @param fname (file)name of the language (without ".amr"
 *  @param workdir  dir to save in
 */
language rdinlan(char *fname, char *workdir) { 
    int i;
    language lanin = newlang();

    fprintf(stdout, "loading %s from %s",fname,workdir);
    fflush(stdout);

    for(i=0; i < NLEN; i++) lanin->name[i] = fname[i]; /* copy name */
    
    /* open file ... */
    if( workdir != NULL ) {
      (void)strcpy(fname,workdir);
      (void)strcat(fname,"/");
      (void)strcat(fname,lanin->name);
    } 
    (void)strcat(fname,".amr");

    fprintf(stdout, "loading file named %s ",fname);
    fflush(stdout);

    if ((amrp=fopen(fname,"r")) == NULL) {
#ifdef DEBUG
	perror("rdinlan 1");
#endif
	return NULL;
    }
    /* ...end open file */
    
    /* from here on, quoted strings in comments mean, this is expected */
    /* in the next line... strings are from 'amrtext.h' */
    
    
    frdin();
    /* "AMORE Language Save File" = 24 chars long */
    if(strncmp(line,text[GLBNO],LEN_OF_HEADER) != 0) 
      return NULL;
    
    /* "\n"(v0) oder " VERSION 1.0"(v1) */
    version=CUR_VERSION+1;
    for(i=0;i<=CUR_VERSION;i++)
	if(strcmp(&(line[LEN_OF_HEADER]),versiontext[i]) == 0) /* BE = 24 */
	    {
		version=i;
		break;
	    }
    
    if (version==CUR_VERSION+1) return NULL;
    
    
    frdin(); 
    /*  "NAME \n"                                                 */
    if (strcmp(line,text[GLBNO+1]) != 0) return NULL;
    /*  <name of the language>, equal to filename, forget it      */
    frdin();
    /*  <reference string>, better:description                    */
    frdin(); (void)strcpy(lanin->ref,line);
    
    lanin->ref[strlen(line)-1] = '\0'; /* delete '\n' */
    lanin->saved = TRUE;
    
    /*  "ALPHABET \n" */
    frdin(); if (strcmp(line,text[GLBNO+2]) != 0) return NULL;
    /* read pos. int */
    frdin(); lanin->sizeal = a2pi(line);
    /*   "INPUT\n" */
    frdin(); if (strcmp(line,text[GLBNO+3]) != 0) return NULL;
    /* where is the user input ? read integer */
    frdin(); lanin->inputis = a2i(line);
    
    switch (lanin->inputis) {
	
    case REXINP:
    case DFAINP:
    case NFAINP: break; /* if user entered fa, do nothing ???? */
	
    case OREXINP: lanin->orex = newrex(); /* original rex */
	if (!rdinrex(lanin->orex)) {        /* reads rex ... to lanin->orex*/
	    dispose(lanin->orex);             /* other cases similar */
	    lanin->inputis = REXINP;
	    return NULL;
	} break;
	
    case ODFAINP: lanin->odfa = newdfa(); /* original dfa */
	if (!rdindfa(lanin->odfa)) {
	    dispose(lanin->odfa);
	    lanin->inputis = DFAINP;
	    return NULL;
	} break;
	
    case ONFAINP: lanin->onfa = newnfa(); /* original nfa */
	if (!rdinnfa(lanin->onfa)) {
	    dispose(lanin->onfa);
	    lanin->inputis = NFAINP;
	    return NULL;
	} break;
	
    case OENFAINP:lanin->oenfa = newnfa(); /* original eps-nfa */
	if (!rdinnfa(lanin->oenfa)) {
	    dispose(lanin->oenfa);
	    lanin->inputis = OENFAINP;
	    return NULL;
	} break;
    default: return NULL; /* error */
    }
    
    /* "BOOLE FLAGS: (BREX,BDFA,BNFA,BENFA,BMON) \n"   ((completely useless))*/
    frdin(); if (strcmp(line,text[GLBNO+4]) != 0) return NULL;
    /* "REGULAR EXPRESSION \n" */
    frdin(); if (strcmp(line,text[GLBNO+5]) != 0) return NULL;
    
    /* read 'boole' (in a2be ... "T" = True, else False) */
    frdin(); lanin->brex = a2be(line);
    
    if (lanin->brex) { /* read in rex */
	lanin->lrex = newrex();
	if (!rdinrex(lanin->lrex)) {
	    dispose(lanin->lrex);
	    lanin->brex = FALSE;
	    return NULL;
	}	
    }
    
    /* "DETERMINISTIC FINITE AUTOMATON \n" */
    frdin(); if (strcmp(line,text[GLBNO+6]) != 0) return NULL;
    /* read 'boole' */
    frdin(); lanin->bdfa = a2be(line);
    
    if (lanin->bdfa) { /* read in dfa */
	lanin->ldfa = newdfa();
	if (!rdindfa(lanin->ldfa)) {
	    dispose(lanin->ldfa);
	    lanin->bdfa = FALSE;
	    return NULL;
	}	
    }
    
    /* "NONDETERMINISTIC FINITE AUTOMATON \n" */
    frdin(); if (strcmp(line,text[GLBNO+7]) != 0) return NULL;
    /* read 'boole' */
    frdin(); lanin->bnfa = a2be(line);
    
    if (lanin->bnfa) { /* read in nfa */
	lanin->lnfa = newnfa();
	if (!rdinnfa(lanin->lnfa)) {
	    dispose(lanin->lnfa);
	    lanin->bnfa = FALSE;
	    return NULL;
	}	
    }
    
    /* "MONOID \n" */
    frdin(); if (strcmp(line,text[GLBNO+8]) != 0) return NULL;
    /* read boole */
    frdin(); lanin->bmon = a2be(line);
    if (lanin->bmon) { /* read in monoid */
	lanin->lmon = newmon();
	if (!rdinmon(lanin->lmon)) {
	    dispose(lanin->lmon);
	    lanin->bmon = FALSE;
	    return NULL;
	}
	comprestofmon(lanin->lmon,lanin->ldfa); /* compute resst ??? */
    }
    
    /* "STARFREE EXPRESSION \n" */
    frdin(); if(strcmp(line,text[GLBNO+9]) != 0) return NULL;
    /* read 'boole' */
    frdin(); lanin->stariscomp = a2be(line);
    if ((lanin->bmon) && (lanin->stariscomp)) {
	lanin->sexp = newsfexp();
	if (!rdinsf(lanin->sexp,lanin->lmon->mno)) {
	    dispose(lanin->sexp);
	    lanin->stariscomp = FALSE;
	    return NULL;
	}	
    }
    
    /* "EMPTY,FULL,FOLU,SF,LOC.TESTABLE,DEFINITE,REV.D.,GEN.D.,DOTDEPTH,NILPOTENT\n" */
    frdin(); if (strcmp(line,text[GLBNO+10]) != 0) return NULL;
    /* read 'boolx' types (True, False, Unknown) and one pos. int */
    frdin(); lanin->empty = a2bx(line);
    frdin(); lanin->full = a2bx(line);
    frdin(); lanin->folu = a2bx(line);
    frdin(); lanin->starfree = a2bx(line);
    frdin(); lanin->localtest = a2bx(line);
    frdin(); lanin->definite = a2bx(line);
    frdin(); lanin->revdefinite = a2bx(line);
    frdin(); lanin->gendefinite = a2bx(line);
    frdin(); lanin->dotdepth1 = a2bx(line);
    frdin(); lanin->piecetest = a2bx(line);
    frdin(); lanin->nilpotent = a2bx(line);
    frdin(); lanin->localdegree = a2pi(line);
    
    if (ferror(amrp)) {
	(void)fclose(amrp);
	return NULL;
    }
    (void)fclose(amrp);
    
    /* what if v0 file encountered ?? */
    if( version != CUR_VERSION)
	{
	  /* help(FALSE,"Older Save-File version, converting to new... "); */
	    switch(writelan( lanin, workdir))
		{
		case TRUE:
		  /*help(FALSE,"Older Save-File version, converting to new...o.k");*/
		  break;
		case FALSE:
		  /* help(FALSE,"SAVE LANGUAGE: error! cannot open file."); */
		    return NULL;
		case UN_KNOWN:
		  /* help(FALSE,"SAVE LANGUAGE: error in writing"); */
		    return NULL;
		}					
	}
    return lanin;
} /* rdinlan */
