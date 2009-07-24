/* $Id: rpni_static.cpp 675 2009-07-23 18:10:06Z davidpiegdon $
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

	RPNI<bool> rumps(&knowledge, &log, alphabet_size);
	bool f_is_dfa;
	int f_alphabet_size, f_state_count;
	set<int> f_initial, f_final;
	multimap<pair<int, int>, int> f_transitions;

	if(!rumps.advance(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
		log(LOGGER_ERROR, "advance() returned false!\n");
	} else {
		snprintf(filename, 128, "hypothesis.dot");
		file.open(filename);

		file << automaton2dotfile(f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);

		file.close();
		printf("\n\nhypothesis saved.\n\n");
	}

	return 0;
}

