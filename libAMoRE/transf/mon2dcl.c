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

/* mondcl.c 3.1 (KIEL) 11.94 */
/* modified by oma on 30.Mar.95 */
/* by be Sept 2001 */
#include <amore/mon2dcl.h>

/* functions included:
 *
 *      static void  initstatic()  
 *             boole idempotent()
 *	static void  cpt_rang()
 *      static void  cpt_image()
 *      static void  cpt_kernel()
 *	static short int searchimage()
 *      static short int imcompare()
 *	static short int searchkernel()
 *	static void  l_reg_class ()
 *	static void  r_h_reg_class ()
 *	static void  d_reg_class ()
 *	static void  r_irreg_class()
 *	static void  l_irreg_class()
 *	static void  d_irreg_class()
 *	static boole dc1gedc2()
 *	static void  orderdclass()
 *             void  mon2dcl()
 */

typedef struct dlist {
	d_class info;
	struct dlist *next;
} *d_list;

#define newdlist() (d_list)newbuf((posint)1,(posint)sizeof(struct dlist))

/* only for computation of irregular dclasses */
static array harray;
static array pred;
static array rlqueue;
static array sufarray, prearray;


/* only for computation of regular dclasses */
static posint maximage, maxkernel;	/* maximal number of stored images (kernels) */
static arrayofarray imagearray;	/* two-dimensional arrays , second dimension */
static arrayofarray kernelarray;	/* only up to maximage  (maxkernel) */
static posint imnumber, kernumber;	/* actual number of stored images (kernels)
					 * set in searchimage/ searchkernel  */
static posint goodkernel;	/* number of kernels that represent r-classes
				 * set in searchimage */

static monoid stamon;		/* actual monoid */
static d_list dfirst, dlast;	/* first,last dclass in a list of dclasses */
static posint currentrang;	/* rang of actual dclass */
static array countimage, countkernel;	/* for computation of image and kernel */
static b_array mark;		/* mark elements, used in computation of regular 
				 * and irregular dclasses in different ways */
static b_array isindclass;	/* elements whose dclass is computed */
static posint regcount, irregcount;	/* count (ir)regular elements */
static posint dcounter, rcounter, lcounter;	/* count number of computed d(r,l)classes */
static array no2rang;

static int letterlength;
static array suffix, prefix, h_class, r_class, l_class, hstack;	/* see comments in r_h_reg_class */
posint Q, K;			/* maximal size of images and kernels */

static void initstatic()
{
	suffix = newarray(stamon->mno + 1);
	prefix = newarray(stamon->mno + 1);
	h_class = newarray(stamon->mno + 1);
	hstack = newarray(stamon->mno + 1);
	r_class = h_class;	/* different names in computation of regular and irregular d_classes */
	l_class = hstack;
	kernelarray = newarrayofarray(stamon->mno);
	imagearray = newarrayofarray(stamon->mno);
	mark = newb_array(stamon->mno);
	no2rang = newarray(stamon->mno);
	isindclass = newb_array(stamon->mno);
	Q = stamon->highest_state + 1;
	K = 2 * Q;
	countimage = newarray(Q);
	countkernel = newarray(Q);
	stamon->ds = newdstruct();
	letterlength = (stamon->alphabet_size <= ALPHSIZE) ? 1 : (1 + strlen(pi2a(stamon->alphabet_size)));
	maximage = 0;
	maxkernel = 0;
	dcounter = 0;
	rcounter = 0;
	lcounter = 0;
	regcount = 0;
	irregcount = 0;
}


boole idempotent(posint element, monoid mon)
{
	posint i;
	array a = mon->no2trans[element];	/* abbreviation */
	for (i = 0; i <= mon->highest_state; i++)
		if(a[i] != a[a[i]])
			return (FALSE);
	return (TRUE);
}


static void cpt_rang(element)
/* cpt_rang computes the rang of element 
 * the result is stored in no2rang[element]
 * countimage remains unchanged 
 * O(Q)	
 */
