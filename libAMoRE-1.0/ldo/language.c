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

#include <amore/language.h>

language newlang()
{
	language hlp;

	hlp = (language) malloc(sizeof(struct regset));

	hlp->name = (char *) calloc(NLEN, sizeof(char));
	hlp->ref = (char *) calloc(REFLEN, sizeof(char));

	hlp->saved = FALSE;
	hlp->sizeal = 0;
	/* the properties of the language are UN_KNOWN */
	hlp->empty = UN_KNOWN;
	hlp->full = UN_KNOWN;
	hlp->folu = UN_KNOWN;
	hlp->starfree = UN_KNOWN;
	hlp->localtest = UN_KNOWN;
	hlp->definite = UN_KNOWN;
	hlp->revdefinite = UN_KNOWN;
	hlp->gendefinite = UN_KNOWN;
	hlp->dotdepth1 = UN_KNOWN;
	hlp->piecetest = UN_KNOWN;
	hlp->nilpotent = UN_KNOWN;

	hlp->brex = FALSE;
	hlp->stariscomp = FALSE;
	hlp->bdfa = FALSE;
	hlp->bnfa = FALSE;
	hlp->bmon = FALSE;
	hlp->inputis = 0;
	return hlp;
}				/* newlang */

void freelang(language lan)
{
	dispose(lan->name);
	dispose(lan->ref);

	if(lan->brex) {
		freerex(lan->lrex);
		dispose(lan->lrex);
	}
	if(lan->stariscomp) {
		freesf(lan->sexp, TRUE, lan->lmon->mno);
		dispose(lan->sexp);
	}
	if(lan->bdfa) {
		freedfa(lan->ldfa);
		dispose(lan->ldfa);
	}
	if(lan->bnfa) {
		freenfa(lan->lnfa);
		dispose(lan->lnfa);
	}
	if(lan->bmon) {
		freemon(lan->lmon);
		dispose(lan->lmon);
	}

	switch (lan->inputis) {
	case OREXINP:
		freerex(lan->orex);
		dispose(lan->orex);
		break;
	case ODFAINP:
		freedfa(lan->odfa);
		dispose(lan->odfa);
		break;
	case ONFAINP:
		freenfa(lan->onfa);
		dispose(lan->onfa);
		break;
	case OENFAINP:
		freenfa(lan->oenfa);
		dispose(lan->oenfa);
		break;
	}
}
