/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *                and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_conjecture_h__
# define __libalf_conjecture_h__

#include <string>
#include <set>
#include <map>

#include <stdio.h>

#include <libalf/serialize.h>

namespace libalf {

using namespace std;

enum conjecture_type {
	CONJECTURE_NONE = 0,
	// BEGIN

	CONJECTURE_SIMPLE_MOORE_MACHINE = 1,
	CONJECTURE_BOUNDED_SIMPLE_MVCA = 2,
	CONJECTURE_SIMPLE_MVCA = 3,
	CONJECTURE_MOORE_MACHINE = 4,
	CONJECTURE_MEALY_MACHINE = 5,

	// END
	CONJECTURE_LAST_INVALID = 6
};




class conjecture {
	public:
		bool valid;
	public: // members
		conjecture();
		virtual bool is_valid();
		virtual conjecture_type get_type()
		{ return CONJECTURE_NONE; }

		virtual bool calc_validity();

		virtual void clear();

		// serializer
		virtual basic_string<int32_t> serialize() = 0;
		virtual bool deserialize(serial_stretch & serial) = 0;

		// human readable version
		virtual string write() = 0;
		virtual bool read(string input) = 0;

		// visual version (dotfile preferred)
		virtual string visualize() = 0;
};




class finite_state_machine: public conjecture {
	// in a finite_state_machine, an epsilon transition may be indicates by a transition with label "-1"
	public: // data
		bool is_deterministic;
		int input_alphabet_size;
		int state_count;
		set<int> initial_states;
		bool omega; // is this machine for infinite words?
	public: // methods
		finite_state_machine();
		virtual bool calc_validity();
		virtual void clear();

		virtual bool calc_determinism() = 0;
};

class simple_output_finite_state_machine: public finite_state_machine {
	// a state machine only capable of accepting or rejecting a specific input
	public:
		set<int> final_states;
	public:
		simple_output_finite_state_machine();
		virtual void clear();
		virtual bool calc_validity();
};

class enhanced_output_finite_state_machine: public finite_state_machine { // HAS NOT BEEN TESTED
	// a state machine capable of complex output
	public:
		int output_alphabet_size;
		bool final_output; // does this machine output a label in every step or only at the end of calculation?
	public:
		enhanced_output_finite_state_machine();
		virtual void clear();
		virtual bool calc_validity();
};




class simple_moore_machine: public simple_output_finite_state_machine {
	// a simple moore machine is a (non)deterministic finite automaton,
	// i.e. a state can either be accepting or rejecting.
	public:
		multimap<pair<int, int>, int> transitions; // (state, input-alphabet) -> state
	public:
		simple_moore_machine();
		virtual conjecture_type get_type()
		{ return CONJECTURE_SIMPLE_MOORE_MACHINE; }
		virtual void clear();
		virtual bool calc_validity();
		virtual bool calc_determinism();
		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(serial_stretch & serial);
		virtual string write();
		virtual bool read(string input);
		virtual string visualize();
	protected:
		bool parse_transition(string single);
};

class bounded_simple_mVCA : public simple_moore_machine {
	public:
		int m_bound;
	public:
		bounded_simple_mVCA();
		virtual conjecture_type get_type()
		{ return CONJECTURE_BOUNDED_SIMPLE_MVCA; };
		virtual void clear();
		virtual bool calc_validity();
		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(serial_stretch & serial);
		virtual string write();
		virtual bool read(string input);
		virtual string visualize();
};

class simple_mVCA : public simple_output_finite_state_machine {
	public:
		// pushdown property of input-alphabet:
		vector<int> alphabet_directions;
			// maps each member of the input alphabet to a direction:
			// +1 == UP
			//  0 == STAY
			// -1 == DOWN
			// (-100 == undefined)
		int m_bound;
		map<int, map<int, map<int, set<int> > > > transitions; // m -> state -> input-alphabet -> set<states>
		// (where set<states> is singleton or empty for deterministic mVCA)
	public:
		simple_mVCA();
		virtual conjecture_type get_type()
		{ return CONJECTURE_SIMPLE_MVCA; }
		virtual void clear();
		virtual bool calc_validity();
		virtual bool calc_determinism();
		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(serial_stretch & serial);
		virtual string write();
		virtual bool read(string input);
		virtual string visualize();
};




class moore_machine: public enhanced_output_finite_state_machine { // HAS NOT BEEN TESTED
	public:
		map<int, int> output_mapping; // mapping state to its output-alphabet
		multimap<pair<int, int>, int> transitions; // (state, input-alphabet) -> state
	public:
		moore_machine();
		virtual conjecture_type get_type()
		{ return CONJECTURE_MOORE_MACHINE; };
		virtual void clear();
		virtual bool calc_validity();
		virtual bool calc_determinism();
		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(serial_stretch & serial);
		virtual string write();
		virtual bool read(string input);
		virtual string visualize();
};

class mealy_machine : public enhanced_output_finite_state_machine { // HAS NOT BEEN TESTED
	public: // data
		map<int, map<int, set<pair<int, int> > > > transitions; // state -> input_alphabet -> set( <state, output_alphabet> )
	public: // methods
		mealy_machine();
		virtual conjecture_type get_type()
		{ return CONJECTURE_MEALY_MACHINE; };
		virtual void clear();
		virtual bool calc_validity();
		virtual bool calc_determinism();
		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(serial_stretch & serial);
		virtual string write();
		virtual bool read(string input);
		virtual string visualize();
};

}; // end of namespace libalf

#endif // __libalf_conjecture_h__

