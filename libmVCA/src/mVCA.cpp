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

#ifdef _WIN32
# include <stdio.h>
# include <winsock.h>
#else
# include <arpa/inet.h>
#endif

#include <libmVCA/mVCA.h>
// for libmVCA::deserialize :
#include <libmVCA/deterministic_mVCA.h>
#include <libmVCA/nondeterministic_mVCA.h>

#include <libmVCA/p-automaton.h>

#include "set.h"

namespace libmVCA {

using namespace std;

static const char* libmVCA_version_string = "libmVCA version " VERSION;

const char* libmVCA_version()
{{{
	return libmVCA_version_string;
}}}



// for mVCA_run:
// NOTE: all comparison-functions don't care about the prefix! this way,
// we can easily make a breadth-first search and remember visited
// m/state tuples in a set<mVCA_run> this is used in mVCA::shortest_run
bool operator<(const mVCA_run first, const mVCA_run second)
{{{
	if(first.m < second.m)
		return true;
	if(first.m == second.m)
		return first.state < second.state;
	return false;
}}}
bool operator==(const mVCA_run first, const mVCA_run second)
{{{
	return (first.m == second.m && first.state == second.state);
}}}
bool operator>(const mVCA_run first, const mVCA_run second)
{{{
	if(first.m > second.m)
		return true;
	if(first.m == second.m)
		return first.state > second.state;
	return false;
}}}



mVCA::mVCA()
{{{
	state_count = 0;
	initial_state = -1;
	m_bound = -2;
}}}
void mVCA::set_alphabet(pushdown_alphabet & alphabet)
{{{
	this->alphabet = alphabet;
}}}
void mVCA::unset_alphabet()
{{{
	this->alphabet.clear();
}}}
pushdown_alphabet mVCA::get_alphabet()
{{{
	return this->alphabet;
}}}
enum pushdown_direction mVCA::alphabet_get_direction(int sigma)
{{{
	return this->alphabet.get_direction(sigma);
}}}
int mVCA::get_alphabet_size()
{{{
	return this->alphabet.get_alphabet_size();
}}}
int mVCA::get_m_bound()
{{{
	return this->m_bound;
}}}

int mVCA::get_state_count()
{{{
	return state_count;
}}}
int mVCA::get_initial_state()
{{{
	return initial_state;
}}}
set<int> mVCA::get_initial_states()
{{{
	set<int> ret;
	ret.insert(initial_state);
	return ret;
}}}
set<int> mVCA::get_final_states()
{{{
	return final_states;
}}}
bool mVCA::set_initial_state(unsigned int state)
{{{
	if(state >= 0 && state < state_count) {
		this->initial_state = state;
		return true;
	} else {
		return false;
	}
}}}
bool mVCA::set_final_state(const set<int> & states)
{{{
	set<int>::iterator si;
	for(si = states.begin(); si != states.end(); ++si)
		if(*si < 0 || *si >= (int)state_count)
			return false;
	final_states = states;
	return true;
}}}

bool mVCA::contains_initial_states(const set<int> & states)
{{{
	set<int>::iterator si;
	for(si = states.begin(); si != states.end(); ++si)
		if(*si == initial_state)
			return true;
	return false;
}}}
bool mVCA::contains_final_states(const set<int> & states)
{{{
	set<int>::iterator si;
	for(si = states.begin(); si != states.end(); ++si)
		if(final_states.find(*si) != final_states.end())
			return true;
	return false;
}}}

set<int> mVCA::transition(int from, int & m, int label)
{{{
	set<int> src;
	src.insert(from);
	return transition(src, m, label);
}}}

set<int> mVCA::run(const set<int> & from, int & m, list<int>::iterator word, list<int>::iterator word_limit)
{{{
	set<int> current = from;

	while(word != word_limit) {
		if(!endo_transition(current, m, *word)) {
			++word;
			break; // current is cleared by endo_transition.
		}
		++word;
	}

	return current;
}}}



list<int> mVCA::shortest_run(const set<int> & from, int m, const set<int> & to, int to_m, bool &reachable)
{{{
	// using the saturation algorithm to create the regular set Pre*(C),
	// where C is the regular set of all accepting configurations (c, m) with c in <to> and m = <to_m>.
	// for a reference on this algorithm, see e.g. the
	// Lecture on Applied Automata Theory, Chair of Computer Science 7, RWTH Aachen University of Technology

	set<int>::iterator si;

	p_automaton pa(this);
	for(si = to.begin(); si != to.end(); ++si)
		pa.add_accepting_configuration(*si, to_m);

	pa.saturate_preSTAR();

	list<int> old_word;
	list<int> new_word;
	bool new_word_valid = false;
	reachable = false;

	for(si = from.begin(); si != from.end(); ++si) {
		new_word = pa.get_shortest_valid_mVCA_run(*si, m, new_word_valid);
		// ensure we use the shortest possible run
		if(new_word_valid) {
			if(!reachable || new_word.size() < old_word.size()) {
				reachable = true;
				old_word = new_word;
			}
		}
	}

	if(!reachable)
		old_word.clear();
	return old_word;
}}}

bool mVCA::contains(list<int> & word)
{{{
	return this->contains(word.begin(), word.end());
}}}
bool mVCA::contains(list<int>::iterator word, list<int>::iterator word_limit)
{{{
	set<int> s;
	set<int> ini;
	int m = 0;
	ini = this->get_initial_states();
	s = this->run(ini, m, word, word_limit);
	if(m != 0)
		return false;
	return contains_final_states(s);
}}}
// obtain shortest word in language resp. test if language is empty,
list<int> mVCA::get_sample_word(bool & is_empty)
{{{
	set<int> i = get_initial_states();
	set<int> f = get_final_states();
	bool reachable;
	list<int> w;
	w = shortest_run(i, 0, f, 0, reachable);
	is_empty = ! reachable;
	return w;
}}}
bool mVCA::is_empty()
{{{
	bool ret;
	get_sample_word(ret);
	return ret;
}}}

/*
mVCA * mVCA::crossproduct(mVCA & other, bool intersect)
// if intersect, we build the intersection of both automata. otherwise we build the union.
{
	if(this->alphabet != other.alphabet)
		return NULL;

	mVCA * ret = new nondeterministic_mVCA();


	

	return ret;
}

int mVCA::crossproduct_state_match(mVCA & other, int this_state, int other_state)
// in a possible cross-product, get the state representing (this, other)
{ return this_state * other.get_state_count() + other_state; }

bool mVCA::lang_subset_of(mVCA & other, list<int> & counterexample)
{
	// if this is a subset of other, there exists no word s.t. this accepts it and other does not accept it.

	// A) we create the cross-product of both automata.
	
	// B) then we calculate Pre* of all configurations C that represent configurations
	//    that are accepting in this and not accepting in other.
	
	// C) then we check if any initial configuration is in Pre*(C).
	//    if so, this is not a subset of other and the specific run is a sampleword for this.
	
}

bool mVCA::lang_equal(mVCA & other, list<int> & counterexample)
{
	// almost the same as lang_subset_of, except that we check for reachability of ANY state
	// being final in one and not final in the other automaton.
	
}

bool mVCA::lang_disjoint_to(mVCA & other, list<int> & counterexample)
{
	// FIXME: check if both are deterministic, otherwise fail somehow

	mVCA * tmp;
	bool reachable;

	tmp = this->crossproduct(other, true);
	counterexample = tmp->get_sample_word(reachable);
	delete tmp;

	return !reachable;
}
*/

basic_string<int32_t> mVCA::serialize()
{{{
	basic_string<int32_t> ret;
	set<int>::iterator si;

	ret += 0; // size, will be filled in later.

	ret += ::serialize((int) this->get_derivate_id() );
	ret += ::serialize(state_count);
	ret += alphabet.serialize();
	ret += ::serialize(initial_state);
	ret += ::serialize(final_states);
	ret += ::serialize(m_bound);
	ret += this->serialize_derivate();

	ret[0] = htonl(ret.length() - 1);

	return ret;
}}}
bool mVCA::deserialize(serial_stretch & serial)
{{{
	int size;
	int type;

	if(!::deserialize(size, serial)) return false; // we don't care about the size
	if(!::deserialize(type, serial)) return false;
	if(type != (int) this->get_derivate_id() ) return false;
	if(!::deserialize(state_count, serial)) return false;
	if(state_count < 1) return false;
	if(!alphabet.deserialize(serial)) return false;
	if(!::deserialize(initial_state, serial)) return false;
	if(initial_state < 0 || initial_state >= (int)state_count) return false;
	if(!::deserialize(final_states, serial)) return false;
	if(!::deserialize(m_bound, serial)) return false;
	if(m_bound < 0) return false;
	if(!this->deserialize_derivate(serial)) return false;

	return true;
}}}

string mVCA::generate_dotfile()
{{{
	string ret;
	char buf[128];
	set<int>::iterator si;

	ret += "digraph m_bounded_1_visible_counter_automaton {\n"
		"\tgraph[fontsize=8];\n"
		"\trankdir=LR;\n"
		"\tsize=8;\n";

	// m-bound as title
	snprintf(buf, 128, "\tlabel=\"m-bound=%d\";\n\n", m_bound);
	ret += buf;

	// mark final states with double ring
	if(!final_states.empty()) {
		ret += "\tnode [shape=doublecircle];";
		for(si = final_states.begin(); si != final_states.end(); ++si) {
			snprintf(buf, 128, " q%d", *si);
			ret += buf;
		}
		ret += ";\n";
	}
	// normal states with single ring
	if(final_states.size() != state_count) {
		ret += "\tnode [shape=circle];";
		for(unsigned int i = 0; i < state_count; i++) {
			if(final_states.find(i) == final_states.end()) {
				snprintf(buf, 128, " q%d", i);
				ret += buf;
			}
		}
		ret += ";\n";
	}
	// add non-visible state for arrow to initial state
	ret += "\tnode [shape=plaintext, label=\"\", style=\"\"]; ini;\n";
	ret += "\n";
	snprintf(buf, 128, "\tini -> q%d [ color = blue ];\n", initial_state);
	ret += buf;
	ret += "\n";

	// and all transitions
	ret += this->get_transition_dotfile();

	// end
	ret += "};\n";

	return ret;
}}}




mVCA * deserialize_mVCA(serial_stretch & serial)
{{{
	basic_string<int32_t>::iterator adv;
	if(serial.empty()) return NULL;
	adv = serial.current;
	if(ntohl(*adv) < 1) return NULL;
	adv++;
	if(adv == serial.limit) return NULL;
	enum mVCA::mVCA_derivate derivate_type;
	derivate_type = (enum mVCA::mVCA_derivate)ntohl(*adv);

	mVCA * ret;

	switch(derivate_type) {
		case mVCA::DERIVATE_DETERMINISTIC:
			ret = new deterministic_mVCA;
			break;
		case mVCA::DERIVATE_NONDETERMINISTIC:
			ret = new nondeterministic_mVCA;
			break;
		default:
			return NULL;
	}

	if(!ret->deserialize(serial)) {
		delete ret;
		return NULL;
	}

	return ret;
}}}


mVCA * construct_mVCA(	unsigned int state_count,
			int alphabet_size, map<int, int> & alphabet_directions,
			int initial_state,
			set<int> & final_states,
			int m_bound,
			map<int, map<int, map<int, set<int> > > > & transitions
		)
{{{
	set<int>::iterator si;
	bool is_deterministic = true;

	// do some sanity-checks
	if(state_count < 1 || alphabet_size < 2 || initial_state < 0 || initial_state >= (int)state_count || m_bound < -1)
		return NULL;

	// create alphabet
	pushdown_alphabet alphabet;
	alphabet.set_alphabet_size(alphabet_size);
	map<int, int>::iterator di;
	for(di = alphabet_directions.begin(); di != alphabet_directions.end(); ++di)
		alphabet.set_direction(di->first, (enum pushdown_direction)di->second);

	map<int, map<int, map<int, set<int> > > >::iterator mmmi;
	map<int, map<int, set<int> > >::iterator mmi;
	map<int, set<int> >::iterator mi;

	for(mmmi = transitions.begin(); mmmi != transitions.end(); ++mmmi) {
		if(mmmi->first < 0 || mmmi->first > m_bound)
			return NULL;
		for(mmi = mmmi->second.begin(); mmi != mmmi->second.end(); ++mmi) {
			if(mmi->first < 0 || mmi->first >= (int)state_count)
				return NULL;
			for(mi = mmi->second.begin(); mi != mmi->second.end(); ++mi) {
				if(mi->first < 0 || mi->first >= alphabet_size)
					return NULL;
				if(mi->second.size() > 1)
					is_deterministic = false;
				for(si = mi->second.begin(); si != mi->second.end(); ++si) {
					if(*si < 0 || *si >= (int)state_count)
						return NULL;
				}
			}
		}
	}

	if(is_deterministic) {
		deterministic_mVCA * ret = new deterministic_mVCA;

		ret->alphabet = alphabet;
		ret->state_count = state_count;
		ret->initial_state = initial_state;
		ret->final_states = final_states;
		ret->m_bound = m_bound;

		for(mmmi = transitions.begin(); mmmi != transitions.end(); ++mmmi) {
			for(mmi = mmmi->second.begin(); mmi != mmmi->second.end(); ++mmi) {
				for(mi = mmi->second.begin(); mi != mmi->second.end(); ++mi) {
					if(!mi->second.empty()) {
						si = mi->second.begin();
						ret->transition_function[mmmi->first].transitions[mmi->first][mi->first] = *si;
					}
				}
			}
		}

		return ret;
	} else {
		nondeterministic_mVCA * ret = new nondeterministic_mVCA;

		ret->alphabet = alphabet;
		ret->state_count = state_count;
		ret->initial_state = initial_state;
		ret->final_states = final_states;
		ret->m_bound = m_bound;

		for(mmmi = transitions.begin(); mmmi != transitions.end(); ++mmmi)
			for(mmi = mmmi->second.begin(); mmi != mmmi->second.end(); ++mmi)
				for(mi = mmi->second.begin(); mi != mmi->second.end(); ++mi)
					ret->transition_function[mmmi->first].transitions[mmi->first][mi->first] = mi->second;

		return ret;
	}
}}}


} // end of namespace libmVCA

