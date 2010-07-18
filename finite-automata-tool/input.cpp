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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
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

#include "amore_alf_glue.h"

#include "fat.h"

using namespace liblangen;

bool get_input(amore::finite_automaton *& automaton, input in, string gentype)
{
	basic_string<int32_t> str;
	basic_string<int32_t>::iterator si;

	string type, asize;
	size_t pos;
	int modelsize, alphabet_size;

	string input_string;
	int i;

	libalf::finite_automaton aut;
	set<int> final_states;
	serial_stretch serial;

	switch(in) {
		case input_serial:
			if(!fd_to_basic_string(STDIN_FILENO, str)) {
				cerr << "input of serialized automaton failed\n";
				return false;
			}

			automaton = new amore::nondeterministic_finite_automaton;

			serial.init(str);
			if(!automaton->deserialize(serial.current, serial.limit))
				return false;
			return serial.empty();
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

			automaton = amore_alf_glue::automaton_libalf2amore(aut);

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
				automaton = new amore::nondeterministic_finite_automaton(rex.c_str(), ok);
				if(!ok) {
					cerr << "failed to generate NFA\n";
					return false;
				}
			} else {
				if(type == "dfa") {
					dfa_randomgenerator dfarg;
					if(!dfarg.generate(alphabet_size, modelsize,
						aut.is_deterministic, aut.input_alphabet_size, aut.state_count, aut.initial_states, final_states, aut.transitions)) {
						cerr << "failed to generate DFA\n";
						return false;
					}
				} else {
					if(type == "nfa") {
						nfa_randomgenerator nfarg;
						if(!nfarg.generate(alphabet_size, modelsize, 2, 0.5, 0.5,
							aut.is_deterministic, aut.input_alphabet_size, aut.state_count, aut.initial_states, final_states, aut.transitions)) {
							cerr << "failed to generate NFA\n";
							return false;
						}
					} else {
						cerr << "bad generator type \"" << type << "\"\n";
						return false;
					}
				}

				automaton = construct_amore_automaton(aut.is_deterministic, aut.input_alphabet_size, aut.state_count, aut.initial_states, final_states, aut.transitions);
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

