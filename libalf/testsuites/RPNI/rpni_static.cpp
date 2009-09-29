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

#include <libalf/algorithm_RPNI.h>
#include <libalf/automaton.h>

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
	{
		list<int> w;

		alphabet_size = 2;

		knowledge.add_knowledge(w, false); // epsilon
		w.push_back(0);
		knowledge.add_knowledge(w, false); // a
		w.push_back(1);
		knowledge.add_knowledge(w, false); // ab
		w.push_back(1);
		knowledge.add_knowledge(w, true); // abb (~ b)

		w.pop_back();
		w.pop_back();
		w.pop_back();
		w.push_back(1);
		knowledge.add_knowledge(w, true); // b
		w.push_back(0);
		knowledge.add_knowledge(w, false); // ba
		w.push_back(1);
		knowledge.add_knowledge(w, true); // bab (~ b)
		w.pop_back();
		w.pop_back();
		w.push_back(1);
		w.push_back(0);
		knowledge.add_knowledge(w, true); // bba (~ b)
	};

	cout << "\n";
	knowledge.print(cout);
	cout << "\n";

	RPNI<bool> rumps(&knowledge, &log, alphabet_size);
	conjecture *cj;

	if(!rumps.conjecture_ready()) {
		log(LOGGER_WARN, "RPNI says that no conjecture is ready! trying anyway...\n");
	}

	if( NULL == (cj = rumps.advance()) ) {
		log(LOGGER_ERROR, "advance() returned false!\n");
	} else {
		snprintf(filename, 128, "hypothesis.dot");
		file.open(filename);

		file << cj->visualize();

		file.close();
		printf("\n\nhypothesis saved.\n\n");
	}

	delete cj;
	return 0;
}

