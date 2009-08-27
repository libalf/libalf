/*  \file
 *  \brief monoid to defining relations
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
#ifndef _MON2REL_H
#define _MON2REL_H

#include <amore/mon.h>

#ifdef __cplusplus
extern "C" {
#endif

/** compute defining relations of monoid mon.
 *  @ingroup TRANSF_A_FROM_MONOID
 *    i.e. equation of the form w1 = w2
 *    where w1 and w2 are representatives of the same monoidelelement
 *    and w1 is lexicographical greater than w2.
 * possible relations are stored in list relfirst.
 *   this list is computed in dfa2mon during the computation of the monoid.
 * there are many relations which follow from other relations.
 *   for example relations of the form uv.a = uw with v.a=w is also a relation.
 *   relations like this are deleted.
 * relations of the form zeroa=zero and azero=zero are deleted.
 * relations are ordered in tree.
 */
void mon2rel(monoid mon);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
