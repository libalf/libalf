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
#include <list>
#include <vector>

#include <gmpxx.h> // GNU Multiple Precision Arithmetic Library, C++ interface

#ifndef __LanguageGenerator_DFArandomgenerator_h__
# define __LanguageGenerator_DFArandomgenerator_h__

#include <LanguageGenerator/automaton_constructor.h>

namespace LanguageGenerator {

using namespace std;

class DFArandomgenerator {
	private:
		class table {
			private:
				map<mpz_class, map<mpz_class, mpz_class> > data;
				bool changed;
				int m;
				mpz_class t, p;
			public:
				table(int m);

				bool save(string path);
				bool load(string path);
				bool was_changed();
				int get_m();
				mpz_class & getElement(mpz_class t, mpz_class p);
		};
	private:
		gmp_randstate_t grstate;
		vector<table*> tables;
		string table_path;
	protected:
		mpz_class & elementOfC(int m, mpz_class t, mpz_class p);

	public:
		DFArandomgenerator();
		~DFArandomgenerator();

		// set location for storage of generated tables.
		void set_table_path(string path);

		// flush all tables currently in memory. we will try to save
		// them to <table_path> if they have been changed.
		// returns false if unable to write tables to <table_path>
		bool flush_tables();
		// does the same as flush_tables() except not saving them.
		void discard_tables();

		// non-reference version for public export
		mpz_class getTableContent(int m, mpz_class t, mpz_class p)
		{ return this->elementOfC(m,t,p); }

		// get a random element of K(m,t,p)
		list<int> randomElementOfK(int m, mpz_class t, mpz_class p);

		// generate a random automaton
		bool generate(int alphabet_size, int state_count, LanguageGenerator::automaton_constructor & automaton);

};

}; // end of namespace LanguageGenerator

#endif

