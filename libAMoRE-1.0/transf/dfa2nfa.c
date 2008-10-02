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

#include "dfa2nfa.h"

/* (trivial) transformation of a dfa to a nfa */

nfa dfa2nfa(register dfa da) { 
  register nfa result;
  register posint state;
  register posint letter;
        result = newnfa();
        result->qno = da->qno;
        result->sno = da->sno;
        result->minimal = FALSE;
        result->is_eps = FALSE;
        result->infin = newfinal(result->qno);
        result->delta = newndelta(result->sno,result->qno);
        setinit(result->infin[da->init]);
        for(state=0; state<=result->qno; state++)
                setfinal(result->infin[state],da->final[state]);
        for(letter=1; letter<=result->sno; letter++)
                for(state=0; state<=result->qno; state++) {
                        connect(result->delta,letter,state,da->delta[letter][state]);
                }
        return result;
} /* dfa2nfa */
