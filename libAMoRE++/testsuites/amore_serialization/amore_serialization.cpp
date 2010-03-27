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
#include <fstream>
#include <algorithm>

#include "amore++/nondeterministic_finite_automaton.h"
#include "amore++/deterministic_finite_automaton.h"
#include "amore++/serialize.h"

using namespace std;
using namespace amore;

int main(int argc, char**argv)
{
	finite_automaton *nfa;
	finite_automaton *dfa;

	basic_string<int32_t> serialized;
	serial_stretch ser;
	ofstream file;

	bool success = false;

	bool regex_ok;
	if(argc == 3) {
		nfa = new nondeterministic_finite_automaton(atoi(argv[1]), argv[2], regex_ok);
	} else {
		if(argc == 2) {
			nfa = new nondeterministic_finite_automaton(argv[1], regex_ok);
		} else {
			cout << "either give a sole regex as parameter, or give <alphabet size> <regex>.\n\n";
			cout << "example regular expressions:\n";
			cout << "alphabet size, \"regex\":\n";
			cout << "2 '((a((aa)a))U(((bb))*((((bU(ab))U(bUa)))*)*))'\n";
			cout << "2 '(((bU((aa)U(aUb)))U(a(aUb)))U((aUa)(bb)))'\n";
			cout << "2 '(((aa)(a)*)(((a((b(b)*)(aUb)))((ba))*))*)'\n";
			cout << "3 '(cbb(ab(c)*))* U (a((cbb*) U a+b+bc)+)'\n";
			return 1;
		}
	}

	if(regex_ok) {
		printf("REGEX ok.\n");
	} else {
		printf("REGEX failed.\n");
		return 1;
	}

	file.open("original-nfa.dot");
	file << nfa->visualize();
	file.close();

	dfa = nfa->determinize();
	dfa->minimize();

	file.open("original-dfa.dot");
	file << dfa->visualize();
	file.close();




	// serialize and deserialize now
	serialized = nfa->serialize();
	ser.init(serialized);

	delete nfa;
	nfa = new nondeterministic_finite_automaton();
	if(! nfa->deserialize(ser.current, ser.limit)) {
		cout << "nfaa serialization failed: returned false.\n";
	} else {
		if(!ser.empty()) {
			cout << "nfaa serialization failed: not at end of blob.\n";
		} else {
			file.open("deserialized-nfa.dot");
			file << nfa->visualize();
			file.close();
		}
	}


	serialized = dfa->serialize();
	ser.init(serialized);

	delete dfa;
	dfa = new deterministic_finite_automaton();
	if(! dfa->deserialize(ser.current, ser.limit) ) {
		cout << "dfaa serialization failed: returned false.\n";
	} else {
		if(!ser.empty()) {
			cout << "dfaa serialization failed: not at end of blob.\n";
		} else {
			file.open("deserialized-dfa.dot");
			file << dfa->visualize();
			file.close();
		}
	}

	delete nfa;
	delete dfa;

	if(success)
		return 0;
	else
		return 2;
}

