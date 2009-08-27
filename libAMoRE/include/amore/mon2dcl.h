/** \file
 * \brief computation of Green's relations.
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
#ifndef _MON2DCL
#define _MON2DCL

#include <amore/mon.h>

#ifdef __cplusplus
extern "C" {
#endif

/** appends dclassstructure of mon to mon (computes Green's relations).
 * see: J.E.Pin, Vari'et'es de langages formels
 *
 *  input: M transformation monoid
 *  output: decomposition of M with respect to Green's relations D,R,L,H
 *
 *  Definition:
 *                m D m'       iff       MmM = Mm'M
 *                m R m'       iff        mM = m'M
 *                m L m'       iff        Mm = Mm'
 *                m H m'       iff       m R m' and m L m'
 *
 *  m is a regular element iff there exists m' with: mm'm=m
 *  idempotent elements (mm=m) are regular!
 *
 *  a dclass is called regular iff one element (or equivalent all elements)
 *    in the dclass is regular
 *
 *  Fact: every regular dclass contains at least one idempotent element!
 *
 *  Example: rang,image,kernel of a transformation
 *  0 -> 2, 1  -> 0,  2  -> 1,  3  -> 3, 4  -> 0, 5  -> 1, 6 -> 0
 *
 *  rang:   4                      (size of the image)
 *  image:  0 1 2 3
 *  kernel: (0) (1 4 6) (2 5) (3)  (sorted partition of the set of states)
 *
 *  representation of this kernel:  0 7 1 4 6 7 2 5 7 3 7
 *     (7 is a seperator, sets are sorted with respect
 *      to the least element in the set)
 *
 *  an image is a transversale of a kernel if every set of the kernel
 *  contains exactly one element of the image
 *
 *  1 3 4 is     a transversale of (1 5) (2 4) (3)
 *           not a transversale of (1 2) (3 4) (5)
 *
 *  see cpt_rang, cpt_image, cpt_kernel
 *
 *
 *  Facts:
 *       if m L m' then m and m' have the same image
 *       if m R m' then m and m' have the same kernel
 *       if m D m' then m and m' have the same rang
 *
 *  for the computation of regular dclasses we use the following facts:
 *
 *  if m is regular, x,y are arbitrary elements  and
 *
 *  -mx has the same image as the transfomation of m,
 *   then m and mx belong to the same H-class
 *
 *  -the rang of ymx is equal to the rang of m, then either m and ymx
 *   belong to the same dclass or ymx is an irregular element
 *
 *  -if the image of mx is a transversale of the kernel of ym then
 *   mx and ym belong to the dclass of m
 *
 *  for arbitrary dclasses we use the facts:
 *
 *  -all h(l,r)classes of a fixed dclass have the same size
 *
 *  -if m and mx belong to different lclasses of one dclass,
 *   then  Lx = L' where L (L') is the lclass of m (resp. mx)
 *   (i.e. the mapping n -> nx is a bijektion from L onto L')
 *
 *  - if m and ym belong to different rclasses of one dclass,
 *    then  yR = R' where R (R') is the rclass of m (resp. ym)
 *
 *  - m and ym either belong to the same lclass or to different dclasses
 *  - m and mx either belong to the same rclass or to different dclasses
 *
 *  the algorithm:
 *    first step: computation of all regular dclasses
 *                search for idempotent elements and
 *                compute the dclass of these elements using image and kernel
 *   second step: computation of all irregular dclasses
 *                search for elements which are not already in a dclass
 *                compute the dclass of these elements
 *  \ingroup TRANSF_A_FROM_MONOID
 */
void mon2dcl(monoid mon);

/** test whether element*element=element
 * O(Q)
 *  \ingroup MISC_MONOID
 */
boole idempotent(posint element, monoid mon);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
