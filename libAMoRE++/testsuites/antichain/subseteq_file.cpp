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
#include <amore++/deterministic_finite_automaton.h>
#include <amore++/serialize.h>

#include <libalf/alphabet.h>
#include <libalf/basic_string.h>

using namespace std;
using namespace amore;

int main(int argc, char**argv)
{
	int ret = -1;

	if(argc != 3) {
		cout << "please give <File1> <File2> as parameters. the program will check if "
			<< "automaton 1 is subset of or equal automaton 2"
			<<".\n";
		return -1;
	}

	finite_automaton *n1, *n2;

	// construct automata
	{{{
		  basic_string<int32_t> serial;
		  serial_stretch ser;

		  libalf::file_to_basic_string(argv[1], serial);
		  libalf::print_basic_string_2hl(serial, cout);
		  cout << "\n";
		  n1 = new nondeterministic_finite_automaton;
		  ser.init(serial);
		  if( ! n1->deserialize(ser.current, ser.limit)) {
			  cout << "failed to deserialize first automaton!\n";
			  return -1;
		  }

		  libalf::file_to_basic_string(argv[2], serial);
		  libalf::print_basic_string_2hl(serial, cout);
		  cout << "\n";
		  n2 = new nondeterministic_finite_automaton;
		  ser.init(serial);
		  if( ! n2->deserialize(ser.current, ser.limit)) {
			  cout << "failed to deserialize second automaton!\n";
			  return -1;
		  }
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

