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
{ this->m = m; changed = false; }

bool DFArandomgenerator::table::save(string path)
{
	
}

bool DFArandomgenerator::table::load(string path)
{
	
}

bool DFArandomgenerator::table::was_changed()
{ return changed; }

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
	flush_tables();
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

	if(tables[m-1] == NULL) {
		tables[m-1] = new table(m);
		tables[m-1]->load(table_path);
	}

	// return table element
	return tables[m-1]->getElement(t,p);
}}}

list<int> DFArandomgenerator::randomElementOfK(int m, mpz_class t, mpz_class p)
{{{
	list<int> ret;

	if(m < 2)
		return ret;

	if(p < (t/(m-1)))
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

bool DFArandomgenerator::flush_tables()
{{{
	bool success_saving = true;
	// check and save changed tables
	vector<table*>::iterator ti;
	for(ti = tables.begin(); ti != tables.end(); ti++)
		if(*ti && (*ti)->was_changed())
			if(!(*ti)->save(table_path))
				success_saving = false;
	discard_tables();
	return success_saving;
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

struct transition_constraint {
	int dst;
	int max_dst_depth;
};

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
{
	if(alphabet_size < 2)
		return false;
	if(state_count < 1)
		return false;

	list <int> K;
	K = randomElementOfK(alphabet_size, state_count*(alphabet_size-1), state_count);

	stack<int> internal;
	stack<int> current_label;
	int internal_done;
	int current_state;
	bool implicit_done = false;

	set<int> depth_members[state_count];
	list<transition_constraint> missing_transitions[state_count];

	internal_done = 0;

	// add root node
	internal.push(0);
	current_label.push(0);
	depth_members[0].insert(0);
	internal_done++;

	current_state = 1;

	// tansform this element into a transition structure
	list<int>::iterator ni;
/* {{{ cout << "K: (";
for(ni = K.begin(); ni != K.end(); ni++)
	cout << *ni << " ";
cout << ")\n"; }}} */
	ni = K.begin();
	while(!implicit_done || ni == K.end()) {
		transition_constraint tcon;
		if(ni != K.end()) {
			// if all children of current internal are done, pop it.
			while(current_label.top() >= alphabet_size) {
				internal.pop();
				current_label.pop();
			}
			while(*ni > internal_done) {
				// add new internal node and transition
				tcon.dst = current_state;
				tcon.max_dst_depth = -1;
				missing_transitions[internal.top()].push_back(tcon);

				depth_members[internal.size()].insert(current_state);
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

		// add leaf (i.e. mark outgoing transition as to-be-done)
		tcon.dst = -1;
		tcon.max_dst_depth = internal.size();
		missing_transitions[internal.top()].push_back(tcon);

		current_label.top()++;

		ni++;
	}

/* {{{ for(int depth = 0; depth < state_count; depth++) {
	cout << "depth " << depth << ": ";
	for(set<int>::iterator si = depth_members[depth].begin(); si != depth_members[depth].end(); si++)
		cout << *si << " ";
	cout << "\n";
}
cout << "\n";
for(current_state = 0; current_state < state_count; current_state++) {
	cout << "source: " << current_state << ":\n";
	for(list<transition_constraint>::iterator tci = missing_transitions[current_state].begin(); tci != missing_transitions[current_state].end(); tci++) {
		cout << "\t to " << tci->dst << " (max depth " << tci->max_dst_depth << ")\n";
	}
} }}} */

	// add missing transitions in a well-distributed way.

	// prepare arrays so we can do fast lookups
	int LEQ_than_depth[state_count+1]; // +1 because if all states are in a row, last may connect to depth state_count
	int depth_limited_state[state_count];
	current_state = 0;
	for(int i = 0; i < state_count; ++i) {
		for(set<int>::iterator si = depth_members[i].begin(); si != depth_members[i].end(); ++si, ++current_state)
			depth_limited_state[current_state] = *si;
		LEQ_than_depth[i] = (i==0?0:LEQ_than_depth[i-1]) + depth_members[i].size();
	}
	LEQ_than_depth[state_count] = LEQ_than_depth[state_count-1];

	// now to pick a random state of max depth >= n, pick a random element from
	// depth_limited_state[ 0 .. LEQ_than_depth[n] - 1 ]

/* {{{
cout << "\ndepth limited state:      ";
for(int i = 0; i < state_count; i++)
	cout << depth_limited_state[i] << " ";
cout << "\nless than or equal depth: ";
for(int i = 0; i <= state_count; i++)
	cout << LEQ_than_depth[i] << " ";
cout << "\n\n";
}}} */

	transition_set transitions;
	transition tr;

	for(current_state = 0; current_state < state_count; ++current_state) {
		transition tr;
		list<int> valid_labels;

		for(int i = 0; i < alphabet_size; ++i)
			valid_labels.push_back(i);
		tr.source = current_state;

		for(list<transition_constraint>::iterator tci = missing_transitions[current_state].begin(); tci != missing_transitions[current_state].end(); tci++) {
			// pick random label for this transition and remove it from the list of valid labels
			int l = my_rand(valid_labels.size() - 1);
			list<int>::iterator li = valid_labels.begin();
			while(l) { li++; l--; };
			tr.label = *li;
			valid_labels.erase(li);

			if(tci->dst >= 0) {
				tr.destination = tci->dst;
			} else {
				// pick random destination of valid depth
				tr.destination = depth_limited_state[ my_rand(LEQ_than_depth[tci->max_dst_depth]-1) ];
			}
			transitions.insert(tr);
		}
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
}


}; // end of namespace LanguageGenerator

