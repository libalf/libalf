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
#include <iterator>
#include <fstream>
#include <algorithm>

#include "libalf/alf.h"

#include <libalf/automata_amore.h>
#include <libalf/algorithm_biermann_minisat.h>

#include <amore/vars.h>
#include <amore/rexFromString.h>
#include <amore/rex2nfa.h>

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<ANSWERTYPE> knowledge;

	ofstream file;
	char filename[128];

	int alphabet_size;

	// init AMoRE buffers
	initbuf(); // XXX LEAK

	// create sample set in knowledgebase
#if 0
	{
		// for now, just add some samples...
		alphabet_size = 2;
		list<int> w;
		knowledge.add_knowledge(w, false);
		w.push_back(0);
		knowledge.add_knowledge(w, true);
		w.push_back(0);
		knowledge.add_knowledge(w, true);

		w.pop_back();
		w.pop_back();
		w.push_back(1);
		knowledge.add_knowledge(w, false);
		w.push_back(0);
		knowledge.add_knowledge(w, true);
		w.push_back(0);
		knowledge.add_knowledge(w, true);
	}
#else
	{
		alphabet_size = 2;
		list<int> w;
		w.push_back(0);
		knowledge.add_knowledge(w, false);
		w.push_back(1);
		knowledge.add_knowledge(w, true);

		w.pop_back();
		w.push_back(0);
		knowledge.add_knowledge(w, false);
		w.push_back(0);
		knowledge.add_knowledge(w, true);

		w.pop_back();
		w.pop_back();
		w.pop_back();
		w.push_back(1);
		knowledge.add_knowledge(w, false);
		w.push_back(1);
		knowledge.add_knowledge(w, false);
		w.push_back(1);
		knowledge.add_knowledge(w, true);
		w.push_back(0);
		w.push_back(1);
		knowledge.add_knowledge(w, true);
	};
#endif

	cout << "\n";
	knowledge.print(cout);
	cout << "\n";

	MiniSat_biermann<ANSWERTYPE> diebels(&knowledge, &log, alphabet_size);

	deterministic_finite_amore_automaton hypothesis;

	if(!diebels.advance(&hypothesis))
		printf("\nadvance returned false\n\n");

	printf("\n\n");
	diebels.print(cout);

	snprintf(filename, 128, "hypothesis.dot");
	file.open(filename);
	file << hypothesis.generate_dotfile();
	file.close();

	// release AMoRE buffers
	freebuf();

	return 0;
}

