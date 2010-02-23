/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libmVCA.
 *
 * libmVCA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libmVCA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libmVCA.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <ostream>
#include <fstream>
#include <iterator>
#include <list>

#include <stdlib.h>

#include <libmVCA/mVCA.h>
#include <libmVCA/deterministic_mVCA.h>
#include <libmVCA/pushdown.h>
#include <libmVCA/p-automaton.h>

#include <amore++/finite_automaton.h>

using namespace std;
using namespace libmVCA;


void print_word(list<int> &word)
{{{
	printf(".");
	for(list<int>::iterator l = word.begin(); l != word.end(); l++)
		printf("%d.", *l);
}}}

pushdown_alphabet get_alphabet()
{{{
	pushdown_alphabet ret(3);
	ret.set_direction(0, DIR_UP);
	ret.set_direction(1, DIR_STAY);
	ret.set_direction(2, DIR_DOWN);
	return ret;
}}}

mVCA * get_aNbcN3()
{{{
	map<int, map<int, map<int, set<int> > > > transitions;

	pushdown_alphabet al = get_alphabet();

	set<int> final;
	final.insert(1);

	transitions[0][0][0].insert(0);

	transitions[1][0][0].insert(0);
	transitions[1][1][2].insert(1);

	transitions[2][0][0].insert(0);
	transitions[2][1][2].insert(1);

	transitions[3][0][0].insert(0);
	transitions[3][1][2].insert(1);
	transitions[3][0][1].insert(1);

	return construct_mVCA(/*state-count*/ 2, al, /*initial state*/ 0, final, /*m_bound*/ 3, transitions);
}}};

int main(int argc, char**argv)
{
	mVCA *aNbcN3;

	aNbcN3 = get_aNbcN3();

	bool f_is_deterministic;
	int f_alphabet_size, f_state_count;
	set<int> f_initial_states, f_final_states;
	multimap<pair<int,int>, int> f_transitions;

	aNbcN3->get_bounded_behaviour_graph(4, f_is_deterministic, f_alphabet_size, f_state_count, f_initial_states, f_final_states, f_transitions);

	amore::finite_automaton *a;
	a = amore::construct_amore_automaton(f_is_deterministic, f_alphabet_size, f_state_count, f_initial_states, f_final_states, f_transitions);
	cout << a->generate_dotfile();
}

