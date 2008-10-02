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

/* regexp.c
 */

#include "regexp.h"

regex newrex() { 
    regex hlp;
    hlp = (regex)calloc(1, sizeof(struct rexstruct));
    hlp->useda = -1;
    if (hlp == NULL) {
#ifdef DEBUG
	perror("newrex 1"); 
#endif
	OUT_OF_MEM();
    }
    return hlp;
} /* newrex */

string newrexstr(posint strl) { 
    string hlp;
    hlp = (string)calloc(strl+1, sizeof(char));
    if (hlp == NULL) {
#ifdef DEBUG
	perror("newrexstr 1"); 
#endif
	OUT_OF_MEM();
    }
    return hlp;
} /* newrexstr */

void freerex(regex rx) { 
    register int i;
    for(i=0; i<=rx->useda; i++) dispose(rx->abbr[i]);
    dispose(rx->rex);
    dispose(rx->exprex);
} /* freerex */
