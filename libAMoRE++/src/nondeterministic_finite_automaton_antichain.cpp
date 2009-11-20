/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libAMoRE++
 *
 * libAMoRE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libAMoRE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libAMoRE++.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */


// this file only contains antichain-related functions for the
// nondeterministic finite automaton.


// Antichain-based algorithms. See
//    M. De Wulf, L. Doyen, J.-F. Raskin
//    Antichains: A New Algorithm for Checking Universality of Finite Automata


#include <list>
#include <string>
#include <queue>
#include <set>

#include <stdio.h>
#include <ostream>
#include <fstream>

#include <unistd.h>
#include <stdlib.h>

# include "nondeterministic_finite_automaton_antichain.h"

# define LIBAMORE_LIBRARY_COMPILATION
# include "amore++/nondeterministic_finite_automaton.h"
# include "amore++/deterministic_finite_automaton.h"

namespace amore {

using namespace std;

/*
set<set<int> > nondeterministic_finite_automaton::antichain_universality_cpre(set<int> stateset)
{
	set<set<int> > ret;
	
	return ret;
}
bool nondeterministic_finite_automaton::antichain_universality_test(list<int> counterexample)
{
	
}
*/

bool nondeterministic_finite_automaton::antichain_equivalence_test(nondeterministic_finite_automaton &other, list<int> counterexample)
{{{
	if(!this->antichain_subset_test(other, counterexample))
		return false;
	return other.antichain_subset_test(*this, counterexample);
}}}


static inline void print_set(set<int> &s)
{{{
	set<int>::iterator si;
	for(si = s.begin(); si != s.end(); si++)
		printf("%d ", *si);
}}}
static inline void print_mm(multimap< int, set<int> > &mm)
{{{
	multimap< int, set<int> >::iterator mmi;
	for(mmi = mm.begin(); mmi != mm.end(); mmi++) {
		printf("\t%2d, { ", mmi->first);
		print_set(mmi->second);
		printf("}\n");
	}
}}}

multimap< int, set<int> > nondeterministic_finite_automaton::antichain_subset_cpre(multimap< int, set<int> > &stateset, nondeterministic_finite_automaton &other)
{{{
printf("cpre\n");
	multimap< int, set<int> > ret;

	multimap< int, set<int> >::iterator sti;	// stateset iterator -- this is (l',s')
	unsigned int sigma;				// label iterator

	for(sti = stateset.begin(); sti != stateset.end(); ++sti) {
		for(sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
			// get all allowed l
			set<int> l_prime, l;
			l_prime.insert(sti->first);
			l = this->predecessor_states(l_prime, sigma);

			// get all allowed s -- NOTE that the antichain-paper (see above) has a non-trivial definition of `` $ post_{sigma}^A(s) \subseteq T $ '' (1)
			//   post_{sigma}^A(s) \subseteq T   :=   \forall l \in s : \delta_A(l,sigma) \subseteq T
			// thus, (1) means that all states in s have to be a subset of the sigma-predecessor states of all elements of s'.
			// as we want the maximal subset, we just use the sigma-predecessor states of all elements of s'
			set< set<int> > s;
			// here, s_prime := sti->second
			for(set<int>::iterator spi = sti->second.begin(); spi != sti->second.end(); ++spi) {
				set<int> state;
				state.insert(*spi);
				s.insert( other.predecessor_states(state, sigma) );
			}
//			powerset_to_inclusion_antichain(s); // FIXME: does this perform better? it is not strictly necessary

			// build cartesian product ( l * s )
			set< set<int> >::iterator si;
			set<int>::iterator li;
			for(si = s.begin(); si != s.end(); ++si) {
				pair<int, set<int> > lXs;
				lXs.second = *si;
				for(li = l.begin(); li != l.end(); ++li) {
					lXs.first = *li;
					ret.insert(lXs);
				}
			}
		}
	}

	// afaik this is still required, even though we did powerset_to_inclusion_antichain(s) above.
	inner_powerset_to_inclusion_antichain(ret);

	return ret;
}}}

void nondeterministic_finite_automaton::antichain_subset_cpreN(pair<int, set<int> > &target, multimap< int, set<int> > &Fn, nondeterministic_finite_automaton &other)
{{{
	multimap< int, set<int> > Fnplus1;

	Fn.clear();
	Fn.insert(target);

printf("cpreN -------\n");

	Fnplus1 = antichain_subset_cpre(Fn, other);

int i = 1;
printf("F%d:\n", i);
print_mm(Fn);

	while( Fn != Fnplus1 ) {
		Fn = Fnplus1;
		Fnplus1.insert(target);
		i++;
		Fnplus1 = antichain_subset_cpre(Fnplus1, other);

printf("F%d:\n", i);
print_mm(Fnplus1);
if(i > 30)
	exit(-1);

	}

printf("\n");
}}}

bool nondeterministic_finite_automaton::antichain_subset_test(nondeterministic_finite_automaton &other, list<int> counterexample)
// check if L(this) is a subset of L(other)
{
	multimap< int, set<int> > Fn, single;

	pair<int, set<int> > target;

	set<int> our_final = this->get_final_states();
	set<int> our_initial = this->get_initial_states();
	set<int> other_nonfinal;
	set<int> other_final = other.get_final_states();

printf("this->final     { "); print_set(our_final); printf("}\n");
printf("this->initial   { "); print_set(our_initial); printf("}\n");
printf("other->final    { "); print_set(other_final); printf("}\n");

	for(unsigned int i = 0; i < other.get_state_count(); i++)
		other_nonfinal.insert(i);
	other_nonfinal = set_without(other_nonfinal, other_final);
	target.second = other_nonfinal;

printf("other->nonfinal { "); print_set(other_nonfinal); printf("}\n");

	for(set<int>::iterator si = our_final.begin(); si != our_final.end(); ++si) {
		target.first = *si;
		if(si == our_final.begin()) {
			// first run: initialize Fn
			antichain_subset_cpreN(target, Fn, other);
		} else {
printf("INTERSECT\n");
			// Nth run: intersect with former Fn
			antichain_subset_cpreN(target, single, other);
			Fn = inner_set_intersect(Fn, single);
		}
	}

	// this is not a subset of other iff there exists a
	// state I in this->initial s.t. (l, other->initial)
	// is in Fn
	multimap< int, set<int> > initial;
	target.second = other.get_initial_states();
	for(set<int>::iterator si = our_initial.begin(); si != our_initial.end(); ++si) {
		target.first = *si;
		initial.clear();
		initial.insert(target);
		if( inner_set_includes(Fn, initial) )
			return true;
	}


	// calculate matching counterexample
	

	return false;
}


}; // end of namespace amore.

