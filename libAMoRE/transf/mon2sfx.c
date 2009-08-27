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

/* monsfx.c 3.0 (KIEL) 11.94 */

#include <amore/mon2sfx.h>

#include <amore/mon2dcl.h>

/* functions included:
 *      static void initmemory()
 *	static void elem2class()
 *	static void c2ccompute()
 *      static boole testc2c()
 *	static void d2dcomplete()
 *	static void d2dcompute()
 *	static void inversphi()
 *	starfexp void mon2sfx()
 */

/******************************************************************/
/******************************************************************/
static monoid stamon;
static starfexp stastarf;

static array melem2rang;	/* rang of an element */
static array melem2dno;		/* dclass of an element */
static array melem2rno;		/* rclass of an element */
static array melem2lno;		/* lclass of an element */
static array_of_int_array r2r;	/* used to test: not n in mM  */
static array_of_int_array l2l;	/* used to test: not n in Mm  */
static arrayofb_array d2d;	/* used to test: not n in MmM */

static array queue;		/* mark,queue used to compute transitive closure */
static b_array mark;		/* of relations r2r and l2l */
static array result;

/*  variables to avoid direct recursion in inversphi */
static array stak;		/* stack to manage the recursion */
static posint high;		/* number of elements on stak */

/******************************************************************/
/******************************************************************/

static void initmemory()
/* get memory */
{
	posint j, k;
	melem2rang = newarray_of_int(stamon->mno);
	melem2dno = newarray_of_int(stamon->mno);
	melem2rno = newarray_of_int(stamon->mno);
	melem2lno = newarray_of_int(stamon->mno);
	r2r = newarray_of_int_array(stamon->ds->rno);
	for (j = 0; j < stamon->ds->rno; j++) {
		r2r[j] = newarray_of_int(stamon->gno);
		for (k = 0; k < stamon->gno; k++)
			r2r[j][k] = stamon->mno;
	}
	l2l = newarray_of_int_array(stamon->ds->lno);
	for (j = 0; j < stamon->ds->lno; j++) {
		l2l[j] = newarray_of_int(stamon->gno);
		for (k = 0; k < stamon->gno; k++)
			l2l[j][k] = stamon->mno;
	}
	d2d = newarrayofb_array(stamon->ds->dno);
	for (j = 0; j < stamon->ds->dno; j++)
		d2d[j] = newb_array(stamon->ds->dno);
	j = ((stamon->ds->rno) > (stamon->ds->lno)) ? stamon->ds->rno : stamon->ds->lno;
	queue = newarray_of_int(j);
	mark = newb_array(j);
	stak = newarray_of_int(stamon->mno);
	high = 0;
	result = newarray_of_int(3 * stamon->mno);
	/* resident memory */
	stastarf = newsfexp();
	stastarf->u = newarray_of_int1(stamon->mno);
	stastarf->v = newarray_of_int1(stamon->mno);
	stastarf->w0 = newarray_of_int1(stamon->mno);
	stastarf->w1 = newarray_of_int1(stamon->mno);
	stastarf->ulength = newar(stamon->mno);
	stastarf->vlength = newar(stamon->mno);
	stastarf->w0length = newar(stamon->mno);
	stastarf->w1length = newar(stamon->mno);
	stastarf->computed = newbarray(stamon->mno);
}

/******************************************************************/
static void elem2class()
/* compute for every element an rclass, lclass and dclass-number */
{
	d_class dc;
	posint rno, lno, hno, help, help1, drun, rrun = 0, lrun = 0, rang;
	for (drun = 0; drun < stamon->ds->dno; drun++) {
		dc = stamon->ds->dclassarray[drun];
		rang = dc->rang;
		for (rno = 0; rno < dc->rno; rno++, rrun++)
			for (hno = 0; hno < dc->hsize; hno++) {
				help = mult(stamon, dc->rrep[rno], dc->hclass[hno]);
				for (lno = 0; lno < dc->lno; lno++) {
					help1 = mult(stamon, help, dc->lrep[lno]);
					melem2dno[help1] = drun;
					melem2rno[help1] = rrun;
					melem2lno[help1] = lrun + lno;
					melem2rang[help1] = rang;
				}
			}
		lrun += dc->lno;
	}
}

