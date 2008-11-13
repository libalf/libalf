/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>
#include <ostream>
#include <fstream>
#include <iterator>
#include <list>

#include "arpa/inet.h"

#include "libalf/alf.h"
#include "libalf/automata_amore.h"

#include <amore/vars.h>

using namespace std;
using namespace libalf;

int main()
{
		int automaton[] = {
			2, // alphabet size
			4, // state count
			2, // number of initial states
			0,
			1,
			2, // number of final states
			2,
			3,
			4, // number of transitions
			0,-1,2,
			2,0,2,
			1,1,3,
			3,1,3
		};
		nondeterministic_finite_amore_automaton *nfa;
		basic_string<int32_t> serial;
		set<int> states;
		ofstream file;
		ostream_iterator<int32_t> out(cout, ", ");

		for(unsigned int i = 0; i < sizeof(automaton)/sizeof(int); i++) {
			serial += htonl(automaton[i]);
		}

		nfa = new nondeterministic_finite_amore_automaton();
		if(!nfa->deserialize(serial)) {
			cout << "deserialization failed. check automaton.\n";
			return 1;
		}

		file.open("original-nfa.dot");
		file << nfa->generate_dotfile();
		file.close();

		states.insert(0);

		cout << "epsilon closure of states ";
		copy(states.begin(), states.end(), out);
		cout << ":\n";

		nfa->epsilon_closure(states);

		copy(states.begin(), states.end(), out);
		cout << "\n";

}

