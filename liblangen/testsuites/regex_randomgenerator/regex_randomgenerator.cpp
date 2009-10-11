/* $Id$
 * vim: fdm=marker
 *
 * This file is part of liblangen (LANguageGENerator)
 *
 * liblangen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblangen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with liblangen.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

/*
 * regex random generator: testsuite for regex random generator.
 */

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>

#include <amore++/nondeterministic_finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>
#include <amore++/finite_automaton.h>

#include <liblangen/regex_randomgenerator.h>

using namespace liblangen;
using namespace amore;
using namespace std;

int main(int argc, char**argv)
{
	int alphabet_size;
	int num_op;
	bool show_mregex = false;

	if((argc != 3 && argc != 4) || (argc == 4 && 0 != strcmp(argv[1], "-r")) ) {
		cout << "please give <alphabet size> and <operand count> as parameters,\n"
			"possibly prefixed by -r to see the regex derived from the mDFA\n";
		return 1;
	}

	if(argc == 4) {
		show_mregex = true;
		alphabet_size = atoi(argv[2]);
		num_op = atoi(argv[3]);
	} else {
		alphabet_size = atoi(argv[1]);
		num_op = atoi(argv[2]);
	}

	string regex;
	regex_randomgenerator rrg;

	float pcon, puni, pstar;
	pcon = 50;
	puni = 25;
	pstar = 13;

	regex = rrg.generate(alphabet_size, num_op, pcon, puni, pstar);

	if(regex == "") {
		cout << "empty regex\n";
		return 0;
	}

	bool success;
	nondeterministic_finite_automaton automaton(alphabet_size, regex.c_str(), success);

	if(!success) {
		cout << "failed to create automaton from regex! RegEx is: \"" << regex << "\"\n";
		return 1;
	}

	finite_automaton *dfa;
	dfa = automaton.determinize();
	dfa->minimize();

	printf("NFA has %3d, mDFA has %3d states.\n"
		"\t\tRegEx:  %s\n",
		automaton.get_state_count(), dfa->get_state_count(), regex.c_str());

	if(show_mregex) {
		nondeterministic_finite_automaton *nfa;

		nfa = dynamic_cast<nondeterministic_finite_automaton*>(dfa->nondeterminize());
		string minrex = nfa->to_regex();

		printf("\t\tmRegEx: %s\n", minrex.c_str());
		delete nfa;
	}

	delete dfa;

	return 0;
}