posint element;
{
	posint rang = 0, j;
	array a = stamon->no2trans[element];
	for (j = 0; j < Q; j++)
		countimage[a[j]] = 1;
	for (j = 0; j < Q; j++)
		if(countimage[j]) {
			rang++;
			countimage[j] = 0;
		}
	no2rang[element] = rang;
}


/* cpt_image computes the rang of element and  
 *   the image if the rang equals currentrang
 * the results are stored in imagearray[imnumber]      : image
 *                           no2rang[element]          : rang
 * countimage remains unchanged 
 * O(Q) 
 */
static void cpt_image(element)
posint element;
{
	posint rang = 0, j;
	array a = stamon->no2trans[element];
	if((!no2rang[element]) || (no2rang[element] == currentrang)) {
		for (j = 0; j < Q; j++)
			countimage[a[j]] = 1;
		/* countimage[j]!=0 iff state j is an image */
		if(maximage == imnumber)
			imagearray[maximage++] = newarray(Q);
		for (j = 0; j < Q; j++)
			if(countimage[j]) {
				imagearray[imnumber][rang++] = j;
				countimage[j] = 0;
			}
		no2rang[element] = rang;
	}
}

/* cpt_kernel computes the rang of element and the kernel
 *   if the rang equals currentrang
 * the results are stored in kernelarray[kernumber]   : kernel
 *                                   no2rang[element] : rang
 * countimage remains unchanged 
 * O(Q)
 */
static void cpt_kernel(element)
posint element;
{
	posint rang = 0, j, start = 0, image;
	array a = stamon->no2trans[element];
	if((!no2rang[element]) || (no2rang[element] == currentrang)) {
		for (j = 0; j < Q; j++)
			if(countimage[a[j]])
				countimage[a[j]]++;
			else {
				rang++;
				countimage[a[j]] = 1;
			}
		no2rang[element] = rang;
		/* countimage[j]==k iff state j is the image of k states */
/* compute kernel
 * the sets of a kernel are separated by the number Q
 * the sets of a kernel are ordered with respect to the least element in a set
 * the representation of a kernel requires at most an array of length K
 */
		if(no2rang[element] == currentrang) {
			if(maxkernel == kernumber)
				kernelarray[maxkernel++] = newarray(K);
			for (j = 0; j < Q; j++) {
				image = a[j];
				if(countimage[image])
					/* j is the least state with this image 
					 * the states with this image start at position countkernel[image]
					 * start is the first free place in the kernel 
					 */
				{
					kernelarray[kernumber][start] = j;
					countkernel[image] = start + 1;	/* next free place */
					start += countimage[image];
					kernelarray[kernumber][start++] = Q;	/* seperator */
					countimage[image] = 0;
				} else {	/* countkernel[image] is already computed */

					kernelarray[kernumber][countkernel[image]] = j;
					countkernel[image]++;
				}
			}	/* end for */
		} else
			for (j = 0; j < Q; j++)
				countimage[j] = 0;
	}
}


/* searchkernel = TRUE iff same rang and new kernel 
 * mark element if same rang, but not same kernel as the origin 
 *
 * O(Q * kernumber)
 */
static boole searchkernel(element)
posint element;
{
	posint i, j;
	posint kernellength = Q + currentrang - 1;	/* Q states and currentrang-1 seperators */
	array a, b;
	cpt_kernel(element);
	if(no2rang[element] != currentrang)
		return (FALSE);
	b = kernelarray[kernumber];
	for (i = 0; i < kernumber; i++) {
		a = kernelarray[i];
		for (j = 0; (j < kernellength) && (a[j] == b[j]); j++);
		if(j == kernellength) {
			if(i)
				mark[element] = TRUE;
			return (FALSE);	/* kernels are equal */
		}
	}
	mark[element] = TRUE;
	return (TRUE);
}

