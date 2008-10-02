/*!\file
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
#ifndef _DFAMDFA_H
#define _DFAMDFA_H

#include "dfa.h"

/** return minimal automaton
 * \ingroup TRANSF_A_FROM_DFA
 * \param free if free==TRUE then the memory of indfa is free after the procedure, if not free indfa isn't changed after computation 
 */
dfa   dfamdfa(dfa indfa,boole free) ;

// dfa   dfammdfa(dfa indfa, array *transformation) ;

#endif
