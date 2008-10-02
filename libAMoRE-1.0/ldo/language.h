/*! \file language.h
 *  \brief declarations for language
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
#ifndef _LANGUAGE_H
#define _LANGUAGE_H


#include "global.h"
#include "dfa.h" 
#include "nfa.h" 
#include "regexp.h"
#include "sfexp.h"
#include "mon.h"

#define NLEN	20  /*< max. number of charaters in a language name */
#define REFLEN  60  /*< max. number of charaters in reference */


/** \defgroup MNEMO Mnemonics for location of input LDO
 *  \ingroup LANGUAGE
 * @{
 */
#define REXINP		0
#define DFAINP		1
#define NFAINP		2
/* no ENFAINP this is always 13 */
#define OREXINP		10
#define ODFAINP		11
#define ONFAINP		12
#define OENFAINP	13
//@}

/** a regular language.
 *  @ingroup LANGUAGE
 *  More specifically, a regset structure contains pointers to various 
 *   LDOs all recognizing the same language.
 */
struct regset {
    /** name of language, must not be longer than NLEN */
    char      *name;
    /** reference = description, must not be longer than REFLEN */
    char      *ref;
    /** TRUE iff file exists */
    boole     saved;          
    /** holds the location of the input ldo.
     * 0 regex
     *
     * 1 dfa
     *
     * 2 nfa
     *
     * 3 eps-nfa
     *
     * 10 org. regex
     *
     * 11 org. dfa
     *
     * 12 org. nfa
     *
     * 13 org. eps-nfa
     */
    int       inputis;        
    /** size of alphabet */
    posint    sizeal;         
    /**  reg.\ expr. */  
    regex     lrex;           
    /** original reg.\ expr.\ if inputis == 10 */
    regex     orex;           
    /** false iff lrex == nil */
    boole     brex;           
    /** starfree expr. */
    starfexp  sexp;           
    /** starfree expression is computed */
    boole     stariscomp;      
    /** pt to a dfa */
    dfa       ldfa;          
    /** original dfa if inputis == 11 */
    dfa       odfa;           
    /** false iff ldfa == nil */
    boole     bdfa;           
    /** nfa */
    nfa       lnfa;           
    /** original nfa if inputis == 12 */
    nfa       onfa;           
    /** false iff lnfa == nil */
    boole     bnfa;            
    /** original enfa if inputis == 13 */
    nfa       oenfa;          
    /** monoid */
    monoid    lmon;           
    /** false iff lmon == nil */
    boole     bmon;          
    /** test-result */
    boolx     folu; 
    /** test-result */
    boolx     empty; 
    /** test-result */
    boolx     full; 
    /** test-result */
    boolx     starfree; 
    /** test-result */
    boolx     localtest; 
    /** test-result */
    boolx     definite; 
    /** test-result */
    boolx     revdefinite; 
    /** test-result */
    boolx     gendefinite; 
    /** test-result */
    boolx     dotdepth1; 
    /** test-result */
    boolx     piecetest; 
    /** test-result */
    boolx     nilpotent;
    /** localdegree 
     * if degree is unknown lmon->mno 
     */
    posint    localdegree;    
} ;

/** pointer to a regular language 
 *  @ingroup LANGUAGE
 */
typedef struct regset *language;

/** allocates a new language
 *  @ingroup LANGUAGE
 */
language newlang() ;

/** frees the mem used by a regset
 *  @ingroup LANGUAGE
 */
void     freelang(language lan) ;

#endif