/* find all the kernels of which Im(element) is a transversal.  Resort
   the kernelarray so that the "good" kernels (that allow such a
   transversal) are at the beginning.  Return TRUE iff Im(element) is
   a transversal of a kernel.*/
static boole transversales(element)
posint element;
{
	array a, b, c;
	boole notfound, transversale, good = FALSE;
	posint i, j, k, begin;
	a = imagearray[imnumber];

	for (i = 0; i < kernumber; i++) {
		b = kernelarray[i];
		begin = 0;
		transversale = TRUE;
		for (j = 0; (j < currentrang) && (transversale); j++)
			/* for all sets in the kernel ..
			 * begin is the first element of the set
			 */
		{
			notfound = TRUE;
			for (k = 0; (b[begin] != Q) && notfound && (k < currentrang);)
				if(b[begin] == a[k])
					notfound = FALSE;	/* intersection is not empty */
				else if(b[begin] < a[k])
					begin++;
				else
					k++;
			if(notfound)	/* intersection is empty, test next kernel */
				transversale = FALSE;
			else {	/* intersection is not empty, test next set */

				for (; b[begin] != Q; begin++);	/* search for seperator */
				begin++;
			}
		}		/* end for j */
		if(transversale) {	/* actual image is a transversale of kernel at position i */
			good = TRUE;
			if(goodkernel < i) {	/* new representative for l-class, 
						 * switch prefix at position i and goodkernel 
						 */
				c = kernelarray[goodkernel];
				kernelarray[goodkernel] = kernelarray[i];
				kernelarray[i] = c;
				j = prefix[goodkernel];
				prefix[goodkernel] = prefix[i];
				prefix[i] = j;
				goodkernel++;
			} else if(goodkernel == i)
				goodkernel++;
			else
				i = goodkernel - 1;	/* only test bad kernel */
		}
	}			/* end for i */
	return (good);
}

/* searchimage = 3 iff same image as the original element
 *             = 2 iff same rang, old image, is transversale of a kernel 
 *             = 1 iff same rang, new image, is transversale of a kernel
 *             = 0 iff different rang or image which is not a transversale of a kernel
 *
 * O(Q * imnumber + kernumber * currentrang * Q ) (search image + test transversale)
 */
static short int searchimage(element)
posint element;
{
	posint i, j;
	array a, b;
	/* posint begin; */
	cpt_image(element);
	if(no2rang[element] != currentrang)
		return (0);
	mark[element] = TRUE;
	a = imagearray[imnumber];
	for (i = 0; i < imnumber; i++) {
		b = imagearray[i];
		for (j = 0; (j < currentrang) && (a[j] == b[j]); j++);
		if(j == currentrang) {	/* images are equal */
			if(i == 0)
				return (3);
			else
				return (2);
		}
	}
	/* new image, test for transversale
	 *
	 * an image is a transversale of an kernel
	 * iff
	 * the intersection of each class of the kernel with the image
	 * is not equal to the empty set
	 */
	if(transversales(element))
		return (1);
	else
		return (0);
}


/* imcompare = 2 iff the images at position 0 and imnumer are equal
 *           = 1     image is in imagearray 
 *           = 0     different rang or image not in imagearray (irregular element)
 *
 * O(imnumber * currentrang)
 */
static short int imcompare(element)
posint element;
{
	posint i, j;
	array a, b;
	cpt_image(element);
	if(no2rang[element] != currentrang)
		return (0);
	mark[element] = TRUE;
	a = imagearray[imnumber];
	for (i = 0; i < imnumber; i++) {
		b = imagearray[i];
		for (j = 0; (j < currentrang) && (a[j] == b[j]); j++);
		if(j == currentrang) {	/* images are equal */
			if(i == 0)
				return (2);
			else
				return (1);
		}
	}
	return (0);
}


/* compute all kernels for elements y*no of the same rang as no
 * store only y in prefix, prefix is used as a queue
 * kernumber is the actual number of different kernels and
 *   also the first free place in prefix
 *
 * O(kernelnumber * gno * Q) 
 *   where kernumber is number of reachable kernels of the same rang
 */
