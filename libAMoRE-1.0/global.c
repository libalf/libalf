/*
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

/* global.c
 * error handling, unconditional jumps...
 */

#include "global.h"
#include "buffer.h"

void OUT_OF_MEM() {
    buffree(); freebuf(); longjmp(_jmp,EXCEPTION_OUT_OF_MEMORY);
}

mrkfin newfinal(posint a) { 
    mrkfin hlp;
    hlp = (mrkfin)calloc(a+1, sizeof(boolx));
    if (hlp == NULL) {
#ifdef DEBUG
	perror("newfinal 1"); 
#endif
	OUT_OF_MEM();
    }
    return hlp;
} /* newfinal */

array newar(posint a) { 
  array hlp;
  hlp = (array)calloc(a, sizeof(posint));
  if (hlp == NULL) {
#ifdef DEBUG
    perror("newar 1"); 
#endif
    OUT_OF_MEM();
  }
  return hlp;
} /* newar */

boole *newbarray(posint a) { 
    boole *hlp;
    hlp = (boole *)calloc(a, sizeof(boole));
    if (hlp == NULL) {
#ifdef DEBUG
	perror("newbarray 1"); 
#endif
	OUT_OF_MEM();
    }
    return hlp;
} /* newbarray */

arrayofarray newarray1(posint a) { 
    arrayofarray hlp;
    hlp = (arrayofarray)calloc(a,sizeof(array));
    if (hlp == NULL) {
#ifdef DEBUG
	perror("newarray 1"); 
#endif
	OUT_OF_MEM();
    }
    return hlp;
} /* newarray1 */


char *pi2a(posint pi) {
    (void)sprintf(dummy,PIF,pi);
    return dummy;
} /* pi2a */


/** converts a positive integer to its string representation.
 */
string pi2s(posint n) {
  posint i = n;
  posint count = 1;
  string outstring;
  
  /* count digits */
  while (i > 9) { ++count; i /= 10; };

  outstring = ((string) newbuf(count + 1, sizeof(char)));
  i = n;
  while (count > 0) {
    outstring[count - 1] = (char) ('0' + (i % 10));
    i /= 10;
    --count;
  }
  return outstring;
} /* pi2s */
