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

#include <iostream>
#include <ostream>
#include <iterator>
#include <list>

#include <stdlib.h>

#include "amore++/nondeterministic_finite_automaton.h"

using namespace std;
using namespace amore;

void print_word(ostream &os, list<int> &word)
{{{
	ostream_iterator<int> out(os, ".");
	os << ".";
	copy(word.begin(), word.end(), out);
}}}

int main(int argc, char**argv)
{
	if(argc != 4) {
		cout << "please give <alphabet size> <Regex1> <Regex2> as parameters. the program will check if "
#ifdef SUBSET
			<< "R1 is subset of or equal R2"
#else
			<< "R1 and R2 are equal"
#endif
			<<".\n";
		return -1;
	}

	nondeterministic_finite_automaton *n1, *n2;
	bool regex_ok;

	int alphabet_size = atoi(argv[1]);
	if(alphabet_size < 2) {
		cout << "invalid alphabet size.\n";
		return -1;
	}

	n1 = new nondeterministic_finite_automaton(alphabet_size, argv[2], regex_ok);
	if(!regex_ok) {
		cout << "first regex is invalid.\n";
		return -1;
	}
	n2 = new nondeterministic_finite_automaton(alphabet_size, argv[3], regex_ok);
	if(!regex_ok) {
		cout << "second regex is invalid.\n";
		return -1;
	}

	list<int> counterexample;
#ifdef SUBSET
	if(n1->antichain_subset_test(*n2, counterexample)) {
		cout << "R1 is subset of or equal R2.\n";
		return 0;
	} else {
		cout << "R1 is NOT subset of or equal R2.\n";
		return 1;
	}
#else
	if(n1->antichain_equivalence_test(*n2, counterexample)) {
		cout << "R1 and R2 are equal.\n";
		return 0;
	} else {
		cout << "R1 and R2 are NOT equal.\n";
		return 1;
	}
#endif

}

