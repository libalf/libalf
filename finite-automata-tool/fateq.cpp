/* $Id$
 * vim: fdm=marker
 *
 * Finite Automata Tools
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>

#include <getopt.h>

#include <amore++/nondeterministic_finite_automaton.h>
#include <libalf/alphabet.h>
#include <libalf/automaton.h>
#include <libalf/basic_string.h>

using namespace std;
using namespace amore;
using namespace libalf;

void usage()
{{{
	cerr << "Finite Automata Tool: equivalence tester for automata\n"
		"\n"
		"FAT Version "VERSION "\n"
		"\n"
		"Input options (via stdin):\n"
		"\t-1 or --first\n"
		"\t\t\tfilename of first automaton\n"
		"\t-2 or --second\n"
		"\t\t\tfilename of second automaton\n"
		"\t-h or --human_readable_input\n"
		"\t\t\tautomata are in human readable input\n"
		"\t\t\t(otherwise serial form is assumed)\n"
		"\n"
		"Output options (via stdout):\n"
		"\tdefault:\n"
		"\t-H or --human_readable_output\n"
		"\t\t\tgenerate human readable version of automaton\n"
		"\t\t\twrite discriminating word to stdout\n"
		"\t-d or --discriminator\n"
		"\t\t\twrite discriminating word to stdout (default)\n"
		"\t-D or --difference\n"
		"\t\t\twrite difference automaton to stdout\n"
		"\n";
}}}

int main(int argc, char**argv)
{

	const struct option server_long_options[] = {
		// { const char *name , int has_arg , int *flag, int val }
		// has_arg \in { required_argument, optional_argument, no_argument };
		{ "help",                   no_argument,            NULL, '?' },	// help

		{ "first",                  required_argument,      NULL, '1' },	// first automaton
		{ "second",                 required_argument,      NULL, '2' },	// second automaton
		{ "human_readable_input",   no_argument,            NULL, 'h' },	// read human-readable automaton
		{ "discriminator",          no_argument,            NULL, 'd' },	// give discriminating word between both automata
		{ "human_readable_output",  no_argument,            NULL, 'H' },	// write human-readable automaton to stdout
		{ "difference",             no_argument,            NULL, 'D' },	// create difference-automaton
		{ 0,0,0,0 }
	};

	bool human_readable_input = false;
	bool human_readable_output = false;
	bool give_difference = false;
	string firstfile, secondfile;

	int c;
	while(0 <= (c = getopt_long(argc, argv, "?1:2:hdHD", server_long_options, NULL)))
	{{{ // parse command line
		switch (c) {
			case '1':
				firstfile = optarg;
				break;
			case '2':
				secondfile = optarg;
				break;
			case 'h':
				human_readable_input = true;
				break;
			case 'd':
				if(give_difference){
					cerr << "invalid output constellation.\n\n";
					usage();
					return -1;
				}
				break;
			case 'H':
				human_readable_output = true;
				break;
			case 'D':
				give_difference = true;
				break;
			case '?':
				usage();
				return 0;
			default:
				cerr << "bad argument.\n\n";
				usage();
				return -1;
		}
	}}}

	finite_automaton *first, *second;

	// input

	if(human_readable_input)
	{{{
		string in;
		int i;

		bool f_is_dfa;
		int f_alphabet_size;
		int f_state_count;
		std::set<int> f_initial;
		std::set<int> f_final;
		multimap<pair<int,int>, int> f_transitions;

		ifstream f1(firstfile.c_str());
		while(!f1.eof())
			if( (i = f1.get()) >= 0 ) {
				in += (char)i;
			}
		f1.close();
		if(!read_automaton(in, f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
			cerr << "failed to read first automaton\n";
			return -1;
		}
		first = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(!first) {
			cerr << "failed to construct first automaton\n";
			return -1;
		}

		ifstream f2(secondfile.c_str());
		while(!f2.eof())
			if( (i = f2.get()) >= 0 )
				in += (char)i;
		f2.close();
		if(!read_automaton(in, f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
			cerr << "failed to read second automaton\n";
			return -2;
		}
		second = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(!second) {
			cerr << "failed to construct second automaton\n";
			return -2;
		}
	}}}
	else
	{{{
		basic_string<int32_t> serial;
		basic_string<int32_t>::iterator si;

		if(!file_to_basic_string(firstfile.c_str(), serial)) {
			cerr << "failed to read first serial\n";
			return -1;
		}
		first = new nondeterministic_finite_automaton;
		si = serial.begin();
		if(!first->deserialize(si, serial.end())) {
			cerr << "failed to deserialize first automaton\n";
			return -1;
		}
		if(si != serial.end())
			cerr << "garbage at end of first?\n";

		if(!file_to_basic_string(secondfile.c_str(), serial)) {
			cerr << "failed to read second serial\n";
			return -2;
		}
		second = new nondeterministic_finite_automaton;
		si = serial.begin();
		if(!second->deserialize(si, serial.end())) {
			cerr << "failed to deserialize second automaton\n";
			return -2;
		}
		if(si != serial.end())
			cerr << "garbage at end of second?\n";
	}}}

	finite_automaton *difference, *tmp;

	difference = first->lang_symmetric_difference(*second);
	tmp = difference->determinize();
	tmp->minimize();
	delete first;
	delete second;
	delete difference;
	difference = tmp;

	list<int> discriminator;
	bool empty;

	discriminator = difference->get_sample_word(empty);

	// output

	if(empty) {
		delete difference;
		return 0;
	}

	if(give_difference) {{{
		basic_string<int32_t> serial;
		basic_string<int32_t>::iterator si;
		serial = difference->serialize();

		if(human_readable_output) {
			bool f_is_dfa;
			int f_alphabet_size;
			int f_state_count;
			std::set<int> f_initial;
			std::set<int> f_final;
			multimap<pair<int,int>, int> f_transitions;

			si = serial.begin();
			deserialize_automaton(si, serial.end(), f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
			f_is_dfa = automaton_is_deterministic(f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
			cout << write_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		} else {
			basic_string_to_fd(serial, STDOUT_FILENO);
		}
	}}}
	else
	{{{
		if(human_readable_output) {
			cout << word2string(discriminator) << "\n";
		} else {
			basic_string<int32_t> serial;
			serial = serialize_word(discriminator);
			basic_string_to_fd(serial, STDOUT_FILENO);
		}
	}}}

	delete difference;

	return 1;
};

