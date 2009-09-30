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

#include <string>
#include <iostream>

#include <unistd.h>

#include <libalf/basic_string.h>
#include <libalf/conjecture.h>

#include <amore++/nondeterministic_finite_automaton.h>

#include <liblangen/dfa_randomgenerator.h>
#include <liblangen/nfa_randomgenerator.h>
#include <liblangen/regex_randomgenerator.h>

#include "fat.h"

using namespace liblangen;

bool get_input(finite_automaton *& automaton, input in, string gentype)
{
	basic_string<int32_t> str;
	basic_string<int32_t>::iterator si;

	string type, asize;
	size_t pos;
	int modelsize, alphabet_size;

	string input_string;
	int i;

	simple_automaton aut;

	switch(in) {
		case input_serial:
			if(!fd_to_basic_string(STDIN_FILENO, str)) {
				cerr << "input of serialized automaton failed\n";
				return false;
			}

			automaton = new nondeterministic_finite_automaton;

			si = str.begin();
			if(!automaton->deserialize(si, str.end()))
				return false;
			return si == str.end();
		case input_human_readable:
			// get input
			while(!cin.eof())
				if( (i = cin.get()) >= 0 )
					input_string += (char)i;

			// transform input to automaton
			if(!aut.read(input_string)) {
				cerr << "input of human-readable automaton failed\n";
				return false;
			}

			automaton = construct_amore_automaton(aut.is_deterministic, aut.alphabet_size, aut.state_count, aut.initial, aut.final, aut.transitions);

			if(!automaton) {
				cerr << "construct of human-readable automaton failed\n";
				return false;
			}

			return true;
		case input_generate:
			pos = gentype.find_first_of(':');
			if(pos != string::npos) {
				type = gentype.substr(0, pos);
				gentype = gentype.substr(pos+1, string::npos);
			} else {
				cerr << "generator missing model size parameter\n";
				return false;
			};
			pos = gentype.find_first_of(':');
			if(pos != string::npos) {
				asize = gentype.substr(0, pos);
				gentype = gentype.substr(pos+1, string::npos);
			} else {
				cerr << "generator missing alphabet size parameter\n";
				return false;
			};

			alphabet_size = atoi(asize.c_str());
			modelsize = atoi(gentype.c_str());

			if(alphabet_size < 1 || modelsize < 1) {
				cerr << "bad alphabet or modelsize for generator\n";
				return false;
			}

			// nfa|dfa|regex
			if(type == "regex") {
				regex_randomgenerator regng;
				string rex;
				bool ok;

				rex = regng.generate(alphabet_size, modelsize, 0.556, 0.278, 0.166);
				automaton = new nondeterministic_finite_automaton(rex.c_str(), ok);
				if(!ok) {
					cerr << "failed to generate NFA\n";
					return false;
				}
			} else {
				if(type == "dfa") {
					dfa_randomgenerator dfarg;
					if(!dfarg.generate(alphabet_size, modelsize,
						aut.is_deterministic, aut.alphabet_size, aut.state_count, aut.initial, aut.final, aut.transitions)) {
						cerr << "failed to generate DFA\n";
						return false;
					}
				} else {
					if(type == "nfa") {
						nfa_randomgenerator nfarg;
						if(!nfarg.generate(alphabet_size, modelsize, 2, 0.5, 0.5,
							aut.is_deterministic, aut.alphabet_size, aut.state_count, aut.initial, aut.final, aut.transitions)) {
							cerr << "failed to generate NFA\n";
							return false;
						}

					} else {
						cerr << "bad generator type \"" << type << "\"\n";
						return false;
					}
				}

				automaton = construct_amore_automaton(aut.is_deterministic, aut.alphabet_size, aut.state_count, aut.initial, aut.final, aut.transitions);
			}
			if(automaton)
				return true;

			cerr << "failed to construct generated automaton\n";
			return false;
		default:
			cerr << "wrong\n";
	}

	return false;
}