/******************************************************************/
static void c2ccompute()
/*  prepare test n notin mM for all n,m such that rang(n)=rang(m)
 *
 *  n in mM  depends only on the rclasses of n and m:
 *  n in mM  and m R m' => n in m'M  (because mM = m'M)
 *  n in mM  and n R n' => n' in mM 
 *   proof: n in mM <=> nM subset mMM=mM 
 *                  <=> m'M subseteq mMM=mM
 *                  <=> n' in mM
 * 
 *  r2r[R][g] = R' if there exists a generator g s.t. Rg=R'
 *
 *  R rclass of m,  R' rclass of n ,  
 *  n in mM iff Rg1...gr =R'
 *
 *  testc2c(i,j,r2r) <=> (i,j) in transitive closure og r2r 
 *
 * similar for lclasses
 */
{
	posint element, newelement, gen, i, drun, cno, class, rang;
	posint rrun = 0, lrun = 0;	/* actual rclass,lclass */
	d_class dc;
	for (drun = 0; drun < stamon->ds->dno; drun++) {
		dc = stamon->ds->dclassarray[drun];
		rang = dc->rang;
		for (cno = 0; cno < dc->rno; cno++, rrun++) {
			element = mult(stamon, dc->rrep[cno], dc->org);	/* element of Rclass rrun */
			for (gen = 1; gen <= stamon->gno; gen++) {
				newelement = stamon->gensucc[element][gen];
				if(melem2rang[newelement] == rang) {
					class = melem2rno[newelement];
					/* search for class in r2r[rrun] */
					if(class != rrun) {
						for (i = 0; i < stamon->gno; i++)
							if(r2r[rrun][i] == stamon->mno) {	/* new successor */
								r2r[rrun][i] = class;
								break;
							} else if(r2r[rrun][i] == class)
								break;	/* old successor */
					}
				}
			}
		}
		for (cno = 0; cno < dc->lno; cno++, lrun++) {
			element = mult(stamon, dc->org, dc->lrep[cno]);	/*element of Lclass lrun */
			for (gen = 1; gen <= stamon->gno; gen++) {
				newelement = mult(stamon, gen, element);
				if(melem2rang[newelement] == rang) {
					class = melem2lno[newelement];
					/* search for class in l2l[lrun] */
					if(class != lrun) {
						for (i = 0; i < stamon->gno; i++)
							if(l2l[lrun][i] == stamon->mno) {	/* new successor */
								l2l[lrun][i] = class;
								break;
							} else if(l2l[lrun][i] == class)
								break;	/* old successor */
					}
				}
			}
		}
	}
}

/******************************************************************/
static boole testc2c(no1, no2, c2c)
/* n is in mM <=> testc2c(no1,no2,r2r) 
 *                    with no1 is rclass of n and no2 is the rclass of m
 * n is in Mm <=> testc2c(no1,no2,l2l) 
 *                    with no1 is lclass of n and no2 is the lclass of m
 * 
 * test: (no1,no2) in transitive closure of r2r, l2l
 *
 */
posint no1, no2;
array_of_int_array c2c;
{
	posint gno, run, counter, help, test;
	boole found = FALSE;
	if(no1 == no2)
		return (TRUE);
	/* init queue */
	run = 0;
	counter = 1;
	queue[0] = no1;
	mark[no1] = TRUE;
	while((run < counter) && (!found)) {
		help = queue[run];
		for (gno = 0; gno < stamon->gno; gno++) {
			test = c2c[help][gno];
			if(test == stamon->mno)
				break;	/* advance in queue */
			else if(test == no2) {	/* no2 found */
				found = TRUE;
				break;
			} else if(!mark[test]) {
				queue[counter++] = test;
				mark[test] = TRUE;
			}
		}
		run++;
	}
	for (run = 0; run < counter; run++)
		mark[queue[run]] = FALSE;	/* clear mark */
	return (found);
}

