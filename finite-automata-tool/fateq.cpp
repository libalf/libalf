/* $Id$
 * vim: fdm=marker
 *
 * This file is part of Finite Automata Tools (FAT).
 *
 * FAT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FAT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FAT.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>

#include <getopt.h>

#include <libalf/alf.h>
#include <amore++/amore.h>
#include <amore/global.h>

using namespace std;
using namespace amore;
using namespace libalf;

void usage()
{{{
	cerr << "\n"
		"Finite Automata Tool: equivalence tester for automata\n"
		"\n"
		"FAT version "VERSION "\n"
	     << libalf::libalf_version() << "\n"
	     << amore::libamorepp_version() << "\n"
	     << libamore_version() << "\n"
	     << "\n"
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
		"\t\t\toutput serial of discriminating word\n"
		"\t-d or --discriminator\n"
		"\t\t\twrite discriminating word to stdout (default)\n"
		"\t-D or --difference\n"
		"\t\t\twrite difference automaton to stdout\n"
		"\t-H or --human_readable_output\n"
		"\t\t\twrite human readable version of difference automaton\n"
		"\t\t\tor discriminating word to stdout\n"
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

	if(firstfile == "") {
		cerr << "please give name of first automaton\n";
		usage();
		return -1;
	}

	if(secondfile == "") {
		cerr << "please give name of second automaton\n";
		usage();
		return -1;
	}

	if(human_readable_input)
	{{{
		string in;
		int i;

		simple_automaton aut;

		ifstream f1(firstfile.c_str());
		while(!f1.eof())
			if( (i = f1.get()) >= 0 )
				in += (char)i;
		f1.close();
		if(!aut.read(in)) {
			cerr << "failed to read first automaton\n";
			return -1;
		}
		first = construct_amore_automaton(aut.is_deterministic, aut.alphabet_size, aut.state_count, aut.initial, aut.final, aut.transitions);
		if(!first) {
			cerr << "failed to construct first automaton\n";
			return -1;
		}

		aut.clear();
		in = "";

		ifstream f2(secondfile.c_str());
		while(!f2.eof())
			if( (i = f2.get()) >= 0 )
				in += (char)i;
		f2.close();
		if(!aut.read(in)) {
			cerr << "failed to read second automaton\n";
			return -2;
		}
		second = construct_amore_automaton(aut.is_deterministic, aut.alphabet_size, aut.state_count, aut.initial, aut.final, aut.transitions);
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
		cerr << "eq\n";
		delete difference;
		return 0;
	}

	if(give_difference) {{{
		basic_string<int32_t> serial;
		basic_string<int32_t>::iterator si;
		serial = difference->serialize();

		if(human_readable_output) {
			simple_automaton aut;

			si = serial.begin();
			aut.deserialize(si, serial.end());
			cout << aut.write();
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

	cerr << "neq\n";

	return 1;
};

