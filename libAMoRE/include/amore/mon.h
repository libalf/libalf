/*!\file  mon.h
 * \brief declarations for monoids, d-class decompositions
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
#ifndef _MON_H
#define _MON_H

#include <amore/global.h>
#include <amore/dfa.h>

#ifdef __cplusplus
extern "C" {
#endif

/** D-class (Green's relations)
 */
struct dclass {
    /** element in the upper left hclass */
	posint org;
    /** #r-classes,#lclasses */
	posint rno, lno;
    /** #elements in a h-class */
	posint hsize;
    /** array of prefixes(suffixes)
     * of representatives of
     * each r(l)-class (0 to r(l)no-1)
     */
	array lrep, rrep;
    /** one full h-class (0 to hsize-1) */
	array hclass;
    /** true iff regular d-class */
	boole regular;
    /** rang of d-class */
	posint rang;
    /** max.\ length of an element */
	posint maxlen;
    /** height of dclass in partial order */
	posint hei;
};
/** pointer to a D-class */
typedef struct dclass *d_class;

/** array of D-classes */
typedef d_class *darray;

/** D-class decomposition */
struct d_struct {
    /** (0 to dno-1) */
	darray dclassarray;
    /** #dclasses */
	posint dno;
    /** #lclasses */
	posint lno;
    /** #rclasses */
	posint rno;
    /** height of d-class partial order */
	posint height;
    /** partialorder */
	arrayofarray partial;
    /** number of succ */
	array numberofsucc;
};

/** pointer to a D-class decomposition */
typedef struct d_struct *dstruct;

/** relation */
struct r_struct {
	posint rno; /**<no of relations*/
	array lside;/**< leftside(number of a monoidelement) (0 .. rno-1) */
	array rside;/**< rightside(number of a generator)    (0 .. rno-1) */

};
/** pointer to a relation */
typedef struct r_struct *rstruct;

/** monoid
 *  @ingroup LDO_DECL
 */
struct mono {
    /** #states */
	posint highest_state;
    /** #letters */
	posint alphabet_size;
    /** #elements */
	posint mno;
    /** #generators */
	posint gno;
    /** list of generators (0 to gno)*/
	array generator;
    /** let2gen[i]=j iff
     * j is the least letter with the
     * transformation of generator i
     */
	array let2gen;
    /** transf_ of elements(0 to mno-1, 0 to highest_state) */
	arrayofarray no2trans;
    /** gensucc[i][0]= predecessor of i
     *
     *  gensucc[i][gen]=i*gen (0 to mno-1,0gno)
     */
	arrayofarray gensucc;
    /** last letter of a shortest
     *  representation of a element (0 to mno-1)
     */
	array lastletter;
    /** length of the representation (0 to mno-1) */
	array no2length;
    /** number of the zero if one exists
     *
     *  stamon->mno if no zero exists
     */
	posint zero;
    /** syn.\ monoid = syn.\ semigroup */
	boole mequals;
    /** d-classes are computed */
	boole dclassiscomputed;
    /** dclass structure if computed */
	dstruct ds;
    /** no2rang is computed */
	boole relationcomputed;
    /** relation if computed */
	rstruct rs;
    /** free space used for the computation
     * of a representative with
     * lastletter and gensucc[0]
     */
	array word;
    /** free space for display of repr. */
	string repr;
};
/** pointer to a monoid
 *  @ingroup LDO_DECL
*/
typedef struct mono *monoid;

/** allocates a new monoid
 */
monoid newmon();
/** frees the memory used by the monoid mon
 */
void freemon(monoid mon);

/** frees the memory used by the D-class structure dcl
 *
 * i is the number of the used entries in dclassarray
 */
void freedcl(dstruct dcl, posint i);

/** allocates a new dclass
 */
d_class newdclass();

/** allorcates a new dstruct
 */
dstruct newdstruct();

/** allocates a new rstruct
 */
rstruct newrstruct();
/** allocates a new darray
 */
darray newdarray(posint a);

/** same as prword1((char **)NULL,FALSE,(posint *)NULL,no,mon,with,TRUE));
 */
char *prword(posint no, monoid mon, boole with);

/** returns the shortest representative of the element no in monoid mon
 *
 * if (copy) word contains a copy of the output
 *
 * in this case length is the size of word
 *
 * if (with) mark idempotent elements with a star
 *
 * if (zeroone) print 0 for mon->zero else the representative of the zero
 *
 *              print 1 for the identity else ""
 */
char *prword1(char **word, boole copy, posint * length, posint no, monoid mon, boole with, boole zeroone);

/** multiplication of 2 monoidelements a and b */

posint mult(monoid mon, posint a, posint b);

/** needed when loading... from file */
void comprestofmon(monoid mon, dfa indfa);

/** compute length of longest representative of monoid mon */
posint monmaxlen(monoid mon);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
