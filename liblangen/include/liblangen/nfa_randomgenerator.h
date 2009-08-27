/* $Id$
 * vim: fdm=marker
 *
 * liblangen (LANguageGENerator)
 * nfa_randomgenerator: random generator for NFA
 * original idea of algorithm is described in section 4.2 of
 * "F. Denis, A. Lemay and A. Terlutte - Learning regular languages using RFSAs"
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <set>
#include <map>
#include <list>
#include <vector>

#include <gmpxx.h> // GNU Multiple Precision Arithmetic Library, C++ interface

#ifndef __liblangen_nfa_randomgenerator_h__
# define __liblangen_nfa_randomgenerator_h__

namespace liblangen {

using namespace std;

class nfa_randomgenerator {
	public:
		nfa_randomgenerator();

		// generate a random automaton
		bool generate(int alphabet_size, int state_count, int transitions_p_state, float p_initial, float p_final,
			      bool &t_is_dfa, int &t_alphabet_size, int &t_state_count, std::set<int> &t_initial, std::set<int> &t_final, multimap<pair<int,int>, int> &t_transitions);
};

}; // end of namespace liblangen

#endif

