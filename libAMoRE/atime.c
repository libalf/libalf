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

/** atime.c
 *  Linux-specific
 */

#include <amore/atime.h>


#include <sys/types.h>
#include <sys/times.h>
#include <sys/timex.h>		/* ntp_gettime */
#include <sys/param.h>

#ifdef DEBUG
#include <stdio.h>
#endif

static struct ntptimeval t;

static double start;

double clock_getTime()
{
	double check;

	ntp_gettime(&t);

	check = (double) t.time.tv_sec + t.time.tv_usec / 1000000.0;
#ifdef DEBUG
	printf("getTime returns: %f\n", check - start);
#endif
	return check - start;
}

void clock_start()
{
	ntp_gettime(&t);

	start = (double) t.time.tv_sec + t.time.tv_usec / 1000000.0;

#ifdef DEBUG
	printf("clock started.. start %f !\n", start);
#endif
}
