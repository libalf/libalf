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
 * (c) 2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libmvca_mvca_h__
# define __libmvca_mvca_h__

#include <list>
#include <set>
#include <string>
#include <map>

#include <libmVCA/pushdown.h>
#include <libmVCA/transition_function.h>

namespace libmVCA {

using namespace std;

const char * libmVCA_version();

// NOTE: this implementation DOES NOT SUPPORT epsilon transitions.

template <class delta_f> // delta_f has to implement class the interface defined by class transition_function
class mVCA {
	private: // data
		int state_count;
		pushdown_alphabet alphabet;
		int initial_state;
		int m; // m-bound (upper bound for delta_function)
		map<int, delta_f> delta_function;
		set<int> final_states;

	public: // methods
		// set alphabet (will be copied, will erase all other information about former automaton)
		void set_alphabet(pushdown_alphabet & alphabet); // FIXME


		// test if word is contained in language of automaton
		bool contains(list<int> & word);

		// format for serialization:
		// all values in NETWORK BYTE ORDER!
		// <serialized automaton>
		//	FIXME
		// </serialized automaton>
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);
		// construct a new mVCA
//		bool construct(...); FIXME

		string generate_dotfile();
};

// automatically construct new automaton
//m_visibly_1counter_automaton * construct_mVCA(...); FIXME
//m_visibly_1counter_automaton * deserialize_mVCA(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit); FIXME

}; // end of namespace libmVCA.

#endif // __libmvca_mvca_h__

