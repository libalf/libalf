/** \file
 * \brief basic constants.
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

#define ALPHSIZE	26 /*!< size of latin alphabet */

/*! number of abbriviations in the definition of reg. expressions */
#define NABBR	12 
#define MAXLET  'L' /*< the character to NABBR (abbreviations) */
/*define NLANG	9   < number of languages */

#define DEFAULT_PLINES	66 /*< lines on the default printer device  */
#define DEFAULT_PCOLS	80 /*< columns on the default printer device*/

#ifndef NULL
#define NULL	0 /*!< nil value for pointer*/
#endif

/*! if monoid > TABLESWITCH the multtable will not be shown full */
#define TABLESWITCH 100

#define LINELEN 100   /*!< the length of lines in amr-files */
#define LINELEN1 (LINELEN+2)

/* system constants -------------------------------------------- */
#define SIZEOFBYTE	8  /*!< #bits in char */
#define PILEN		7 /*!< max. length of a posint or int  number */
 /*! print and scan format for posint in format-strings */
#define PIF		"%u" 
 /*! print and scan format for posint and int (inclusive a newline) */
#define PIFMT		"%u\n" 
 /*! print and scan format for posint and int (inclusive a newline) */
#define IFMT 		"%d\n" 

/* BACKSPACE: keyboard code for <BACKSPACE> 
 * PATHSTR: separator for pathnames
 */
#ifdef DOS				    /* Lu-Go */
#define BACKSPACE	'\b'		    /* Lu-Go */
#define PATHSTR 	"\\"		    /* Lu-Go */
#endif					    /* Lu-Go */

#ifdef UNIX				    /* Lu-Go */
#define BACKSPACE	(010)               /* LU-Go */
#define PATHSTR 	"/"                 /* Lu-Go */
#endif                                      /* Lu-Go */


/* the boolean values ------------------------------------------
 */
#ifndef TRUE
#define TRUE  (1) /*< boolean value */
#define FALSE (0) /*< boolean value */
#endif
 
/*! is a boolx value with meaning: not yet computed */
#define UN_KNOWN (2)
 /*! is a boolx value with meaning: stop the computation */
#define ISQUIT	 (2)

#define FINMASK	0x1 /*!< masks for final states */
#define INMASK	0x2 /*!< masks for initial states etc.*/
