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
#include <amore++/nondeterministic_finite_automaton.h>
#include <LanguageGenerator/DFArandomgenerator.h>
#include <LanguageGenerator/NFArandomgenerator.h>
#include <LanguageGenerator/regex_randomgenerator.h>

#include "fat.h"

using namespace LanguageGenerator;

bool get_input(finite_automaton *& automaton, input in, string gentype)
{
	basic_string<int32_t> str;
	basic_string<int32_t>::iterator si;

	string type, asize;
	size_t pos;
	int modelsize;

	bool f_is_dfa;
	int f_alphabet_size;
	int f_state_count;
	std::set<int> f_initial;
	std::set<int> f_final;
	multimap<pair<int,int>, int> f_transitions;

	switch(in) {
		case input_serial:
			if(!fd_to_basic_string(STDIN_FILENO, str))
				return false;

			automaton = new nondeterministic_finite_automaton;

			si = str.begin();
			if(!automaton->deserialize(si, str.end()))
				return false;
			return si == str.end();
		case input_human_readable:
			
			break;
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

			f_alphabet_size = atoi(asize.c_str());
			modelsize = atoi(gentype.c_str());

			if(f_alphabet_size < 1 || modelsize < 1) {
				cerr << "bad alphabet or modelsize for generator\n";
				return false;
			}

			// nfa|dfa|regex
			if(type == "regex") {
				regex_randomgenerator regng;
				string rex;
				bool ok;

				rex = regng.generate(f_alphabet_size, modelsize, 0.556, 0.278, 0.166);
				automaton = new nondeterministic_finite_automaton(rex.c_str(), ok);
				if(!ok) {
					cerr << "failed to generate NFA\n";
					return false;
				}
			} else {
				if(type == "dfa") {
					DFArandomgenerator dfarg;
					if(!dfarg.generate(f_alphabet_size, modelsize,
						f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
						cerr << "failed to generate DFA\n";
						return false;
					}
				} else {
					if(type == "nfa") {
						NFArandomgenerator nfarg;
						if(!nfarg.generate(f_alphabet_size, modelsize, 2, 0.5, 0.5,
							f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
							cerr << "failed to generate NFA\n";
							return false;
						}

					} else {
						cerr << "bad generator type \"" << type << "\"\n";
						return false;
					}
				}

				automaton = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
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

