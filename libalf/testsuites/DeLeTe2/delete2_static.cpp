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

#include <libalf/algorithm_DeLeTe2.h>

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<bool> knowledge;

	ofstream file;
	char filename[128];

	int alphabet_size;

	{
		// sample-set example from page 285 of TSC 313 (2004),
		// F. Denis, A. Lemay, A. Terlutte: "Learning regular languages using RFSAs" [1]
		list<int> w;

		alphabet_size = 2;

		knowledge.add_knowledge(w, true);	// .
		w.push_back(0);
		knowledge.add_knowledge(w, false);	// .0.
		w.push_back(0);
		knowledge.add_knowledge(w, true);	// .0.0.
		w.push_back(1);
		knowledge.add_knowledge(w, false);	// .0.0.1.
	//
//		w.push_back(1);
//		knowledge.add_knowledge(w, true);	// .0.0.1.1.
	//
		w.pop_back();
		w.pop_back();
		w.push_back(1);
		knowledge.add_knowledge(w, false);	// .0.1.
		w.push_back(0);
		knowledge.add_knowledge(w, true);	// .0.1.0.
		w.clear();
		w.push_back(1);
		knowledge.add_knowledge(w, false);	// .1.
		w.push_back(0);
		knowledge.add_knowledge(w, true);	// .1.0.
		w.pop_back();
		w.push_back(1);
		knowledge.add_knowledge(w, true);	// .1.1.
	};

	cout << "\n";
	knowledge.print(cout);
	cout << "\n";

	DeLeTe2<bool> rm(&knowledge, &log, alphabet_size);
	conjecture *cj;

	if(!rm.conjecture_ready()) {
		log(LOGGER_WARN, "RPNI says that no conjecture is ready! trying anyway...\n");
	}

	if( NULL == (cj = rm.advance()) ) {
		log(LOGGER_ERROR, "advance() returned false!\n");
	} else {
//		rm.print(cout);
		snprintf(filename, 128, "hypothesis.dot");
		file.open(filename);

		file << cj->visualize();

		file.close();
		printf("\n\nhypothesis saved.\n\n");
	}

	delete cj;
	return 0;
}