/******************************************************************/
/******************************************************************/
static void d2dcomplete(dno, dcomplete)
/* d2dcomplete computes the transitive closure of the direct
 * successor relation
 *
 * dcomplete[i] <=> the successors of dclass i are computed
 */
b_array dcomplete;
posint dno;
{
	posint i, j;
	if(!dcomplete[dno]) {
		dcomplete[dno] = TRUE;
		for (i = 1; i <= stamon->ds->dno; i++)
			if(d2d[dno][i]) {	/* i is a successor of dno */
				if(!dcomplete[i])
					d2dcomplete(i, dcomplete);
				for (j = 1; j <= stamon->ds->dno; j++)
					if(d2d[i][j] == 1)
						d2d[dno][j] = 1;
			}
	}
}

/******************************************************************/
static void d2dcompute()
/*  n in MmM  depends only on the dclasses of n and m
 *  n in MmM and MmM = Mm'M => n  in Mm'M
 *  n in MmM and MnM = Mn'M => n' in MmM
 *    proof: n in MmM <=> MnM  subseteq MMmMM=MmM 
 *                    <=> Mn'M subseteq MMmMM=MmM 
 *                    <=>  n'  in MmM
 * 
 *  n in MmM <=> there are x,y in M: n = y*m*x
 *           <=> the dclass of n is a successor of the dclass of m
 *  n in MmM <=> d2d[Dm,Dn]
 */
{
	posint mel, dno, help, i, gno1, gno2;
	b_array dcomplete = newb_array(stamon->ds->dno + 1);
	/* compute d2d for direct successors of each dclass */
	for (mel = 0; mel < stamon->mno; mel++) {
		dno = melem2dno[mel];
		for (gno1 = 1; gno1 <= stamon->gno; gno1++) {
			i = stamon->gensucc[mel][gno1];
			d2d[dno][melem2dno[i]] = TRUE;
			help = mult(stamon, gno1, mel);
			d2d[dno][melem2dno[help]] = TRUE;
			for (gno2 = 1; gno2 <= stamon->gno; gno2++) {
				i = stamon->gensucc[help][gno2];
				d2d[dno][melem2dno[i]] = TRUE;
			}
		}
	}
	/* complete d2d */
	for (i = 0; i < stamon->ds->dno; i++)
		if(!dcomplete[i])
			d2dcomplete(i, dcomplete);
	for (i = 0; i < stamon->ds->dno; i++)
		d2d[i][i] = TRUE;
}

/******************************************************************/
static void inversphi(m)
 /*
  * F :={(n,g)    in MxG   | na R m , not n in mM }
  * F`:={(n,g)    in MxG   | an L m , not n in Mm }
  * C :={(g,n,g') in GxMxG | m in MgnM, m in Mng'M, not m in Mgng'M }
  * D :={ g       in A     | not m in MgM }
  *
  * u           =   U    inversphi(n).g
  *              (n,g) in F
  *
  * v           =   U    g.inversphi(n)
  *              (n,g) in F`
  *
  * w           =   U    g.inversphi(n).g'    union      U      g
  *              (g,n,g') in C                          g in D
  *
  * inversphi(m)=((u.A* # A*.v) - A*.w.A*)
  *
  */