static void l_reg_class(no)
posint no;
{
	posint actuel, preactuel;
	posint gen, test;
	posint nextnumber = 0;	/* next element to be tested */
	kernumber = 0;
	cpt_kernel(no);
	prefix[0] = 0;
	kernumber = 1;
	goodkernel = 1;
	while(nextnumber < kernumber) {
		preactuel = prefix[nextnumber++];
		actuel = mult(stamon, preactuel, no);
		for (gen = 1; gen <= stamon->gno; gen++) {
			test = mult(stamon, gen, actuel);
			if((!mark[test]) && (!isindclass[test]))
				if(searchkernel(test))	/* new kernel of the same rang */
					prefix[kernumber++] = mult(stamon, gen, preactuel);
		}		/* end for all generators */
	}
}

/* no has to be an idempotent element
 * r_h_reg_class computes
 *  - one representative no.w for every image with the same rang and where
 *      the image is a transversale of a kernel (computed in l_reg_class)
 *      this are the representatives for all h_classes in the r_class of no 
 *      we store only w in SUFFIX
 *
 *  - all elements no.w with the image of no 
 *      this are the members of the h_class of no
 *      we store only no.w in H_CLASS
 *
 *  hstack : successors of these elements are possible candidates for H-class of no
 *           used as a stack
 *
 *  suffix : used as a queue
 *
 *  if rg(u.w)=rg(u) but not u.w in Ru then u.w is an irregular element
 *
 *  mark[i] => i is irregular or element in a regular rclass which is already computed
 *
 */

static void r_h_reg_class(no, hlength)
posint no;
posint *hlength;		/* size of hclass */
{
	posint sufactuel, actuel, i;
	posint gen, test;
	posint tophstack = 0;	/* first free space in hstack  */
	posint tophclass = 1;	/* first free space in h_class */
	posint nextnumber = 0;
	imnumber = 0;
	cpt_image(no);
	transversales(no);	/* introduced by oma, 29.03.95 */
	/* without this command, the procedure malfunctions if the image of
	   the original element is the only transversal of more than one
	   kernel. */
	imnumber = 1;
	suffix[0] = 0;		/* identity */
	h_class[0] = no;
	while(nextnumber < imnumber) {
		sufactuel = suffix[nextnumber++];
		actuel = mult(stamon, no, sufactuel);
		for (gen = 1; gen <= stamon->gno; gen++) {	/* for all generators .. */
			test = stamon->gensucc[actuel][gen];
			if((!mark[test]) && (!isindclass[test])) {	/* if not yet tested .. */
				i = searchimage(test);
				if(i == 3) {	/* same image as no => new element in hclass */
					h_class[tophclass++] = test;
					hstack[tophstack++] = test;
				} else if(i == 2)	/* old image which is a transversale of a kernel 
							 * test belongs to rclass of no and
							 * successors of test can be elements of h_class 
							 */
					hstack[tophstack++] = test;
				else if(i == 1)	/* new image which is a transversale of a kernel */
					suffix[imnumber++] = stamon->gensucc[sufactuel][gen];
			}	/* end if !mark */
		}		/* end for all generators */
	}			/* end while */
	/* compute all elements of the hclass */
	while(tophstack) {
		actuel = hstack[--tophstack];
		for (gen = 1; gen <= stamon->gno; gen++) {
			test = stamon->gensucc[actuel][gen];
			if((!mark[test]) && (!isindclass[test])) {
				i = imcompare(test);
				if(i == 2) {	/* same image => new element in hclass */
					h_class[tophclass++] = test;
					hstack[tophstack++] = test;
				} else if(i == 1)	/* test belongs to r_class of no, 
							 * successors of test can be elements of hclass */
					hstack[tophstack++] = test;
			}
		}		/* end for */
	}			/* end while */
	*hlength = tophclass;
}


/* 
 * d_reg_class computes the dclass of no
 * no must be an regular element
 */
