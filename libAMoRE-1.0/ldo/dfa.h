/*! \file dfa.h
 *  \brief declarations for deterministic FA
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
#ifndef _DFA_H
#define _DFA_H

#include "global.h" /* basic types */

/** deterministic transition of a letter. array of states, see ddelta
 *  <p>trans[ source ] = dest
 *  <p>means "transition from source to dest".
 *  <p>(indexed with state numbers from 0 to qno)
 */
typedef posint *trans;

/** deterministic transition table. is array of transitions
 *  <p>ddelta[i][source] = dest
 *  <p>means "transition labelled with letter i from source to dest"
 *  <p>(indexed with letter numbers from 1 to sno) 
 */
typedef trans  *ddelta;


/** a deterministic finite automaton
 *  @ingroup LDO_DECL
 */
struct dfauto {
    /** maximal state number, minimal state number is 0 */
    posint qno; 
    /** initial state */
    posint init;     
    /** size of alphabet    */
    posint sno;    
    /** marks for final states  */
    mrkfin final;    
    /** det.\  transition function   */ 
    ddelta delta;  
    /** true iff automaton is minimal */ 
    boole  minimal;  
} ;

/** pointer to a deterministic finite automaton 
 *  @ingroup LDO_DECL
 */
typedef struct dfauto *dfa;


/** allocates a new dfa */
dfa	 newdfa() ;

/** clone the dfa */
dfa  clonedfa(dfa n) ;

/** frees the memory used by the dfa da */
void freedfa(dfa da) ;

/** allocate memory for the transition function of a deterministic automaton
 * s,q are the maximal letter and the max. state
 */

ddelta newddelta(posint s,posint q) ;

#endif
