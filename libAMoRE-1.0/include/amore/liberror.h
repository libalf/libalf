/* \file liberror.h
 *  \brief displays messages for weird errors that should never happen
 *
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

#ifndef _LIBERROR_H
#define _LIBERROR_H

#include <stdio.h>		/* for perror() */

#ifdef __cplusplus
extern "C" {
#endif

#define errex(A,B)      {                      \
  (void)printf("\n\nERROR in function %s\n",B);\
  (void)exit(A);                               \
}

#define SWERR		100
#define FILERR		200
#define JMPERR		300
#define BOSERR		400
#define LOSERR		401

/** perror - replacement
 */
void liberror(const char *f, const char *s);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
