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

// interface and common functions for nondeterministic_mVCA and deterministic_mVCA.
class mVCA {
	public: // types
		enum mVCA_derivate {
			DERIVATE_NONE = 0,

			DERIVATE_DETERMINISTIC = 1,
			DERIVATE_NONDETERMINISTIC = 2,

			DERIVATE_LAST_INVALID = 3
		};
	protected: // data
		int state_count;
		pushdown_alphabet alphabet;
		int initial_state;
		set<int> final_states;

		int m; // m-bound (upper bound for delta_function)
//		delta_function :: implemented by deriving classes

	public: // methods

		// ALPHABET
		// set alphabet (will be copied, will erase all other information about former automaton)
		void set_alphabet(pushdown_alphabet & alphabet);
		void unset_alphabet();
		pushdown_alphabet get_alphabet();
		int get_alphabet_size();

		// INITIAL/FINAL STATES
		int get_initial_state();
		set<int> get_initial_states();
		set<int> get_final_states();
		bool set_initial_state(int state);
		bool set_final_state(const set<int> & states);

		bool contains_initial_states(const set<int> & states);
		bool contains_final_states(const set<int> & states);

		// TRANSITIONS
		// XXX: successor/predecessor/transition maps(?)

		virtual set<int> transition(const set<int> & from, int & m, int label) = 0; // depends on delta function

		set<int> run(const set<int> & from, int & m, list<int>::iterator word, list<int>::iterator word_limit);
		list<int> shortest_run(const set<int> & from, int & m, const set<int> & to, bool &reachable);
		// test if word is contained in language of automaton
		bool contains(list<int> & word);
		bool contains(list<int>::iterator word, list<int>::iterator word_limit);
		// obtain shortest word in language resp. test if language is empty,
		list<int> get_sample_word(bool & is_empty);
		bool is_empty();

		// obtain id of unique derived class
		virtual enum mVCA_derivate get_derivate_id() = 0;

		// format for serialization:
		// all values in NETWORK BYTE ORDER!
		// <serialized automaton>
		//	FIXME
		// </serialized automaton>
		basic_string<int> serialize();
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);
		// construct a new mVCA
		//		bool construct(...); FIXME

		virtual string generate_dotfile() = 0;
	protected:
		virtual basic_string<int32_t> serialize_derivate() = 0;
		virtual bool deserialize_derivate(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;
};

// automatically construct new automaton
//mVCA * construct_mVCA(...); FIXME
mVCA * deserialize_mVCA(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

}; // end of namespace libmVCA.

#endif // __libmvca_mvca_h__

