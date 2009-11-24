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

#include "libalf/alf.h"

#include <libalf/algorithm_DeLeTe2.h>
#include <libalf/alphabet.h>

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<bool> knowledge;

	ofstream file;
	char filename[128];

	int alphabet_size;

	if(argc != 2) {
		cout << "give filename of serialized knowledgebase as parameter.\n";
		return -1;
	}

	basic_string<int32_t> serialized;
	basic_string<int32_t>::iterator si;
	if(!file_to_basic_string(argv[1], serialized)) {
		log(LOGGER_ERROR, "failed to obtain basic_string from file \"%s\"!\n", argv[1]);
		return -1;
	}
	si = serialized.begin();
	if(!knowledge.deserialize(si, serialized.end())) {
		log(LOGGER_ERROR, "failed to load knowledgebase from file \"%s\"!\n", argv[1]);
		return -1;
	}
	if(si != serialized.end())
		log(LOGGER_WARN, "garbage at end of file?\n");

	cout << "\n";
	knowledge.print(cout);
	cout << "\n";

	alphabet_size = knowledge.get_largest_symbol();

	DeLeTe2<bool> rm(&knowledge, &log, alphabet_size);
	conjecture *cj;

	if(!rm.conjecture_ready()) {
		log(LOGGER_WARN, "DeLeTe2 says that no conjecture is ready! trying anyway...\n");
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

	return 0;
}

