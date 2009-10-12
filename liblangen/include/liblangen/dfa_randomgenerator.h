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

#include <set>
#include <map>
#include <list>
#include <vector>

#include <gmpxx.h> // GNU Multiple Precision Arithmetic Library, C++ interface

#ifndef __liblangen_dfa_randomgenerator_h__
# define __liblangen_dfa_randomgenerator_h__

namespace liblangen {

using namespace std;

class dfa_randomgenerator {
	private:
		class table {
			private:
				map<mpz_class, map<mpz_class, mpz_class> > data;
				int m;
				mpz_class t, p;
			public:
				table(int m);

				int get_m();
				mpz_class & getElement(mpz_class t, mpz_class p);
		};
	private:
		vector<table*> tables;
	protected:
		// get table content for m,t,p
		mpz_class & elementOfC(int m, mpz_class t, mpz_class p);

		// get a random element of K(m,t,p)
		list<int> randomElementOfK(int m, mpz_class t, mpz_class p);

	public:
		dfa_randomgenerator();
		~dfa_randomgenerator();

		// discard all tables currently in memory.
		void discard_tables();

		// generate a random automaton
		bool generate(int alphabet_size, int state_count,
			      bool &t_is_dfa, int &t_alphabet_size, int &t_state_count, std::set<int> &t_initial, std::set<int> &t_final, multimap<pair<int,int>, int> &t_transitions);

};

}; // end of namespace liblangen

#endif

