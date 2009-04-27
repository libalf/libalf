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
	unsigned long int seed;
	int ran;
	ran = open("/dev/urandom", O_RDONLY);
	read(ran, &seed, sizeof(seed));
	close(ran);

	cout << "seed: "<<seed<<"\n";

	// FIXME: make this dependent on $PREFIX
	table_path = "/usr/local/share/LanguageGenerator";

	// init GMP random number generator state
	gmp_randinit_default(grstate);
	gmp_randseed_ui(grstate, seed); // FIXME: do i need to seed? if so, use a better seed.
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

list<mpz_class> DFArandomgenerator::randomElementOfK(int m, mpz_class t, mpz_class p)
{{{
	list<mpz_class> ret;

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
		ret.push_back(x);
		return ret;
	} else {
		mpz_urandomm(De.get_mpz_t(), grstate, C.get_mpz_t());
		De += 1;
		if((De <= elementOfC(m,t,p-1)) && p > 1) {
			return randomElementOfK(m,t,p-1);
		} else {
			ret = randomElementOfK(m,t-1,p);
			ret.push_back(p);
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

bool DFArandomgenerator::generate(int alphabet_size, int state_count, LanguageGenerator::automaton_constructor & automaton)
{
	list <mpz_class> K;
	K = randomElementOfK(alphabet_size, state_count*(alphabet_size-1), state_count);

	// tansform this element into a transition structure
	

	// add leaf transitions in a well-distributed way
	
	set<int> initial;
	initial.insert(0);
	set<int> final;
	transition_set transitions;
	

	// construct and return resulting automaton
	return automaton.construct(true, alphabet_size, state_count, initial, final, transitions);
}


}; // end of namespace LanguageGenerator

