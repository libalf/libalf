/** \file
 *  \brief functions to produce output for debugging
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
#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H


#include "global.h"
#include "nfa.h"
#include "dfa.h"
#include "regexp.h"
#include "mon.h"

/** \defgroup DEBUGP DebugPrint (procedures to print an LDO to stdout)
 *  \ingroup MISC
 *  @{
 */

/** print nfa to stdout 
 */
void debugPrintNfa( nfa n ) ;

/** print dfa to stdout 
 */
void debugPrintDfa( dfa d ) ;

/** print regexp to stdout 
 */
void debugPrintRegExp( regex r ) ;

/** print monoid to stdout 
 */
void debugPrintMonoid( monoid m );

//@}
#endif