static void d_reg_class(no)
posint no;
{
	posint hsize;
	posint i, j, k, help, help1;
	posint maxlen = stamon->no2length[no];
	d_class dc = newdclass();
	if(!no2rang[no])
		cpt_rang(no);
	currentrang = no2rang[no];
	mark[no] = TRUE;
	l_reg_class(no);
	r_h_reg_class(no, &hsize);
	dc->org = no;
	dc->regular = TRUE;
	dc->rang = no2rang[no];
	dc->hsize = hsize;
	dc->rno = goodkernel;
	dc->lno = imnumber;
	dc->hclass = newar(hsize);
	dc->rrep = newar(goodkernel);
	dc->lrep = newar(imnumber);
	for (i = 0; i < hsize; i++)
		dc->hclass[i] = h_class[i];
	for (i = 0; i < imnumber; i++)
		dc->lrep[i] = suffix[i];
	for (i = 0; i < goodkernel; i++)
		dc->rrep[i] = prefix[i];
	/* compute all elements of this d_class */
	for (i = 0; i < dc->rno; i++)
		for (k = 0; k < hsize; k++) {
			help = mult(stamon, prefix[i], h_class[k]);
			for (j = 0; j < dc->lno; j++) {
				help1 = mult(stamon, help, suffix[j]);
				isindclass[help1] = TRUE;	/*tick(); */
				no2rang[help1] = currentrang;
				if(maxlen < stamon->no2length[help1])
					maxlen = stamon->no2length[help1];
			}
		}
	dc->maxlen = maxlen * letterlength;
	if(no) {		/* not the first dclass, append to list */
		dlast->next = newdlist();
		dlast = dlast->next;
	} else {		/* first dclass */

		dfirst = newdlist();
		dlast = dfirst;
	}
	dlast->info = dc;
	dcounter++;
	rcounter += dc->rno;
	lcounter += dc->lno;
	regcount += ((dc->hsize) * (dc->lno) * (dc->rno));
}


/*    m is in the rclass of no
 * <=>
 *    there exits x,y with no*x=m and no=x*m
 *
 * r_irreg_class computes a directed graph where the nodes are labeled
 * with elements and a node with label x becomes the successor
 * of the elements x*a for all generators a
 *
 *    an element m belongs to the rclass of no
 * <=>
 *    there is a cycle in the graph with m and no 
 *
 * mark is used for the detection of such cycles and new elements
 *
 *    mark[i] == 0   new element in the tree
 *            == 1   old element in the tree
 *            == 2   element in r_class
 *
 * rlqueue contains all elements no*x with the same rang as no such that 
 *     no*x does not belong to a dclass which is already computed  
 * count is size of rlqueue 
 *
 * rlqueue[i]=no*x => sufarray[i]=x 
 *
 * r_class contains all elements no*x in the rclass of no, 
 *    i.e. there exists y no*x*y = no 
 *
 * r_class[i]=no*x => suffix[i]=x
 * rcount is size of r_class 
 *
 * i and pred[i] are positions in rlqueue, e.g. there exists a generator g such
 *    that rlqueue[pred[i]]*g = rlqueue[i]
 */