posint m;			/* no of an melement  */
{
	posint count = 0, gen1, gen2, n, help, help1, class, i;
	stastarf->computed[m] = TRUE;
	if(m == 0)
		return;		/* inversphi(1) computed in output */
	/* compute u */
	class = melem2rno[m];
	for (n = 0; n < stamon->mno; n++)
		if(melem2rang[n] >= melem2rang[m])
			for (gen1 = 1; gen1 <= stamon->gno; gen1++) {
				help = stamon->gensucc[n][gen1];	/* help=n*gen1 */
				if(melem2rno[help] == class)	/* n*gen1 in rclass of m */
					if((melem2rang[n] > melem2rang[m]) || (!(testc2c(class, melem2rno[n], r2r))))
						/* not n in m*M */
					{
						if(!(stastarf->computed[n])) {
							stak[high++] = n;
							stastarf->computed[n] = TRUE;
						}
						result[count++] = n;
						result[count++] = gen1;
					}
			}	/* end for gen1 */
	stastarf->u[m] = newar(count);
	for (i = 0; i < count; i++)
		stastarf->u[m][i] = result[i];
	stastarf->ulength[m] = count / 2;
	count = 0;

	/* compute v */
	class = melem2lno[m];
	for (n = 0; n < stamon->mno; n++)
		if(melem2rang[n] >= melem2rang[m])
			for (gen1 = 1; gen1 <= stamon->gno; gen1++) {
				help = mult(stamon, gen1, n);	/* help=gen1*n */
				if(melem2lno[help] == class)	/* gen*n in lclass of m */
					if((melem2rang[n] > melem2rang[m]) || (!(testc2c(class, melem2lno[n], l2l))))
						/* not n in M*m */
					{
						if(!(stastarf->computed[n])) {
							stak[high++] = n;
							stastarf->computed[n] = TRUE;
						}
						result[count++] = gen1;
						result[count++] = n;
					}
			}	/* end for gen1 */
	stastarf->v[m] = newar(count);
	for (i = 0; i < count; i++)
		stastarf->v[m][i] = result[i];
	stastarf->vlength[m] = count / 2;
	count = 0;

	/* compute w0 */
	class = melem2dno[m];
	for (n = 0; n < stamon->mno; n++)
		if(melem2rang[n] >= melem2rang[m])
			for (gen1 = 1; gen1 <= stamon->gno; gen1++) {
				help = mult(stamon, gen1, n);
				if(d2d[melem2dno[help]][class])	/* m in M*gen1*n*M */
					for (gen2 = 1; gen2 <= stamon->gno; gen2++) {
						help1 = stamon->gensucc[n][gen2];
						if(d2d[melem2dno[help1]][class]) {	/* m in M*n*gen2*M */
							help1 = stamon->gensucc[help][gen2];
							if(!d2d[melem2dno[help1]][class]) {	/* not m in M*gen1*n*gen2*M */
								if(!(stastarf->computed[n])) {
									stak[high++] = n;
									stastarf->computed[n] = TRUE;
								}
								result[count++] = gen1;
								result[count++] = n;
								result[count++] = gen2;
							}
						}
					}
			}	/* end for gen2 */
	stastarf->w0[m] = newar(count);
	for (i = 0; i < count; i++)
		stastarf->w0[m][i] = result[i];
	stastarf->w0length[m] = count / 3;
	count = 0;

	/* compute w1 */
	for (gen1 = 1; gen1 <= stamon->gno; gen1++)
		if(!d2d[melem2dno[gen1]][class])	/* not m in M*gen1*M */
			result[count++] = gen1;
	stastarf->w1length[m] = count;
	stastarf->w1[m] = newar(count);
	for (i = 0; i < count; i++)
		stastarf->w1[m][i] = result[i];
	/*tick(); */
}


/* mon2sfx computes a starfree expression for an aperiodic monoid
 *
 *  L = +   inversphi(m)
 *       m assigns to the initial state a final state
 */
starfexp mon2sfx(dfa indfa, monoid mon)
{
	posint i, help;
	stamon = mon;
	if(!mon->dclassiscomputed)
		mon2dcl(mon);
	initmemory();
	elem2class();
	c2ccompute();
	d2dcompute();
/* computation of all m which assign a final state to the initial state */
	help = indfa->init;	/* abbreviation */
	for (i = 0; i < stamon->mno; i++)
		if(indfa->final[stamon->no2trans[i][help]]) {
			stak[high++] = i;
			stastarf->computed[i] = TRUE;
		}
	while(high) {
		high--;
		inversphi(stak[high]);
	}
	freebuf();
	return (stastarf);
}
