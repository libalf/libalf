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
#include <typeinfo>

#include <stdio.h>

#include <libalf/serialize.h>
#include <libalf/set.h>

namespace libalf {

using namespace std;



enum conjecture_type {
	CONJECTURE_NONE = 0,
	// BEGIN

	CONJECTURE_MOORE_MACHINE = 1,
	CONJECTURE_MEALY_MACHINE = 2,
	CONJECTURE_MVCA = 3,
	CONJECTURE_FINITE_AUTOMATON = 4,
	CONJECTURE_SIMPLE_MVCA = 5,
	CONJECTURE_BOUNDED_SIMPLE_MVCA = 6,

	// END
	CONJECTURE_LAST_INVALID = 7
};



class conjecture {
	public: // data
		bool valid;

	public: // members
		conjecture()
		{ clear(); }

		virtual ~conjecture()
		{ };

		// returns this->valid
		virtual bool is_valid() const
		{ return valid; }

		// return type of conjecture
		virtual conjecture_type get_type() const
		{ return CONJECTURE_NONE; }

		// check if data is valid w.r.t. conjecture type
		virtual bool calc_validity()
		{ return valid; };

		// clear data
		virtual void clear()
		{ valid = false; };

		// (de)serialize data
		virtual basic_string<int32_t> serialize() const = 0;
		virtual bool deserialize(serial_stretch & serial) = 0;

		// create/parse human readable version
		virtual string write() const = 0;
		virtual bool read(string input) = 0;

		// visual version (dotfile preferred)
		virtual string visualize() const = 0;
};



template <typename output_alphabet>
class finite_state_machine: public conjecture {
	public: // data
		bool is_deterministic;
		int input_alphabet_size;
		int state_count;
		set<int> initial_states;
		bool omega; // is this machine for infinite words?
	public: // methods
		finite_state_machine()
		{{{
			is_deterministic = true;
			input_alphabet_size = 0;
			state_count = 0;
			omega = false;
		}}}
		virtual ~finite_state_machine()
		{ };
		virtual void clear()
		{{{
			conjecture::clear();
			is_deterministic = true;
			input_alphabet_size = 0;
			state_count = 0;
			initial_states.clear();
			omega = false;
		}}}
		virtual bool calc_validity()
		{{{
			set<int>::const_iterator si;

			if(!conjecture::calc_validity())
				goto invalid;

			for(si = initial_states.begin(); si != initial_states.end(); ++si)
				if(*si < 0 || *si >= state_count)
					goto invalid;

			if(input_alphabet_size < 1 || state_count < 1)
				goto invalid;

			return true;
invalid:
			this->valid = false;
			return false;
		}}}
		virtual basic_string<int32_t> serialize() const
		{{{
			basic_string<int32_t> ret;

			if(this->valid) {
				ret += 0; // size, filled in later.
				ret += ::serialize(is_deterministic);
				ret += ::serialize(input_alphabet_size);
				ret += ::serialize(state_count);
				ret += ::serialize(initial_states);
				ret += ::serialize(omega);
				ret[0] = htonl(ret.length() - 1);
			}

			return ret;
		}}}
		virtual bool deserialize(serial_stretch & serial)
		{{{
			clear();
			int size;
			if(!::deserialize(size, serial)) goto failed;
			if(!::deserialize(is_deterministic, serial)) goto failed;
			if(!::deserialize(state_count, serial)) goto failed;
			if(!::deserialize(initial_states, serial)) goto failed;
			if(!::deserialize(omega, serial)) goto failed;

			this->valid = true;
			return true;
failed:
			clear();
			return false;
		}}}