static void r_irreg_class(no, rno)
posint no;
posint *rno;
{
	posint i, actual, help, gen;	/* number of elements in the tree */
	posint nextnumber = 0, count = 1;	/* positions in rlqueue */
	boole newfound = TRUE;
	posint rcount = 1;	/* rcount is first free space in suffix and r_class */
	r_class[0] = no;
	suffix[0] = 0;
	rlqueue[0] = no;
	sufarray[0] = 0;	/* initialize queue with no and suffix of no = epsilon */
	mark[no] = 2;
	pred[0] = 0;		/*  this value is not used */
	/* compute tree */
	while(nextnumber < count) {
		for (gen = 1; gen <= stamon->gno; gen++) {
			actual = stamon->gensucc[rlqueue[nextnumber]][gen];
			if(mark[actual] == 2)
				/* stack[nextnumber] and all predeccessors of this 
				 * element are elements in r_class*/
			{
				help = nextnumber;
				while(mark[rlqueue[help]] != 2) {
					mark[rlqueue[help]] = 2;
					r_class[rcount] = rlqueue[help];
					suffix[rcount++] = sufarray[help];
					help = pred[help];
				}
			} else if((mark[actual] != 1) && (!isindclass[actual]))
				/* actual not already tested and not in another d_class */
			{
				if(!no2rang[actual])
					cpt_rang(actual);
				if(no2rang[actual] == currentrang) {	/* actual must be inserted in the tree */
					mark[actual] = 1;
					pred[count] = nextnumber;
					sufarray[count] = stamon->gensucc[sufarray[nextnumber]][gen];
					rlqueue[count++] = actual;
				}
			}
		}		/* end for all gen */
		nextnumber++;
	}			/* end while */

/* all elements that are reachable from no are marked (1 or 2)
 * search for other cycles in the graph */
	while(newfound) {
		newfound = FALSE;
		for (i = 0; i < count; i++) {
			actual = rlqueue[i];
			if(mark[actual] == 1)
				for (gen = 1; gen <= stamon->gno; gen++)
					if(mark[stamon->gensucc[actual][gen]] == 2) {	/* new cycle */
						newfound = TRUE;
						help = i;
						while(mark[rlqueue[help]] != 2) {
							mark[rlqueue[help]] = 2;
							r_class[rcount] = rlqueue[help];
							suffix[rcount++] = sufarray[help];
							help = pred[help];
						}
					}
		}
	}
	*rno = rcount;
	/* clear mark with rlqueue */
	for (i = 0; i < count; i++)
		mark[rlqueue[i]] = FALSE;
}


/* for comments see r_irreg_class 
 * replace multiplication to the right by multiplication to the left 
 */
static void l_irreg_class(no, lno)
posint no;
posint *lno;
{
	posint i, actual, help, gen;
	posint nextnumber = 0, count = 1;
	boole newfound = TRUE;
	posint lcount = 1;
	mark[no] = 2;
	l_class[0] = no;
	prefix[0] = 0;
	rlqueue[0] = no;
	prearray[0] = 0;
	pred[0] = 0;
	while(nextnumber < count) {
		for (gen = 1; gen <= stamon->gno; gen++) {
			actual = mult(stamon, gen, rlqueue[nextnumber]);
			if(mark[actual] == 2) {
				help = nextnumber;
				while(mark[rlqueue[help]] != 2) {
					mark[rlqueue[help]] = 2;
					l_class[lcount] = rlqueue[help];
					prefix[lcount++] = prearray[help];
					help = pred[help];
				}
			} else if((mark[actual] != 1) && (!isindclass[actual])) {
				if(!no2rang[actual])
					cpt_rang(actual);
				if(no2rang[actual] == currentrang) {	/* actual must be inserted in the tree */
					pred[count] = nextnumber;
					mark[actual] = 1;
					prearray[count] = mult(stamon, gen, prearray[nextnumber]);
					rlqueue[count++] = actual;
				}
			}
		}
		nextnumber++;
	}
	while(newfound) {
		newfound = FALSE;
		for (i = 0; i < count; i++) {
			actual = rlqueue[i];
			if(mark[actual] == 1)
				for (gen = 1; gen <= stamon->gno; gen++)
					if(mark[mult(stamon, gen, actual)] == 2) {
						newfound = TRUE;
						help = i;
						while(mark[rlqueue[help]] != 2) {
							mark[rlqueue[help]] = 2;
							l_class[lcount] = rlqueue[help];
							prefix[lcount++] = prearray[help];
							help = pred[help];
						}
					}
		}
	}
	*lno = lcount;
	for (i = 0; i < count; i++)
		mark[rlqueue[i]] = FALSE;
}


