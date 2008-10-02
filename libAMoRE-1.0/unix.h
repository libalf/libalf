/* \file unix.h 
 *  Copyright (c)        1999 yroos@lifl.fr 
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

/* std functions */
#ifdef LINUX
        void  exit();
        void *calloc();
        void  free();
//      int sprintf() ;
#endif
#ifdef BSD43
/* 	int  exit(); */
/* 	char *calloc(); */
/* 	int  free(); */
/*         int  setjmp(); */
#endif
#ifdef SYS5
/* 	void  exit(); */
/* 	void *calloc(); */
/* 	void  free(); */
/*         int  setjmp(); */
#endif
	int  ungetc();
	void longjmp();
	char *getenv();
	int  chmod();
	char *strcat();
	char *strcpy();
	char *strtok();
	FILE *fopen();
#ifdef DEBUG
	void  abort();
	void  perror();
#endif

