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

#include "sfexp.h"

/** allocates a new starfexp
 */
starfexp newsfexp() { 
    starfexp hlp;
    hlp = (starfexp)calloc(1, sizeof(struct sexp));
    if (hlp == NULL) {
#ifdef DEBUG
	perror("newsfexp 1"); 
#endif
	OUT_OF_MEM();
    }
    return hlp;
} /* newsfexp */

void freesf(starfexp sf, boole flag, posint mno) {
    register posint i;
    dispose(sf->computed);
    dispose(sf->ulength);
    dispose(sf->vlength);
    dispose(sf->w0length);
    dispose(sf->w1length);
    if (flag) for(i=0; i< mno; i++) {
	dispose(sf->u[i]);
	dispose(sf->v[i]);
	dispose(sf->w0[i]);
	dispose(sf->w1[i]);
    }
    dispose(sf->u);
    dispose(sf->v);
    dispose(sf->w0);
    dispose(sf->w1);
} /* freesf */
