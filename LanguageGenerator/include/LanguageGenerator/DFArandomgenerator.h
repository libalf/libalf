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

#include <list>

#include <gmpxx.h> // GNU Multiple Precision Arithmetic Library, C++ interface

#ifndef __LanguageGenerator_DFArandomgenerator_h__
# define __LanguageGenerator_DFArandomgenerator_h__

#include <LanguageGenerator/automaton_constructor.h>

namespace LanguageGenerator {

using namespace std;

class DFArandomgenerator {
	private:
		gmp_randstate_t grstate;
	protected:
		mpz_class elementOfC(mpz_class m, mpz_class t, mpz_class p);
		list<mpz_class> randomElementOfK(mpz_class m, mpz_class t, mpz_class p);

	public:
		DFArandomgenerator();
		bool generate(int alphabet_size, int state_count, LanguageGenerator::automaton_constructor & automaton);

};

}; // end of namespace LanguageGenerator

#endif

