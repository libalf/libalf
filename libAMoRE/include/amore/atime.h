/** \file
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


/** \defgroup CLOCK Clock  (functions to measure computation time)
 *  \ingroup MISC
 *  @{
 */
#ifndef _ATIME_H
#define _ATIME_H

#ifdef __cplusplus
extern "C" {
#endif

/** clock returns how much time has passed.
 *  return time passed since _start to last _start
 */
double clock_getTime();

/** clock: resets clock and starts it
*/
void clock_start();

#ifdef __cplusplus
} // extern "C"
#endif

//@}
#endif