/* computation of irregular dclasses
 *
 * in r_irreg_class:  compute all elements no*x such that there exists a y with 
 *                    no*x*y = no 
 *                    these elements build the rclass of no
 * in l_irreg_class:  same with x*no
 * 
 * the intersection of the two sets build the hclass of no
 *
 * then compute one representative for every l(r) class in this dclass
 *
 */
static void d_irreg_class(no)
posint no;
{
	posint rsize, lsize, hcount = 1, rcount = 0, lcount = 0;
	posint element;
	posint i, j, k, l, m;
	posint maxlen = stamon->no2length[no];
	d_class dc = newdclass();
	harray[0] = no;
	if(!no2rang[no])
		cpt_rang(no);
	currentrang = no2rang[no];
	r_irreg_class(no, &rsize);
	l_irreg_class(no, &lsize);
	/* compute hclass of no = intersection of rclass and lclass 
	 * O(rsize*lsize)
	 * could be faster if sorted before O(rsize*log(rsize)+lsize*log(lsize))
	 */
	for (i = 1; i < rsize; i++) {
		for (j = 1; j < lsize; j++) {
			if(r_class[i] == l_class[j]) {	/* member of h_class */
				harray[hcount++] = r_class[i];
				j = lsize;
			}
		}
	}
	dc->org = no;
	dc->regular = FALSE;
	dc->rang = no2rang[no];
	dc->hsize = hcount;
	dc->lno = rsize / hcount;
	dc->rno = lsize / hcount;
	dc->hclass = newar(dc->hsize);
	dc->lrep = newar(dc->lno);
	dc->rrep = newar(dc->rno);
	for (i = 0; i < hcount;) {
		element = harray[i];
		dc->hclass[i++] = element;
		isindclass[element] = TRUE;	/*tick(); */
		no2rang[element] = currentrang;
	}
	/* hclass is computed, use isindclass to compute one representative 
	 * of each hclass in r(l)class of no 
	 * if(isindclass[element] then element belongs to hclass 
	 * that is already computed
	 * O(rsize + lsize)
	 */
	lcount = 1;
	dc->lrep[0] = 0;
	for (i = 1; i < rsize; i++)
		if(!isindclass[r_class[i]]) {	/*    new h_class in this r_class 
						 * => new representive for l_class 
						 */
			dc->lrep[lcount++] = suffix[i];
			isindclass[r_class[i]] = TRUE;	/*tick(); */
			no2rang[r_class[i]] = currentrang;
			if(maxlen < stamon->no2length[r_class[i]])
				maxlen = stamon->no2length[r_class[i]];
			/* compute the other elements of this h_class */
			for (j = 1; j < hcount; j++) {
				k = mult(stamon, dc->hclass[j], suffix[i]);
				isindclass[k] = TRUE;	/*tick(); */
				no2rang[k] = currentrang;
				if(maxlen < stamon->no2length[k])
					maxlen = stamon->no2length[k];
			}
		}
	rcount = 1;
	dc->rrep[0] = 0;
	for (i = 1; i < lsize; i++)
		if(!isindclass[l_class[i]]) {
			dc->rrep[rcount++] = prefix[i];
			isindclass[l_class[i]] = TRUE;	/*tick(); */
			no2rang[l_class[i]] = currentrang;
			if(maxlen < stamon->no2length[l_class[i]])
				maxlen = stamon->no2length[l_class[i]];
			for (j = 1; j < hcount; j++) {
				k = mult(stamon, prefix[i], dc->hclass[j]);
				isindclass[k] = TRUE;	/*tick(); */
				no2rang[k] = currentrang;
				if(maxlen < stamon->no2length[k])
					maxlen = stamon->no2length[k];
			}
		}
	/* compute the other elements of this dclass 
	 * O(number of elements in the dclass) 
	 */
	for (j = 0; j < hcount; j++)
		for (i = 1; i < rcount; i++) {
			l = mult(stamon, dc->rrep[i], dc->hclass[j]);
			for (k = 1; k < lcount; k++) {
				m = mult(stamon, l, dc->lrep[k]);
				isindclass[m] = TRUE;	/*tick(); */
				no2rang[m] = currentrang;
				if(maxlen < stamon->no2length[m])
					maxlen = stamon->no2length[m];
			}
		}
	dc->maxlen = maxlen * letterlength;
	dcounter++;
	rcounter += dc->rno;
	lcounter += dc->lno;
	irregcount += ((dc->hsize) * (dc->lno) * (dc->rno));
	dlast->next = newdlist();
	dlast = dlast->next;
	dlast->info = dc;
}


