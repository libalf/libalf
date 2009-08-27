/*! \file fileIO.h
 *  \brief contains functions to save or load a language
 *  Copyright (c) ?    - 2000 Lehrstuhl fuer Informatik VII, RWTH Aachen
 *  Copyright (c) 2000 - 2002 Burak Emir
 *  libAMoRE is  free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with the GNU C Library; if not, write to the Free
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307 USA.
 */

#ifndef _FILEIO_H
#define _FILEIO_H

#include <amore/language.h>

#ifdef __cplusplus
extern "C" {
#endif

/** saves a language.
 *  @ingroup FILE_IO
 *  \param lan the language to save
 *  \param workdir the directory (may be NULL)
 */
boolx writelan(language lan, char *workdir);


/** loads a language.
 *  @ingroup FILE_IO
 *  \param fname filename of the language to load
 *  \param workdir the directory (may be NULL)
 */
language rdinlan(char *fname, char *workdir);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
