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

#include <libalf/automaton_constructor.h>
#include <libalf/algorithm_biermann_minisat.h>

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<bool> knowledge;

	ofstream file;
	char filename[128];

	int alphabet_size;

	// create sample set in knowledgebase
#if 0
	{
		// for now, just add some samples...
		list<int> w;

		alphabet_size = 2;

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
		list<int> w;

		alphabet_size = 2;

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

	MiniSat_biermann<bool> diebels(&knowledge, &log, alphabet_size);

	basic_automaton_holder hypothesis;

	if(!diebels.advance(&hypothesis)) {
		printf("\n\n\nadvance returned false\n\n");
	} else {
//		diebels.print(cout);

		snprintf(filename, 128, "hypothesis.dot");
		file.open(filename);
		file << hypothesis.generate_dotfile();
		file.close();
		printf("\n\nhypothesis saved.\n\n");
	}

	return 0;
}

