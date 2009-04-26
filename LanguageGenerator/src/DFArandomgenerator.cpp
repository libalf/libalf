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

#include <set>

#include <stdlib.h>

#include <gmp.h>
#include <gmpxx.h>

#include <LanguageGenerator/DFArandomgenerator.h>
#include <LanguageGenerator/automaton_constructor.h>

namespace LanguageGenerator {

using namespace std;

mpz_class DFArandomgenerator::elementOfC(mpz_class m, mpz_class t, mpz_class p)
{
	
}

list<mpz_class> DFArandomgenerator::randomElementOfK(mpz_class m, mpz_class t, mpz_class p)
{{{
	list<mpz_class> ret;

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
		if(De <= elementOfC(m,t,p-1) && p > 1) {
			return randomElementOfK(m,t,p-1);
		} else {
			ret = randomElementOfK(m,t-1,p);
			ret.push_back(p);
			return ret;
		}
	}
}}}

DFArandomgenerator::DFArandomgenerator()
{{{
	// init GMP random number generator state
	gmp_randinit_default(grstate);
	gmp_randseed_ui(grstate, random()); // FIXME: do i need to seed? if so, use a better seed.
}}}

bool DFArandomgenerator::generate(int alphabet_size, int state_count, LanguageGenerator::automaton_constructor & automaton)
{
	list <mpz_class> K;
	K = randomElementOfK(alphabet_size, state_count*(alphabet_size-1), state_count);

	// tansform this element into a transition structure
	

	// add/label transitions in a well-distributed way
	set<int> initial;
	set<int> final;
	transition_set transitions;
	

	// construct and return resulting automaton
	return automaton.construct(true, alphabet_size, state_count, initial, final, transitions);
}


}; // end of namespace LanguageGenerator

