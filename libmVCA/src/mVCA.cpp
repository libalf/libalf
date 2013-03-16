/* vim: fdm=syntax foldlevel=1 foldnestmax=2
 * $Id$
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
 *           and David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifdef _WIN32
# include <winsock.h>
#else
# include <arpa/inet.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include <libmVCA/mVCA.h>
// for libmVCA::deserialize :
#include <libmVCA/deterministic_mVCA.h>
#include <libmVCA/nondeterministic_mVCA.h>

#include <libmVCA/p-automaton.h>

namespace libmVCA {

using namespace std;

static const char* libmVCA_version_string = "libmVCA version " VERSION;

const char* libmVCA_version()
{
	return libmVCA_version_string;
}



// for mVCA_run:
// NOTE: all comparison-functions don't care about the prefix! this way,
// we can easily make a breadth-first search and remember visited
// m/state tuples in a set<mVCA_run> this is used in mVCA::shortest_run
bool operator<(const mVCA_run first, const mVCA_run second)
{
	if(first.m < second.m)
		return true;
	if(first.m == second.m)
		return first.state < second.state;
	return false;
}
bool operator==(const mVCA_run first, const mVCA_run second)
{
	return (first.m == second.m && first.state == second.state);
}
bool operator>(const mVCA_run first, const mVCA_run second)
{
	if(first.m > second.m)
		return true;
	if(first.m == second.m)
		return first.state > second.state;
	return false;
}



mVCA::mVCA()
{
	state_count = 0;
	initial_state = -1;
	m_bound = -2;
}
mVCA::~mVCA()
{ };
void mVCA::set_alphabet(pushdown_alphabet & alphabet)
{
	this->alphabet = alphabet;
}
void mVCA::unset_alphabet()
{
	this->alphabet.clear();
}
pushdown_alphabet mVCA::get_alphabet() const
{
	return this->alphabet;
}
enum pushdown_direction mVCA::alphabet_get_direction(int sigma) const
{
	return this->alphabet.get_direction(sigma);
}
int mVCA::get_alphabet_size() const
{
	return this->alphabet.get_alphabet_size();
}
int mVCA::get_m_bound() const
{
	return this->m_bound;
}

int mVCA::get_state_count() const
{
	return state_count;
}
int mVCA::get_initial_state() const
{
	return initial_state;
}
set<int> mVCA::get_initial_states() const
{
	set<int> ret;
	ret.insert(initial_state);
	return ret;
}
set<int> mVCA::get_final_states() const
{
	return final_states;
}
bool mVCA::set_initial_state(unsigned int state)
{
	if(state < state_count) {
		this->initial_state = state;
		return true;
	} else {
		return false;
	}
}
bool mVCA::set_final_state(const set<int> & states)
{
	set<int>::iterator si;
	for(si = states.begin(); si != states.end(); ++si)
		if(*si < 0 || *si >= (int)state_count)
			return false;
	final_states = states;
	return true;
}

bool mVCA::contains_initial_states(const set<int> & states) const
{
	set<int>::iterator si;
	for(si = states.begin(); si != states.end(); ++si)
		if(*si == initial_state)
			return true;
	return false;
}
bool mVCA::contains_final_states(const set<int> & states) const
{
	set<int>::iterator si;
	for(si = states.begin(); si != states.end(); ++si)
		if(final_states.find(*si) != final_states.end())
			return true;
	return false;
}

set<int> mVCA::transition(int from, int & m, int label) const
{
	set<int> src;
	src.insert(from);
	return transition(src, m, label);
}

set<int> mVCA::run(const set<int> & from, int & m, list<int>::const_iterator word, list<int>::const_iterator word_limit) const
{
	set<int> current = from;

	while(word != word_limit) {
		if(!endo_transition(current, m, *word)) {
			++word;
			break; // current is cleared by endo_transition.
		}
		++word;
	}

	return current;
}



list<int> mVCA::shortest_run(const set<int> & from, int m, const set<int> & to, int to_m, bool &reachable) const
{
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
}

bool mVCA::contains(const list<int> & word) const
{
	return this->contains(word.begin(), word.end());
}
bool mVCA::contains(list<int>::const_iterator word, list<int>::const_iterator word_limit) const
{
	set<int> s;
	set<int> ini;
	int m = 0;
	ini = this->get_initial_states();
	s = this->run(ini, m, word, word_limit);
	if(m != 0)
		return false;
	return contains_final_states(s);
}
// obtain shortest word in language resp. test if language is empty,
list<int> mVCA::get_sample_word(bool & is_empty) const
{
	set<int> i = get_initial_states();
	set<int> f = get_final_states();
	bool reachable;
	list<int> w;
	w = shortest_run(i, 0, f, 0, reachable);
	is_empty = ! reachable;
	return w;
}
bool mVCA::is_empty() const
{
	bool ret;
	get_sample_word(ret);
	return ret;
}

void mVCA::get_bounded_behaviour_graph(int m_bound, bool & f_is_deterministic, int & f_alphabet_size, int & f_state_count, set<int> & f_initial_states, set<int> & f_final_states, map<int, map<int, set<int> > > & f_transitions) const
{
	// state in new automaton := state + m*state_count
	f_is_deterministic = (this->get_derivate_id() == DERIVATE_DETERMINISTIC);
	f_alphabet_size = alphabet.get_alphabet_size();
	f_state_count = state_count * (m_bound+1);
	f_initial_states.clear();
	f_initial_states.insert(initial_state);
	f_final_states = final_states;

	f_transitions.clear();
	for(int m = 0; m <= m_bound; ++m) {
		for(int label = 0; label < alphabet.get_alphabet_size(); ++label) {
			for(unsigned int src = 0; src < state_count; ++src) {
				int new_m = m;

				set<int> dst = transition(src, new_m, label);

				if(new_m < 0 || new_m > m_bound)
					break;

				for(set<int>::iterator si = dst.begin(); si != dst.end(); ++si)
					f_transitions[src + m*state_count][label].insert(*si + new_m * state_count);
			}
		}
	}
}

mVCA * mVCA::crossproduct(const mVCA & other) const
{
	unsigned int f_state_count;
	int f_initial_state;
	set<int> f_final_states;
	int f_m_bound;
	map<int, map<int, map<int, set<int> > > > f_transitions;

	// alphabet check
	if(alphabet != other.alphabet) {
		cerr << "libmVCA::mVCA::crossproduct(...): given automata do not have the same alphabets. aborting program.\n";
		exit(-1);
	}

	// state count
	f_state_count = this->state_count * other.state_count;

	// initial state
	f_initial_state = crossproduct_state_match(other, this->initial_state, other.initial_state);

	// final states
	set<int>::iterator si, di;
	for(si = final_states.begin(); si != final_states.end(); ++si)
		for(di = other.final_states.begin(); di != other.final_states.end(); ++di)
			f_final_states.insert(crossproduct_state_match(other, *si, *di));

	// m_bound
	f_m_bound = max(m_bound, other.m_bound);

	// transitions (m->state->sigma->states)
	map<int, map<int, map<int, set<int> > > > postmap, other_postmap;
	int tm, om, m;
	map<int, map<int, set<int> > >::iterator tmmsi, ommsi;
	// for set<int> we use si, di;
	get_transition_map(postmap);
	other.get_transition_map(other_postmap);

	for(m = 0, tm = 0, om = 0; m <= f_m_bound; ++m) {
		for(tmmsi = postmap[tm].begin(); tmmsi != postmap[tm].end(); ++tmmsi) {
			for(ommsi = other_postmap[om].begin(); ommsi != other_postmap[om].end(); ++ommsi) {
				int src = crossproduct_state_match(other, tmmsi->first, ommsi->first);
				for(int label = 0; label < this->alphabet.get_alphabet_size(); ++label) {
					for(si = tmmsi->second[label].begin(); si != tmmsi->second[label].end(); ++si)
						for(di = ommsi->second[label].begin(); di != ommsi->second[label].end(); ++di)
						{
							int dst = crossproduct_state_match(other, *si, *di);
							f_transitions[m][src][label].insert(dst);
						}
				}
			}
		}
		if(tm < this->m_bound)
			++tm;
		if(om < other.m_bound)
			++om;
	}

	// for performance, remove all transitions that can not be reached from the initial state
	// TODO

	mVCA * ret;
	ret = construct_mVCA(f_state_count, alphabet, f_initial_state, f_final_states, f_m_bound, f_transitions);
	return ret;
}
int mVCA::crossproduct_state_match(const mVCA & other, int this_state, int other_state) const
// in a possible cross-product, get the state representing (this, other)
{ return this_state * other.get_state_count() + other_state; }

void mVCA::complete_automaton()
// make this mVCA be complete
// (add sink state, add missing outgoing transitions from any state to sink)
{
	// check if there exists a negative sink
	int sink = find_sink();
	// otherwise add a new one
	if(sink < 0) {
		sink = state_count;
		state_count++;
	};

	// for any state, if for a (m,label) there is no outgoing transition, add one the negative sink.
	for(int m = 0; m <= m_bound; ++m) {
		for(unsigned int src = 0; src < state_count; ++src) {
			for(int label = 0; label < alphabet.get_alphabet_size(); ++label) {
				int tmp_m = m;
				if(transition(src, tmp_m, label).empty()) {
					if(tmp_m >= 0) {
						// add transition
						add_transition(m, src, label, sink);
					}
				}
			}
		}
	}
}

int mVCA::find_sink() const
{
	int ret = -1;

	for(unsigned int i = 0; i < state_count; ++i) {
		if(final_states.find(i) != final_states.end())
			continue;

		bool valid = true;
		for(int m = 0; m <= m_bound; ++m) {
			for(int sigma = 0; sigma <= alphabet.get_alphabet_size(); ++sigma) {
				int tmp_m = m;
				set<int> dst = this->transition(i, tmp_m, sigma);
				if(tmp_m >= 0) {
					dst.erase(i);
					if(!dst.empty()) {
						valid = false;
						break;
					}
				}
			}
		}
		if(valid) {
			ret = i;
			break;
		}
	}

	return ret;
}

bool mVCA::lang_subset_of(mVCA & other, list<int> & counterexample)
// NOTE: both this and other have to be deterministic. otherwise, exit().
// NOTE: this implicitly calls complete_automaton() for this and other!
{
	if(this->get_derivate_id() != DERIVATE_DETERMINISTIC || other.get_derivate_id() != DERIVATE_DETERMINISTIC) {
		cerr << "libmVCA::mVCA::lang_subset_of(...): given automata are not deterministic. aborting program.\n";
		exit(-1);
	}
	this->complete_automaton();
	other.complete_automaton();

	// if this is a subset of other, there exists no word s.t. this accepts it and other does not accept it.

	// A) we create the cross-product of both automata.
	mVCA * cross;
	cross = this->crossproduct(other);
	// B) then we calculate Pre* of all configurations C that represent configurations
	//    that are accepting in this and not accepting in other.
	p_automaton pa(cross);
	set<int>::iterator si;
	for(si = final_states.begin(); si != final_states.end(); ++si)
		for(unsigned int i = 0; i < other.state_count; ++i)
			if(other.final_states.find(i) == other.final_states.end()) {
				pa.add_accepting_configuration(crossproduct_state_match(other, *si, i), 0);
			}
	pa.saturate_preSTAR();
	// C) then we check if any initial configuration is in Pre*(C).
	//    if so, this is not a subset of other and the specific run is a sampleword for this.
	bool bad_state_reachable;
	counterexample = pa.get_shortest_valid_mVCA_run(crossproduct_state_match(other, initial_state, other.initial_state), 0, bad_state_reachable);
	delete cross;
	return !bad_state_reachable;
}

bool mVCA::lang_equal(mVCA & other, list<int> & counterexample)
// NOTE: both this and other have to be deterministic. otherwise, exit().
// NOTE: this implicitly calls complete_automaton() for this and other!
{
	// almost the same as lang_subset_of, except that we check for reachability of ANY state
	// being final in one and not final in the other automaton.

	if(this->get_derivate_id() != DERIVATE_DETERMINISTIC || other.get_derivate_id() != DERIVATE_DETERMINISTIC) {
		cerr << "libmVCA::mVCA::lang_equal(...): given automata are not deterministic. aborting program.\n";
		exit(-1);
	}
	this->complete_automaton();
	other.complete_automaton();

	// if this is a subset of other, there exists no word s.t. this accepts it and other does not accept it
	// or other accepts it and this does not accept it.

	// A) we create the cross-product of both automata.
	mVCA * cross;
	cross = this->crossproduct(other);
	// B) then we calculate Pre* of all configurations C that represent configurations
	//    that are accepting in this and not accepting in other.
	p_automaton pa(cross);
	set<int>::iterator si;
	for(si = final_states.begin(); si != final_states.end(); ++si)
		for(unsigned int i = 0; i < other.state_count; ++i)
			if(other.final_states.find(i) == other.final_states.end()) {
				pa.add_accepting_configuration(crossproduct_state_match(other, *si, i), 0);
			}
	for(si = other.final_states.begin(); si != other.final_states.end(); ++si)
		for(unsigned int i = 0; i < state_count; ++i)
			if(final_states.find(i) == final_states.end()) {
				pa.add_accepting_configuration(crossproduct_state_match(other, i, *si), 0);
			}
	pa.saturate_preSTAR();
	// C) then we check if any initial configuration is in Pre*(C).
	//    if so, this is not a subset of other and the specific run is a sampleword for this.
	bool bad_state_reachable;
	counterexample = pa.get_shortest_valid_mVCA_run(crossproduct_state_match(other, initial_state, other.initial_state), 0, bad_state_reachable);
	delete cross;
	return !bad_state_reachable;
}

bool mVCA::lang_disjoint_to(const mVCA & other, list<int> & counterexample) const
// NOTE: both this and other have to be deterministic. otherwise, exit().
{
	if(this->get_derivate_id() != DERIVATE_DETERMINISTIC || other.get_derivate_id() != DERIVATE_DETERMINISTIC) {
		cerr << "libmVCA::mVCA::lang_disjoint_to(...): given automata are not deterministic. aborting program.\n";
		exit(-1);
	}

	mVCA * tmp;
	bool intersect_is_empty;

	tmp = this->crossproduct(other);
	counterexample = tmp->get_sample_word(intersect_is_empty);
	delete tmp;

	return intersect_is_empty;
}

mVCA * mVCA::lang_intersect(const mVCA & other) const
// NOTE: both this and other have to be deterministic. otherwise, exit().
{
	if(this->get_derivate_id() != DERIVATE_DETERMINISTIC || other.get_derivate_id() != DERIVATE_DETERMINISTIC) {
		cerr << "libmVCA::mVCA::lang_intersect(...): given automata are not deterministic. aborting program.\n";
		exit(-1);
	}

	mVCA * tmp;

	tmp = this->crossproduct(other);

	return tmp;
}

basic_string<int32_t> mVCA::serialize() const
{
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
}
bool mVCA::deserialize(serial_stretch & serial)
{
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
}

string mVCA::visualize() const
{
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
}




mVCA * deserialize_mVCA(serial_stretch & serial)
{
	basic_string<int32_t>::const_iterator adv;
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
}

mVCA * construct_mVCA(	unsigned int state_count,
			int alphabet_size, const vector<int> & alphabet_directions,
			int initial_state,
			const set<int> & final_states,
			int m_bound,
			const map<int, map<int, map<int, set<int> > > > & transitions
		)
{
	pushdown_alphabet alphabet;
	alphabet.set_alphabet_size(alphabet_size);
	for(int sigma = 0; sigma < alphabet_size; ++sigma)
		alphabet.set_direction(sigma, (enum pushdown_direction) alphabet_directions[sigma]);

	return construct_mVCA(state_count, alphabet, initial_state, final_states, m_bound, transitions);
}


mVCA * construct_mVCA(	unsigned int state_count,
			const pushdown_alphabet & alphabet,
			int initial_state,
			const set<int> & final_states,
			int m_bound,
			const map<int, map<int, map<int, set<int> > > > & transitions
		)
{
	set<int>::const_iterator si;
	bool is_deterministic = true;

	// do some sanity-checks
	if(state_count < 1 || initial_state < 0 || initial_state >= (int)state_count || m_bound < -1)
		return NULL;

	map<int, map<int, map<int, set<int> > > >::const_iterator mmmi;
	map<int, map<int, set<int> > >::const_iterator mmi;
	map<int, set<int> >::const_iterator mi;

	for(mmmi = transitions.begin(); mmmi != transitions.end(); ++mmmi) {
		if(mmmi->first < 0 || mmmi->first > m_bound)
			return NULL;
		for(mmi = mmmi->second.begin(); mmi != mmmi->second.end(); ++mmi) {
			if(mmi->first < 0 || mmi->first >= (int)state_count)
				return NULL;
			for(mi = mmi->second.begin(); mi != mmi->second.end(); ++mi) {
				if(mi->first < 0 || mi->first >= alphabet.get_alphabet_size())
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
}


} // end of namespace libmVCA

