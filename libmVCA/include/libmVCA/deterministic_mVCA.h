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

#ifndef __libmvca_deterministic_mvca_h__
# define __libmvca_deterministic_mvca_h__

#include <list>
#include <set>
#include <string>
#include <map>

#include <libmVCA/pushdown.h>
#include <libmVCA/transition_function.h>
#include <libmVCA/mVCA.h>

namespace libmVCA {

using namespace std;

// NOTE: this implementation DOES NOT SUPPORT epsilon transitions.

class deterministic_mVCA : public mVCA {
	protected: // data
		map<int, deterministic_transition_function> transition_function;

		friend mVCA * construct_mVCA(unsigned int state_count, pushdown_alphabet & alphabet, int initial_state, set<int> & final_states, int m_bound, map<int, map<int, map<int, set<int> > > > & transitions);

	public: // methods
		virtual set<int> transition(const set<int> & from, int & m, int label);
		virtual bool endo_transition(set<int> & states, int & m, int label);

		virtual void get_transition_map(map<int, map<int, map<int, set<int> > > > & postmap);

		virtual enum mVCA::mVCA_derivate get_derivate_id()
		{ return mVCA::DERIVATE_DETERMINISTIC; };

		virtual void add_transition(int m, int src, int label, int dst);
	protected:
		// format for serialization:
		// all values in NETWORK BYTE ORDER!
		// <serialized derivate-data>
		//	m_bound+1 (number of following transition-functions)
		//	<serialized transition functions>[]
		// </serialized automaton>
		virtual basic_string<int32_t> serialize_derivate();
		virtual bool deserialize_derivate(serial_stretch & serial);
		virtual string get_transition_dotfile();
};

}; // end of namespace libmVCA.

#endif // __libmvca_deterministic_mvca_h__

