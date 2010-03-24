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

#include <string>
#include <set>
#include <map>

#include <stdio.h>

#include <libalf/serialize.h>

#ifndef __libalf_conjecture_h__
# define __libalf_conjecture_h__

namespace libalf {

using namespace std;


enum conjecture_type {
	CONJECTURE_NONE = 0,
	// BEGIN

	CONJECTURE_MOORE_MACHINE = 1,
	CONJECTURE_SIMPLE_MOORE_MACHINE = 2,
	CONJECTURE_MEALY_MACHINE = 3,
	CONJECTURE_BOUNDED_SIMPLE_MVCA = 4,
	CONJECTURE_SIMPLE_MVCA = 5,

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
		unsigned int input_alphabet_size;
		unsigned int output_alphabet_size;
		unsigned int state_count;
		set<unsigned int> initial_states;
		bool final_output; // does this machine output a label every step or only at the end of calculation?
		bool omega; // is this machine for infinite words?
	public: // methods
		finite_state_machine();
		virtual bool calc_validity();
		virtual void clear();

		virtual bool calc_determinism() = 0;
};


class moore_machine: public finite_state_machine{
	public:
		vector<int> output_mapping; // mapping a state to its output-alphabet
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
	protected:
		bool parse_transition(string single);
};

class simple_moore_machine: public moore_machine {
	// a simple moore machine is a (non)deterministic finite automaton,
	// i.e. a state can either be accepting or rejecting.
	// where output alphabet: 0=reject, 1=accept.
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

		virtual set<int> get_final_states();
};



class mealy_machine : public finite_state_machine {
	public: // data
		multimap<pair<int, int>, pair<int, int> > transitions; // (state, input-alphabet) -> (output-alphabet, state)
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



class bounded_simple_mVCA : public simple_moore_machine {
	public:
		int m_bound;
	public:
		bounded_simple_mVCA();
		virtual conjecture_type get_type()
		{ return CONJECTURE_BOUNDED_SIMPLE_MVCA; };
		virtual void clear();
		virtual bool calc_validity();
		virtual bool calc_determinism();
		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(serial_stretch & serial);
		virtual string write();
		virtual bool read(string input);
		virtual string visualize();
};

class simple_mVCA : public finite_state_machine {
	public:
		// as this is a SIMPLE mVCA, output_alphabet_size is 2! (accept or reject)
		// pushdown property of input-alphabet:
		vector<int> alphabet_directions;
			// maps each member of the input alphabet to a direction:
			// +1 == UP
			//  0 == STAY
			// -1 == DOWN
			// (-100 == undefined)
		set<int> final_states;
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

		virtual set<int> get_final_states();
};

}; // end of namespace libalf

#endif // __libalf_conjecture_h__