/* defines total ordering on all dclasses
 * TRUE iff dc1>dc2
 * O(1)
 */
static boole dc1gedc2(index1, index2)
posint index1, index2;
{
	d_class dc1, dc2;
	dc1 = stamon->ds->dclassarray[index1];
	dc2 = stamon->ds->dclassarray[index2];
	if(dc1->rang > dc2->rang)
		return (TRUE);
	if(dc1->rang < dc2->rang)
		return (FALSE);
	if(dc1->regular && (!dc2->regular))
		return (TRUE);
	if((!dc1->regular) && dc2->regular)
		return (FALSE);
	if(dc1->rno < dc2->rno)
		return (TRUE);
	if(dc1->rno > dc2->rno)
		return (FALSE);
	if(dc1->lno < dc2->lno)
		return (TRUE);
	if(dc1->lno > dc2->lno)
		return (FALSE);
	if(dc1->hsize < dc2->hsize)
		return (TRUE);
	if(dc1->hsize > dc2->hsize)
		return (FALSE);
	if(dc1->org < dc2->org)
		return (TRUE);
	return (FALSE);
}


/* sort the dclasses with respect to dc1gedc2 
 * bubblesort O(dcounter^2), but usally the dclasses are already ordered
 */
static void orderdclass()
{
	posint i, j;
	d_class dc;
	boole cont = TRUE;	/* stop if no exchange in a run */
	/* read dclasses from list of dclasses */
	stamon->ds->dno = dcounter;
	stamon->ds->rno = rcounter;
	stamon->ds->lno = lcounter;
	stamon->ds->dclassarray = newdarray(dcounter);
	dlast = dfirst;
	for (i = 0; i < dcounter; i++) {
		stamon->ds->dclassarray[i] = dlast->info;
		dlast = dlast->next;
	}
	/* sort dclasses */
	for (i = dcounter - 1; i && cont; i--) {
		cont = FALSE;
		for (j = 0; j < i; j++)
			if(dc1gedc2(j + 1, j)) {	/* swap dclasses */
				cont = TRUE;
				dc = stamon->ds->dclassarray[j];
				stamon->ds->dclassarray[j] = stamon->ds->dclassarray[j + 1];
				stamon->ds->dclassarray[j + 1] = dc;
			}
	}
}


/* appends dclassstructure of mon to mon */
void mon2dcl(monoid mon)
{
	posint element, help;
	stamon = mon;
	initstatic();
	/* compute all regular dclasses */
	for (element = 0; (regcount < stamon->mno) && (element < stamon->mno); element++)
		if(!isindclass[element])
			if(idempotent(element, stamon))	/* new idempotent element */
				d_reg_class(element);
	/* compute all irregular dclasses */
	help = stamon->mno - regcount;
	if(help) {		/* help = number of irregular elements */
		rlqueue = newarray(help);
		harray = newarray(help);
		prearray = newarray(help);
		sufarray = prearray;
		pred = newarray(help);
		for (element = 0; element < stamon->mno;)
			mark[element++] = FALSE;
		/*    element is an irregular element <=> !isindclass[element] */
		element = 1;
		for (element = 1; irregcount < stamon->mno - regcount; element++)
			if(!isindclass[element])
				d_irreg_class(element);
	}
	orderdclass();
	mon->dclassiscomputed = TRUE;
	freebuf();
}