		// calculate if state machine is deterministic (i.e. no epsilon-transtions, no two targets for source/label pair, ...)
		virtual bool calc_determinism()
		{{{
			is_deterministic = (initial_states.size() <= 1);
			return is_deterministic;
		}}}
};



template <typename output_alphabet>
class moore_machine: public finite_state_machine<output_alphabet> {
	public:
		map<int, output_alphabet> output_mapping; // mapping state to its output-alphabet
		map<int, map<int, set<int> > > transitions; // state -> input-alphabet -> { states }
		// using -1 as epsilon-transition (input-alphabet field)
	public:
		moore_machine()
		{ };
		virtual ~moore_machine()
		{ };
		virtual conjecture_type get_type() const
		{ return CONJECTURE_MOORE_MACHINE; };
		virtual void clear()
		{{{
			finite_state_machine<output_alphabet>::clear();
			output_mapping.clear();
			transitions.clear();
		}}}
		virtual bool calc_validity()
		{{{
			typename map<int, output_alphabet>::const_iterator oi;

			typename map<int, map<int, set<int> > >::const_iterator mmsi;
			typename map<int, set<int> >::const_iterator msi;
			typename set<int>::const_iterator si;

			if(!finite_state_machine<output_alphabet>::calc_validity())
				goto invalid;

			for(oi = output_mapping.begin(); oi != output_mapping.end(); ++oi)
				if(oi->first < 0 || oi->first >= this->state_count)
					goto invalid;

			for(mmsi = transitions.begin(); mmsi != transitions.end(); ++mmsi) {
				if(mmsi->first < 0 || mmsi->first >= this->state_count)
					goto invalid;
				for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
					if(msi->first < -1 || msi->first >= this->input_alphabet_size)
						goto invalid;
					for(si = msi->second.begin(); si != msi->second.end(); ++si) {
						if(*si < 0 || *si >= this->state_count)
							goto invalid;
					}
				}
			}

			return true;
invalid:
			this->valid = false;
			return false;
		}}}
		virtual bool calc_determinism()
		{{{
			if(!finite_state_machine<output_alphabet>::calc_determinism())
				return false;

			// check for epsilon transition and multiple destination states
			map<int, map<int, set<int> > >::const_iterator mmsi;
			map<int, set<int> >::const_iterator msi;

			for(mmsi = transitions.begin(); mmsi != transitions.end(); ++mmsi) {
				for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
					if(msi->first == -1 || msi->second.size() > 1)
						goto nondet;
				}
			}

			this->is_deterministic = true;
			return true;
nondet:
			this->is_deterministic = false;
			return false;
		}}}
		virtual basic_string<int32_t> serialize() const
		{{{
			basic_string<int32_t> ret;

			if(this->valid) {
				ret += 0; // size, filled in later.
				ret += finite_state_machine<output_alphabet>::serialize();
				ret += ::serialize(output_mapping);
				ret += ::serialize(transitions);
				ret[0] = htonl(ret.length() - 1);
			}

			return ret;
		}}}
		virtual bool deserialize(serial_stretch & serial)
		{{{
			clear();
			int size;
			if(!::deserialize(size, serial)) goto failed;
			if(!finite_state_machine<output_alphabet>::deserialize(serial)) goto failed;
			if(!::deserialize(output_mapping, serial)) goto failed;
			if(!::deserialize(transitions, serial)) goto failed;

			this->valid = true;
			return true;
failed:
			clear();
			return false;
		}}}
		// from current_states, follow the transitions given by word. resulting states are stored in current_states.
		virtual void run(set<int> & current_states, list<int>::const_iterator word, list<int>::const_iterator word_end) const
		{{{
			set<int>::const_iterator si;

			set<int> new_states;

			map<int, map<int, set<int> > >::const_iterator mmsi;
			map<int, set<int> >::const_iterator msi;
			set<int>::const_iterator di;

			while(word != word_end) {

				if(current_states.empty())
					break;

				for(si = current_states.begin(); si != current_states.end(); ++si) {
					mmsi = transitions.find(*si);
					if(mmsi != transitions.end()) {
						msi = mmsi->second.find(*word);
						if(msi != mmsi->second.end())
							for(di = msi->second.begin(); di != msi->second.end(); ++di)
								new_states.insert(*di);
					}
				}

				current_states.swap(new_states);
				new_states.clear();

				++word;
			}
		}}}
		virtual string write() const
		{ /* depends on output_alphabet, has to be done by you! */ return ""; }
		virtual bool read(__attribute__ ((__unused__)) string input)
		{ /* depends on output_alphabet, has to be done by you! */ return false; }
		virtual string visualize() const
		/* NOTE: depends on output_alphabet, but we expect to be operator<< to be defined for it. */
		{{{
			stringstream str;

			if(this->valid) {
				set<int>::iterator sti;
				bool header_written;

				// head
				str << "digraph finite_automaton {\n"
					"\tgraph[fontsize=8]\n"
					"\trankdir=LR;\n"
					"\tsize=8;\n\n";

				// mark final states
				header_written = false;
				typename map<int, output_alphabet>::const_iterator oi;

				// normal states
				for(int i = 0; i < this->state_count; ++i) {
					str << "\tnode [shape=circle, style=\"\", color=black, label=\"q" << i;
					oi = this->output_mapping.find(i);
					if(oi != this->output_mapping.end())
						str << "\\n[" << oi->second << "]";

					str << "\"] q" << i << ";\n";
				}

				// non-visible states for arrows to initial states
				if(!this->initial_states.empty()) {
					str << "\tnode [shape=plaintext, label=\"\", style=\"\"];";
					for(sti = this->initial_states.begin(); sti != this->initial_states.end(); ++sti)
						str << " iq" << *sti;
					str << ";\n";
				}

				// and arrows to mark initial states
				for(sti = this->initial_states.begin(); sti != this->initial_states.end(); ++sti)
					str << "\tiq" << *sti << " -> q" << *sti << " [color=blue];\n";

				// transitions
				map<int, map<int, set<int> > >::const_iterator mmsi;
				map<int, set<int> >::const_iterator msi;
				set<int>::const_iterator si;
				for(mmsi = this->transitions.begin(); mmsi != this->transitions.end(); ++mmsi)
					for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi)
						for(si = msi->second.begin(); si != msi->second.end(); ++si)
							str << "\tq" << mmsi->first << " -> q" << *si << " [label=\"" << msi->first << "\"];\n";

				// end
				str << "};\n";
			}

			return str.str();
		}}}
};



