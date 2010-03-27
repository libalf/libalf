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

#include "arpa/inet.h"

#include "amore++/nondeterministic_finite_automaton.h"
#include "amore++/serialize.h"

using namespace std;
using namespace amore;

void print_word(ostream &os, list<int> &word)
{{{
	ostream_iterator<int> out(os, ".");
	os << ".";
	copy(word.begin(), word.end(), out);
}}}

int main()
{
	int automaton[] = {
		26, // length of upcoming data
		2, // alphabet size
		6, // state count
		2, // number of initial states
		0,
		1,
		1, // number of final states
		4,
		6, // number of transitions
		0,0,5,
		1,-1,2,
		2,1,3,
		3,-1,4,
		3,1,4,
		5,0,3
	};
	nondeterministic_finite_automaton *nfa;
	basic_string<int32_t> serial;
	serial_stretch ser;
	set<int> states;
	ofstream file;
	ostream_iterator<int32_t> out(cout, ", ");

	for(unsigned int i = 0; i < sizeof(automaton)/sizeof(int); i++) {
		serial += htonl(automaton[i]);
	}

	nfa = new nondeterministic_finite_automaton();
	ser.init(serial);
	if( ! nfa->deserialize(ser.current, ser.limit) ) {
		cout << "deserialization failed. check automaton.\n";
		return 1;
	}

	file.open("original-nfa.dot");
	file << nfa->visualize();
	file.close();

	states.insert(1);

	cout << "epsilon closure of states {";
	copy(states.begin(), states.end(), out);
	cout << "}:\n\t{";

	nfa->epsilon_closure(states);

	copy(states.begin(), states.end(), out);
	cout << "}\n";


	list<int> sample_word;
	bool is_empty = false;

	sample_word = nfa->get_sample_word(is_empty);

	cout << "sample_word: ";
	print_word(cout, sample_word);
	cout << "\n";
	/*
	*/
}

