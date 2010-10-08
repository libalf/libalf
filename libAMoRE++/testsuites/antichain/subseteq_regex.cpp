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
#include <fstream>
#include <iterator>
#include <list>

#include <stdlib.h>

#include <amore++/nondeterministic_finite_automaton.h>

#include <libalf/alphabet.h>
#include <libalf/basic_string.h>

using namespace std;
using namespace amore;

int main(int argc, char**argv)
{
	int ret = -1;

	if(argc != 4) {
		cout << "please give <alphabet size> <Regex1> <Regex2> as parameters. the program will check if "
			<< "R1 is subset of or equal R2"
			<<".\n";
		return -1;
	}

	nondeterministic_finite_automaton *n1, *n2;

	// construct automata
	{{{
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

		ofstream file;
		file.open("r1.dot");
		file << n1->visualize(true);
		file.close();

		file.open("r2.dot");
		file << n2->visualize(true);
		file.close();
	}}}

	list<int> counterexample;
	if(n2->antichain__is_superset_of(*n1, counterexample)) {
		cout << "\nR1 is subset of or equal R2.\n";
		ret = 0;
	} else {
		cout << "\nR1 is NOT subset of or equal R2.\n";
		cout << "counterexample: ";
		libalf::print_word(cout, counterexample);
		cout << "\n";
		ret = 1;
	}

	delete n1;
	delete n2;

	return ret;
}