template <typename output_alphabet>
class mealy_machine: public finite_state_machine<output_alphabet> {
	public:
		map<int, map<int, set<pair<int, output_alphabet> > > > transitions; // state -> input_alphabet -> set( <state, output_alphabet> )
		// using -1 as epsilon-transition (input-alphabet field)
	public:
		mealy_machine()
		{ };
		virtual ~mealy_machine()
		{ };
		virtual conjecture_type get_type() const
		{ return CONJECTURE_MEALY_MACHINE; };
		virtual void clear()
		{{{
			finite_state_machine<output_alphabet>::clear();
			transitions.clear();
		}}}
		virtual bool calc_validity()
		{{{
			typename map<int, map<int, set<pair<int, output_alphabet> > > >::const_iterator ttsi;
			typename map<int, set<pair<int, output_alphabet> > >::const_iterator tsi;
			typename set<pair<int, output_alphabet> >::const_iterator si;

			if(!finite_state_machine<output_alphabet>::calc_validity())
				goto invalid;

			for(ttsi = transitions.begin(); ttsi != transitions.end(); ++ttsi) {
				if(ttsi->first < 0 || ttsi->first >= this->state_count)
					goto invalid;
				for(tsi = ttsi->second.begin(); tsi != ttsi->second.end(); ++tsi) {
					if(tsi->first < -1 || tsi->first >= this->input_alphabet_size)
						goto invalid;
					for(si = tsi->second.begin(); si != tsi->second.end(); ++si) {
						if(si->first < 0 || si->first >= this->state_count)
							goto invalid;
					}
				}
			}

			return true;
invalid:
			this->valid = false;
			return false;
		}}}
		virtual bool calc_determinism()
		{{{
			if(!finite_state_machine<output_alphabet>::calc_determinism())
				return false;

			// check for epsilon transitions and multiple destination states
			typename map<int, map<int, set<pair<int, output_alphabet> > > >::const_iterator ttsi;
			typename map<int, set<pair<int, output_alphabet> > >::const_iterator tsi;

			for(ttsi = transitions.begin(); ttsi != transitions.end(); ++ttsi) {
				for(tsi = ttsi->second.begin(); tsi != ttsi->second.end(); ++tsi) {
					if(tsi->first == -1 || tsi->second.size() > 1)
						goto nondet;
				}
			}

			this->is_deterministic = true;
			return true;
nondet:
			this->is_deterministic = false;
			return false;
		}}}
		virtual basic_string<int32_t> serialize() const
		{{{
			basic_string<int32_t> ret;

			if(this->valid) {
				ret += 0; // size, filled in later.
				ret += finite_state_machine<output_alphabet>::serialize();
				ret += ::serialize(transitions);
				ret[0] = htonl(ret.length() - 1);
			}

			return ret;
		}}}
		virtual bool deserialize(serial_stretch & serial)
		{{{
			clear();
			int size;
			if(!::deserialize(size, serial)) goto failed;
			if(!finite_state_machine<output_alphabet>::deserialize(serial)) goto failed;
			if(!::deserialize(transitions, serial)) goto failed;

			this->valid = true;
			return true;
failed:
			clear();
			return false;
		}}}
		virtual string write() const
		{ /* depends on output_alphabet, has to be done by you! */ return ""; }
		virtual bool read(__attribute__ ((__unused__)) string input)
		{ /* depends on output_alphabet, has to be done by you! */ return false; }
		virtual string visualize() const
		{ /* depends on output_alphabet, has to be done by you! */ return ""; }
};



