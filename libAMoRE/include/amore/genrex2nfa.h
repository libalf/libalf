/*!\file
 *  \brief contains functions to measure computation time.
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
#ifndef _GREX2NFA_H
#define _GREX2NFA_H

#include <amore/dfa.h>
#include <amore/parser_types.h>
#include <amore/nfa.h>
#include <amore/regexp.h>
#include <amore/regexp-infix.h>
#include <amore/dfamdfa.h>
#include <amore/nfa2dfa.h>
#include <amore/rex2nfa.h>

#ifdef __cplusplus
extern "C" {
#endif

/** GENREGEXP to NFA
 *  \ingroup TRANSF_A_FROM_REGEXP
 */
nfa genrex2nfa(regex r);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
