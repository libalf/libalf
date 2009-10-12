/* $Id$
 * vim: fdm=marker
 *
 * This file is part of liblangen (LANguageGENerator)
 *
 * liblangen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblangen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with liblangen.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 by David R. Piegdon, Chair of Computer Science 2 and 7, RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

/*
 * dfa_randomgenerator: equally distributedly generates random DFAs.
 * original idea of algorithm is described in
 * "Jean-Marc Champarnaud and Thomas Paranthoen - Random Generation of DFAs"
 */

#include <map>
#include <set>
#include <list>
#include <stack>

#include <gmpxx.h>

#include <liblangen/dfa_randomgenerator.h>
#include <liblangen/prng.h>


namespace liblangen {

using namespace std;
using namespace liblangen::prng;

dfa_randomgenerator::table::table(int m)
{ this->m = m; }

int dfa_randomgenerator::table::get_m()
{ return m; }

mpz_class & dfa_randomgenerator::table::getElement(mpz_class t, mpz_class p)
{{{
	static mpz_class zero;
	map<mpz_class, mpz_class>::iterator field;

	field = data[t].find(p);

	if(field == data[t].end()) {
		// field unknown. generate.
		if(t == 1) {
			data[t][p] = (p * (p+1)) / 2;
		} else {
			if(p*(m-1) < t)
				return zero;
			else
				data[t][p] = getElement(t,p-1) + p * getElement(t-1,p);
		}
	}

	return data[t][p];
}}}





dfa_randomgenerator::dfa_randomgenerator()
{{{
	seed_prng();
}}}

dfa_randomgenerator::~dfa_randomgenerator()
{{{
	discard_tables();
}}}

mpz_class & dfa_randomgenerator::elementOfC(int m, mpz_class t, mpz_class p)
// (where m is alphabet size)
{{{
	if(m < 2) {
		static mpz_class zero;
		return zero;
	}

	// check if any table for m is already loaded or try loading it.
	while((int)tables.size() <= m-1)
		tables.push_back((table*)NULL);

	if(tables[m-1] == NULL)
		tables[m-1] = new table(m);

	// return table element
	return tables[m-1]->getElement(t,p);
}}}

list<int> dfa_randomgenerator::randomElementOfK(int m, mpz_class t, mpz_class p)
{{{
	list<int> ret;

	if(m < 2)
		return ret;

	if(p*(m-1) < t)
		return ret;

	mpz_class C, De;

	C = elementOfC(m,t,p);

	if(t == 1) {
		mpz_class Dei, x;

		random_mpz_class(Dei, C);
		Dei += 1;
		De = Dei;
		x = 1;
		while(De > x) {
			De -= x;
			x++;
		};
		ret.push_back(x.get_si());
		return ret;
	} else {
		random_mpz_class(De, C);
		De += 1;
		if((De <= elementOfC(m,t,p-1)) && p > 1) {
			return randomElementOfK(m,t,p-1);
		} else {
			ret = randomElementOfK(m,t-1,p);
			ret.push_back(p.get_si());
			return ret;
		}
	}
}}}

void dfa_randomgenerator::discard_tables()
{{{
	while(!tables.empty()) {
		table * t = tables.back();
		tables.pop_back();
		if(t)
			delete t;
	}
}}}

bool dfa_randomgenerator::generate(int alphabet_size, int state_count, bool &t_is_dfa, int &t_alphabet_size, int &t_state_count, std::set<int> &t_initial, std::set<int> &t_final, multimap<pair<int,int>, int> &t_transitions)
{{{
	t_initial.clear();
	t_final.clear();
	t_transitions.clear();

	if(alphabet_size < 2)
		return false;
	if(state_count < 1)
		return false;

	list <int> K;
	K = randomElementOfK(alphabet_size, state_count*(alphabet_size-1), state_count);

	stack<int> internal;		// we're doing depth first. this stack keeps track of interior nodes
	stack<int> current_label;	// and the labels of these that are done
	int internal_done;		// the number of internal nodes that have so far been created
	int current_state;		// the current state number
	bool implicit_done = false;	// in K(m,t,p), the final (implicit) element is missing. here we keep track if we have created it.


	// note: all LEAFs of the extended m-ary tree of order m are not evaluated to states
	// of the final DFA, but to transitions of their predecessors.

	internal_done = 0;

	// add root node
	internal.push(0);
	current_label.push(0);
	internal_done++;

	current_state = 1;

	// tansform this element into a transition structure
	list<int>::iterator ni;
	ni = K.begin();
	while(!implicit_done || ni == K.end()) {
		if(ni != K.end()) {
			// if all children of current internal are done, pop it.
			while(current_label.top() >= alphabet_size) {
				internal.pop();
				current_label.pop();
			}
			while(*ni > internal_done) {
				// add transition between internal nodes
				pair<int, int> trid;
				trid.first = internal.top();
				trid.second = current_label.top();
				t_transitions.insert( pair<pair<int, int>, int>( trid, current_state) );

				// add new internal node
				current_label.top()++;
				internal.push(current_state);
				current_label.push(0);
				current_state++;
				internal_done++;
			}
		} else {
			implicit_done = true; // (below)
		}

		// if all children of current internal are done, pop it.
		while(current_label.top() >= alphabet_size) {
			internal.pop();
			current_label.pop();
		}

		// add leaf (i.e. create outgoing transition from predecessor)
		pair<int, int> trid;
		trid.first = internal.top();
		trid.second = current_label.top();
		t_transitions.insert( pair<pair<int, int>, int>( trid, random_int(current_state)) );

		current_label.top()++;

		ni++;
	}

	// pick random set of final states
	for(current_state = 0; current_state < state_count; ++current_state)
		if(random_int(2))
			t_final.insert(current_state);

	// construct and return resulting automaton
	t_is_dfa = true;
	t_initial.insert(0);	// initial state := root := state 0
	t_alphabet_size = alphabet_size;
	t_state_count = state_count;

	return true;
}}}


}; // end of namespace liblangen

