/* \file
 * \brief functions for buffer
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

/** \defgroup BUFFER Buffer (memory for intermediate computation)
 *  \ingroup MISC
 * @{
 */

/** inits the buffer
 */void initbuf();


/* get num times size bytes from the buffer (if possible) 
 * \return pointer to the first element
 */
char *newbuf(posint num,posint size) ;

/** clear the buffer above the mark *
 */
void freebuf() ;

/** only used in genrexnf
 */
void bufmark() ;

/* delete the mark 
 */
void buffree() ;

//@}
