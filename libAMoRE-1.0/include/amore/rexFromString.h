/*!\file rexFromString.h
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

#ifndef _REXFROMSTRING_H
#define _REXFROMSTRING_H

#include <amore/regexp.h>

#include <amore/parser_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** allocates regex, parses string
 *  \ingroup LDO_DECL
 *  \param sno size of alphabet
 *  \param str string containing the regexp
 *  \warning if symbol JAVA_EXCEPT is defined upon compilation, you can refer to a variable "extern PARSE_RESULT errorParseResult;"
 */
regex rexFromString(int sno, char *str);

#ifdef JAVAEXCEPT
/** the error message is stored in this local variable so a Java Exception
 *  can be thrown
 */
PARSE_RESULT errorParseResult;
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
