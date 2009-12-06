/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libAMoRE++
 *
 * libAMoRE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libAMoRE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libAMoRE++.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <ostream>
#include <fstream>
#include <iterator>
#include <list>
#include <string>

#include <stdlib.h>

#include <liblangen/nfa_randomgenerator.h>
#include <liblangen/prng.h>

#include <amore++/nondeterministic_finite_automaton.h>

#include <libalf/alphabet.h>
#include <libalf/basic_string.h>

using namespace std;
using namespace amore;
using namespace liblangen;

inline bool classic_superset_of(finite_automaton & superset, finite_automaton & subset, list<int> & counterexample)
{{{
	finite_automaton * difference = subset.lang_difference(superset);
	bool is_empty;

	counterexample = difference->get_sample_word(is_empty);

	delete difference;
	return is_empty;
}}}


void test_superset(finite_automaton & superset, finite_automaton & subset, int & bad_count)
{
	bool antichain_is_super;
	list<int> antichain_counterexample;

	bool classic_is_super;
	list<int> classic_counterexample;

	antichain_is_super = superset.antichain__is_superset_of(subset, antichain_counterexample);

	classic_is_super = classic_superset_of(superset, subset, classic_counterexample);

	if(antichain_is_super != classic_is_super) {
		char filename[128];
		ofstream file;
		basic_string<int32_t> serial;

		cout << " BAD(" << bad_count << ")\n";

		snprintf(filename, 128, "cex%04d-superset.dot", bad_count);
		file.open(filename); file << superset.generate_dotfile(); file.close();

		snprintf(filename, 128, "cex%04d-superset.ser", bad_count);
		serial = superset.serialize();
		libalf::basic_string_to_file(serial, filename);

		snprintf(filename, 128, "cex%04d-subset.dot", bad_count);
		file.open(filename); file << subset.generate_dotfile(); file.close();

		snprintf(filename, 128, "cex%04d-subset.ser", bad_count);
		serial = subset.serialize();
		libalf::basic_string_to_file(serial, filename);

		snprintf(filename, 128, "cex%04d-info", bad_count);
		file.open(filename);
		if(antichain_is_super) {
			file << "antichain says: true.\n";
		} else {
			file << "antichain says: false. counterexample: ";
			libalf::print_word(file, antichain_counterexample);
			file << "\n";
		}
		if(classic_is_super) {
			file << "classic says: true.\n";
		} else {
			file << "classic says: false. counterexample: ";
			libalf::print_word(file, classic_counterexample);
			file << "\n";
		}
		file.close();

		bad_count++;
	}
}

int main()
{{{
	nfa_randomgenerator rag;

	int alphabet_size, state_count, tr_per_state;
	float p_initial, p_final;

	int bad_count = 0;

	while(1) {
		// randomize parameters
		alphabet_size = prng::random_int(4) + 2;
		state_count = prng::random_int(3) + 2;
		tr_per_state = prng::random_int(5) + 1;
		p_initial = prng::random_float();
		p_final = prng::random_float();
		cout << "\nnew set with\n";
		cout << "\n\talphabet_size " << alphabet_size;
		cout << "\n\tstate_count   " << state_count;
		cout << "\n\ttr_per_state  " << tr_per_state;
		cout << "\n\tp_initial     " << p_initial;
		cout << "\n\tp_final       " << p_final;
		cout << "\n";

		for(int run = 0; run < 100; ++run) {
			int f_alphabet_size, f_state_count;
			bool f_is_dfa;
			set<int> f_initial, f_final;
			multimap<pair<int, int>, int> f_transitions;

			finite_automaton *nfa1, *nfa2;

			// generate NFA1
			if(!rag.generate(alphabet_size, state_count, tr_per_state, p_initial, p_final,
					f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
				cout << "generator 1 return false. bad parameters?\n";
				return 1;
			}
			nfa1 = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
			if(!nfa1) {
				cout << "failed to construct nfa1!\n";
				return 2;
			}

			// generate NFA2
			if(!rag.generate(alphabet_size, state_count, tr_per_state, p_initial, p_final,
					f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
				cout << "generator 2 return false. bad parameters?\n";
				return 1;
			}
			nfa2 = construct_amore_automaton(false, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
			if(!nfa2) {
				cout << "failed to construct nfa2!\n";
				return 2;
			}

			cout << "(";

			test_superset(*nfa1, *nfa2, bad_count);
			cout << "+";
			test_superset(*nfa2, *nfa1, bad_count);
			cout << "+)";

			delete nfa1;
			delete nfa2;
			fflush(stdout);
		}
	}
}}}