template <typename output_alphabet>
class mVCA: public finite_state_machine<output_alphabet> {
	// expects omega == false.
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
		// using -1 as epsilon-transition (input-alphabet field)

		map<int, output_alphabet> output_mapping; // mapping state to its output-alphabet
	public:
		mVCA()
		{ m_bound = 0; }
		virtual ~mVCA()
		{ };
		virtual conjecture_type get_type() const
		{ return CONJECTURE_MVCA; };
		virtual void clear()
		{{{
			finite_state_machine<output_alphabet>::clear();
			alphabet_directions.clear();
			m_bound = 0;
			transitions.clear();
			output_mapping.clear();
		}}}
		virtual bool calc_validity()
		{{{
			int i;

			vector<int>::const_iterator vi;

			typename map<int, map<int, map<int, set<int> > > >::const_iterator mmmsi;
			typename map<int, map<int, set<int> > >::const_iterator mmsi;
			typename map<int, set<int> >::const_iterator msi;
			typename set<int>::const_iterator si;

			typename map<int, output_alphabet>::const_iterator oi;

			if(this->omega)
				goto invalid;

			if(!finite_state_machine<output_alphabet>::calc_validity())
				goto invalid;

			for(i = 0,vi = alphabet_directions.begin(); vi != alphabet_directions.end(); ++i, ++vi) {
				if(i >= this->input_alphabet_size)
					goto invalid;
				if(*vi < -1 || *vi > 1)
					goto invalid;
			}

			for(mmmsi = transitions.begin(); mmmsi != transitions.end(); ++mmmsi) {
				if(mmmsi->first < 0 || mmmsi->first > m_bound)
					goto invalid;
				for(mmsi = mmmsi->second.begin(); mmsi != mmmsi->second.end(); ++mmsi) {
					if(mmsi->first < 0 || mmsi->first >= this->state_count)
						goto invalid;
					for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
						if(msi->first < -1 || msi->first >= this->input_alphabet_size)
							goto invalid;
						for(si = msi->second.begin(); si != msi->second.end(); ++si) {
							if(*si < 0 || *si >= this->state_count)
								goto invalid;
						}
					}
				}
			}

			for(oi = output_mapping.begin(); oi != output_mapping.end(); ++oi)
				if(oi->first < 0 || oi->first >= this->state_count)
					goto invalid;

			return true;
invalid:
			this->valid = false;
			return false;
		}}}
		virtual bool calc_determinism()
		{{{
			if(!finite_state_machine<output_alphabet>::calc_determinism())
				return false;

			// check for epsilon transition and multiple destination states
			typename map<int, map<int, map<int, set<int> > > >::const_iterator mmmsi;
			typename map<int, map<int, set<int> > >::const_iterator mmsi;
			typename map<int, set<int> >::const_iterator msi;
			typename set<int>::const_iterator si;

			for(mmmsi = transitions.begin(); mmmsi != transitions.end(); ++mmmsi) {
				for(mmsi = mmmsi->second.begin(); mmsi != mmmsi->second.end(); ++mmsi) {
					for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
						if(msi->first == -1 || msi->second.size() > 1)
							goto nondet;
					}
				}
			}

			this->is_deterministic = true;
			return true;
nondet:
			this->is_deterministic = false;
			return false;
		}}}
		virtual basic_string<int32_t> serialize() const
		{{{
			basic_string<int32_t> ret;

			if(this->valid) {
				ret += 0; // size, filled in later.
				ret += finite_state_machine<output_alphabet>::serialize();
				ret += ::serialize(alphabet_directions);
				ret += ::serialize(m_bound);
				ret += ::serialize(transitions);
				ret += ::serialize(output_mapping);
				ret[0] = htonl(ret.length() - 1);
			}

			return ret;
		}}}
		virtual bool deserialize(serial_stretch & serial)
		{{{
			clear();
			int size;
			if(!::deserialize(size, serial)) goto failed;
			if(!finite_state_machine<output_alphabet>::deserialize(serial)) goto failed;
			if(!::deserialize(alphabet_directions, serial)) goto failed;
			if(!::deserialize(m_bound, serial)) goto failed;
			if(!::deserialize(transitions, serial)) goto failed;
			if(!::deserialize(output_mapping, serial)) goto failed;

			this->valid = true;
			return true;
failed:
			clear();
			return false;
		}}}
		virtual string write() const
		{ /* depends on output_alphabet, has to be done by you! */ return ""; }
		virtual bool read(__attribute__ ((__unused__)) string input)
		{ /* depends on output_alphabet, has to be done by you! */ return false; }
		virtual string visualize() const
		{ /* depends on output_alphabet, has to be done by you! */ return ""; }
};



