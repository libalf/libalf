/*
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
 * (c) 2008-2011 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

#include <list>

# define LIBAMORE_LIBRARY_COMPILATION
# include "amore++/deterministic_finite_automaton.h"
# include "amore++/monoid.h"

# include <amore/mon.h>
# include <amore/dfa2mon.h>
# include <amore/mon2dcl.h>
# include <amore/mon2rel.h>

namespace amore {

// implementation notes:
//
// libAMoRE is using '0' as epsilon, thus in amore, he alphabet is [1 .. size]
// and not [0 .. size-1]
//
// libalf uses (in construct) -1 to indicate an epsilon transition and
// uses [0 .. size-1] as the alphabet.

amore_monoid::amore_monoid() {
	monoid_p = NULL;
}

amore_monoid::amore_monoid(monoid m) {
	monoid_p = m;
}

amore_monoid::~amore_monoid() {
	if(monoid_p) {
		freemon(monoid_p);
		free(monoid_p);
	}
}

unsigned int amore_monoid::get_highest_state() {
	if(monoid_p) {
		return monoid_p->highest_state;
	} else {
		return 0;
	}
}

unsigned int amore_monoid::get_alphabet_size() {
	if(monoid_p) {
		return monoid_p->alphabet_size;
	} else {
		return 0;
	}
}

unsigned int amore_monoid::get_element_count() {
	if(monoid_p) {
		return monoid_p->mno;
	} else {
		return 0;
	}
}

unsigned int amore_monoid::get_generator_count() {
	if(monoid_p) {
		return monoid_p->gno;
	} else {
		return 0;
	}
}

unsigned int amore_monoid::get_zero() {
	if(monoid_p) {
		return monoid_p->zero;
	} else {
		return 0;
	}
}

bool amore_monoid::mequals() {
	if(monoid_p) {
		return monoid_p->mequals;
	} else {
		return false;
	}
}

bool amore_monoid::dclass_iscomputed() {
	if(monoid_p) {
		return monoid_p->dclassiscomputed;
	} else {
		return false;
	}
}

void amore_monoid::compute_dclass() {
	if(monoid_p) {
		mon2dcl(monoid_p);
	}
}

bool amore_monoid::relation_iscomputed() {
	if(monoid_p) {
		return monoid_p->relationcomputed;
	} else {
		return false;
	}
}

void amore_monoid::compute_relation() {
	if(monoid_p) {
		mon2rel(monoid_p);
	}
}

unsigned int amore_monoid::multiplicate(unsigned int a, unsigned int b) {
	if(monoid_p) {
		if(a < monoid_p->mno && b < monoid_p->mno) {
			return mult(monoid_p, a, b);
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

char * amore_monoid::get_representative(unsigned int no, bool with, bool zeroone) {
	if(monoid_p) {
		return prword1((char **) NULL, FALSE, (posint *) NULL, no, monoid_p, with, zeroone);
	} else {
		return NULL;
	}
}

/**
 * Daniel's hack
 */
std::list<int> amore_monoid::get_representative(unsigned int no) {

	std::list<int> rep;

	if(monoid_p) {
		if(no < monoid_p->mno) {

			// Perform some Voodoo copied and adapted from compwrod in mon.c
			unsigned int elem = no;
			unsigned int word[monoid_p->no2length[no]];
			for(unsigned int letter = monoid_p->no2length[no]; letter != 0;) {
				word[--letter] = monoid_p->lastletter[elem];
				elem = monoid_p->gensucc[elem][0];
			}

			// Now, create representative (copied from prword1 in mon.c)
			for (unsigned int l=0; l<monoid_p->no2length[no]; l++) {
				rep.push_back(monoid_p->generator[word[l]]-1);
			}
		}
	}
	
	return rep;
}

std::map<unsigned int, std::map<unsigned int, unsigned int> > amore_monoid::get_table() {

	std::map<unsigned int, std::map<unsigned int, unsigned int> > table;
	
	if(monoid_p) {
		for(unsigned int i=0; i<monoid_p->mno; i++) {
			for(unsigned int j=0; j<monoid_p->mno; j++) {
				table[i][j] = mult(monoid_p, i, j);
			}
		}
	}

	return table;
}

monoid amore_monoid::get_monoid() {
	return monoid_p;
}

}; // end namespace amore

