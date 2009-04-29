/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * DFArandomgenerator: equally distributedly generates random DFAs.
 * original idea of algorithm is described in
 * "Jean-Marc Champarnaud and Thomas Paranthoen - Random Generation of DFAs"
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <map>
#include <set>
#include <list>
#include <vector>
#include <stack>

#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include <gmp.h>
#include <gmpxx.h>

#include <LanguageGenerator/DFArandomgenerator.h>
#include <LanguageGenerator/automaton_constructor.h>

namespace LanguageGenerator {

using namespace std;



DFArandomgenerator::table::table(int m)
{ this->m = m; }

int DFArandomgenerator::table::get_m()
{ return m; }

mpz_class & DFArandomgenerator::table::getElement(mpz_class t, mpz_class p)
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





DFArandomgenerator::DFArandomgenerator()
{{{
	// seed PRNG: init GMP random number generator state
	unsigned long int gmp_seed;
	unsigned int n_seed;
	int ran;

	ran = open("/dev/urandom", O_RDONLY);
	read(ran, &gmp_seed, sizeof(gmp_seed));
	read(ran, &n_seed, sizeof(n_seed));
	close(ran);

	gmp_randinit_default(grstate);
	gmp_randseed_ui(grstate, gmp_seed);

	srand(n_seed);

	// FIXME: make this dependent on $PREFIX
	table_path = "/usr/local/share/LanguageGenerator";
}}}

DFArandomgenerator::~DFArandomgenerator()
{{{
	discard_tables();
}}}

mpz_class & DFArandomgenerator::elementOfC(int m, mpz_class t, mpz_class p)
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

list<int> DFArandomgenerator::randomElementOfK(int m, mpz_class t, mpz_class p)
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

		mpz_urandomm(Dei.get_mpz_t(), grstate, C.get_mpz_t());
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
		mpz_urandomm(De.get_mpz_t(), grstate, C.get_mpz_t());
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

void DFArandomgenerator::discard_tables()
{{{
	while(!tables.empty()) {
		table * t = tables.back();
		tables.pop_back();
		if(t)
			delete t;
	}
}}}

void DFArandomgenerator::set_table_path(string path)
{{{
	table_path = path;
}}}

int my_rand(int limit)
// will return a random integer in [0,limit]
{{{
	float t = RAND_MAX;
	while(t == RAND_MAX)
		t = rand();
	t /= RAND_MAX;
	t *= limit+1;
	return (int)t;
}}}

bool DFArandomgenerator::generate(int alphabet_size, int state_count, LanguageGenerator::automaton_constructor & automaton)
{{{
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

	transition_set transitions;	// set of transitions in final DFA
	transition tr;

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
	// cout << "K: ("; for(ni = K.begin(); ni != K.end(); ni++) cout << *ni << " "; cout << ")\n";
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
				tr.source = internal.top();
				tr.label = current_label.top();
				tr.destination = current_state;
				transitions.insert(tr);

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
		tr.source = internal.top();
		tr.label = current_label.top();
		tr.destination = my_rand(current_state-1);
		transitions.insert(tr);

		current_label.top()++;

		ni++;
	}

	// pick random set of final states
	set<int> final;
	for(current_state = 0; current_state < state_count; ++current_state)
		if(my_rand(1))
			final.insert(current_state);

	// construct and return resulting automaton
	set<int> initial;
	initial.insert(0);	// initial state := root := state 0
	return automaton.construct(true, alphabet_size, state_count, initial, final, transitions);
}}}


}; // end of namespace LanguageGenerator