class finite_automaton : public moore_machine<bool> {
	// a type for [non]determinstic finite automata.

	// a state is final iff output_mapping[state] == true

	// expects omega == false.

	// XXX NOTE that the serialization-format is not conforming to the
	// standard (wrapping the parent-type), but is different to be
	// compatible with to the serialization format of libAMoRE++.
	public:
		finite_automaton()
		{ this->omega = false; };
		virtual ~finite_automaton()
		{ };
		virtual conjecture_type get_type() const
		{ return CONJECTURE_FINITE_AUTOMATON; }
		virtual bool calc_validity();
		virtual basic_string<int32_t> serialize() const;
		virtual bool deserialize(serial_stretch & serial);
		virtual string write() const;
		virtual bool read(string input);
		virtual string visualize() const;

		// checks if a word is accepted by this automaton.
		virtual bool contains(const list<int> & word) const;
		inline void get_final_states(set<int> & into) const
		{{{
			map<int, bool>::const_iterator oi;

			into.clear();

			for(oi = output_mapping.begin(); oi != output_mapping.end(); ++oi)
				if(oi->second)
					into.insert(oi->first);
		}}}
		inline set<int> get_final_states() const
		{{{
			set<int> final_states;
			this->get_final_states(final_states);
			return final_states;
		}}}
		inline void set_final_states(const set<int> &final)
		{{{
			set<int>::const_iterator si;
			this->set_all_non_accepting();
			for(si = final.begin(); si != final.end(); ++si)
				this->output_mapping[*si] = true;
		}}}
		virtual void set_all_non_accepting();
	protected:
		// parse a single, human readable transition and store it in this->transitions
		bool parse_transition(string single);
};



class simple_mVCA : public mVCA<bool> {
	// a type for simple mVCA (accepting or rejecting a word only)

	// expects omega == false.

	// XXX NOTE that the serialization-format is not conforming to the
	// standard (wrapping the parent-type), but is different to be
	// compatible with to the serialization format of libAMoRE++.
	public:
		simple_mVCA()
		{ this->omega = false; };
		virtual ~simple_mVCA()
		{ };
		virtual conjecture_type get_type() const
		{ return CONJECTURE_SIMPLE_MVCA; }
		virtual basic_string<int32_t> serialize() const;
		virtual bool deserialize(serial_stretch & serial);
		virtual string write() const;
		virtual bool read(string input);
		virtual string visualize() const;

		inline void get_final_states(set<int> & into) const
		{{{
			map<int, bool>::const_iterator oi;

			into.clear();

			for(oi = output_mapping.begin(); oi != output_mapping.end(); ++oi)
				if(oi->second)
					into.insert(oi->first);
		}}}
		inline set<int> get_final_states() const
		{{{
			set<int> final_states;
			this->get_final_states(final_states);
			return final_states;
		}}}
};



class bounded_simple_mVCA : public finite_automaton {
	// a type for bounded simple mVCAs, that is in effect a finite automaton
	// with a bound.

	// expects omega == false.
	public:
		int bound;
	public:
		bounded_simple_mVCA()
		{ bound = 0; this->omega = false; };
		virtual ~bounded_simple_mVCA()
		{ };
		virtual conjecture_type get_type() const
		{ return CONJECTURE_BOUNDED_SIMPLE_MVCA; };
		virtual void clear();
		virtual bool calc_validity();
		virtual basic_string<int32_t> serialize() const;
		virtual bool deserialize(serial_stretch & serial);
		virtual string write() const;
		virtual bool read(string input);
		virtual string visualize() const;
};



}; // end of namespace libalf

#endif // __libalf_conjecture_h__

