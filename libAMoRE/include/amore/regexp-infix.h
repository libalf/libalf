/** \file regexp-infix.h
 *  \brief contains regexp-infix
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

#ifndef INFIX_H
#define INFIX_H

#include <amore/regexp.h>

#ifdef __cplusplus
extern "C" {
#endif

/** produce a infix-expression of the postfix-expression re->exprex.
 * \ingroup MISC_REGEXP
 * \return the infix-string and the length of this string in reslen
 * @param re input is re->exprex
 * @param dc TRUE iff concatch should be displayed
 * @param reslen length of the result (out-parameter)
 */
char* infix(regex re, boole dc, posint * reslen);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
