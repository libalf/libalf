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
 * (c) 2010 David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>

#include <libalf/basic_string.h>
#include <libalf/conjecture.h>
#include <libalf/alphabet.h>

using namespace std;
using namespace libalf;

simple_moore_machine * simple_moore_machine_from_file(const char * filename)
{{{
	basic_string<int32_t> serial;
	simple_moore_machine * m;

	if(!file_to_basic_string(filename, serial)) {
		cerr << "failed to load serial from file.\n";
		return NULL;
	};

	cout << "loaded serial (2hl): ";
	print_basic_string_2hl(serial, cout);
	cout << " ;\n\n";

	serial_stretch s(serial);
	m = new simple_moore_machine;
	if(!m->deserialize(s)) {
		cerr << "error: failed to deserialize.\n";
		delete m;
		return NULL;
	};

	if(!s.empty()) {
		cerr << "warning: garbage at end of file!\n";
	}

	return m;
}}}

int main(int argc, char**argv)
{
	if(argc != 2) {
		cerr << "please give name of automaton as sole parameter.\n";
		return -1;
	}

	simple_moore_machine * m;

	m = simple_moore_machine_from_file(argv[1]);

	if(!m) {
		cerr << "failed to obtain automaton from file \"" << argv[1] << "\".\n";
		return -1;
	}

	basic_string<int32_t> serial;

	serial = m->serialize();

	cout << "serial (2hl): ";
	print_basic_string_2hl(serial, cout);
	cout << " ;\n\n";

	cout << "visual:\n" << m->visualize() << "\n\n";

	cout << "human readable:\n" << m->write() << "\n\n";

	if(!m->calc_validity()) {
		cerr << "this automaton seems to be invalid.\n";
		delete m;
		return -1;
	} else {
		cout << "automaton is valid.\n";
	}

	cout << ( (m->calc_determinism()) ? "and deterministic.\n\n" : "and NONdeterministic.\n\n" );



	// check some acceptances
	list<int> word;

	while(word.size() < 4) {
		cout << "acceptance of " << word2string(word) << " : " << ( m->contains(word) ? '+' : '-' ) << "\n";
		inc_graded_lex(word, m->input_alphabet_size);
	}


	return 0;
}

