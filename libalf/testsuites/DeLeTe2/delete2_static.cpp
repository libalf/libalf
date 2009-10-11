/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
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

