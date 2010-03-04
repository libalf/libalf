/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libAMoRE++
 *
 * libAMoRE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libAMoRE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libAMoRE++.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <ostream>
#include <iterator>

#include <string>
#include <stdio.h>
#include <set>
#include <map>

#include <arpa/inet.h>

#include "amore++/finite_automaton.h"
#include "amore++/deterministic_finite_automaton.h"
#include "amore++/nondeterministic_finite_automaton.h"

#include "set.h"

namespace amore {

using namespace std;


finite_automaton * construct_amore_automaton(bool is_dfa, int alphabet_size, int state_count, set<int> &initial, set<int> &final, multimap<pair<int,int>, int> &transitions)
{{{
	finite_automaton * ret;
	if(is_dfa)
		ret = new deterministic_finite_automaton;
	else
		ret = new nondeterministic_finite_automaton;

	if(ret->construct(is_dfa, alphabet_size, state_count, initial, final, transitions))
		return ret;

	delete ret;
	return NULL;
}}}

finite_automaton * deserialize_amore_automaton(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{{{
	finite_automaton * ret;

	basic_string<int32_t>::iterator si;
	int is_det;

	si = it;
	if(si == limit) return NULL;
	si++;
	if(si == limit) return NULL;
	is_det = ntohl(*si);
	if(is_det != 0 && is_det != 1) return NULL;

	if(is_det == 0)
		ret = new nondeterministic_finite_automaton;

	if(is_det == 1)
		ret = new deterministic_finite_automaton;

	ret->deserialize(it, limit);

	return ret;
}}}





finite_automaton::~finite_automaton()
{ };

void finite_automaton::get_transition_maps(map<int, map<int, set<int> > > & premap, map<int, map<int, set<int> > > & postmap)
// mapping: state -> sigma -> stateset
{{{
	premap.clear();
	postmap.clear();

	for(unsigned int state = 0; state < this->get_state_count(); state++) {
		set<int> stateS;
		stateS.insert(state);
		// ( epsilon transitions are automatically included via successor_states() and predecessor_states() )
		for(unsigned int sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
			premap[state][sigma] = this->predecessor_states(stateS, sigma);
			postmap[state][sigma] = this->successor_states(stateS, sigma);
		}
	}
}}}

set<int> finite_automaton::run(set<int> from, list<int>::iterator word, list<int>::iterator word_limit)
{{{
	while(word != word_limit) {
		from = this->transition(from, *word);
		word++;
	}
	return from;
}}}

bool finite_automaton::contains(list<int> & word)
{{{
	set<int> states, final_states;
	set<int>::iterator si;

	states = this->get_initial_states();
	states = this->run(states, word.begin(), word.end());

	final_states = this->get_final_states();
	for(si = states.begin(); si != states.end(); si++)
		if(final_states.find(*si) != final_states.end())
			return true;
	return false;
}}}

string finite_automaton::visualize(bool exclude_negative_sinks)
{{{
	basic_string<int32_t> serialized;
	basic_string<int32_t>::iterator si;
	int n;

	set<int> initial, final, sink;
	set<int>::iterator sti;

	unsigned int state_count;
	bool header_written;
	char buf[128];

	string ret;

	serialized = this->serialize();
	if(serialized.length() == 0)
		return ret; // empty

	si = serialized.begin();

	ret = "digraph automaton {\n"
		"\tgraph[fontsize=8]\n"
		"\trankdir=LR;\n"
		"\tsize=8;\n\n";

	si++; // skip length field

	si++; // skip deterministic-flag

	si++; // skip alphabet size

	// state count
	state_count = ntohl(*si);
	si++;

	// number of initial states
	n = ntohl(*si);
	si++;
	// initial states
	for(/* -- */; n > 0; n--) {
		initial.insert(ntohl(*si));
		si++;
	}
	// number of final states
	n = ntohl(*si);
	si++;
	// final states
	for(/* -- */; n > 0; n--) {
		final.insert(ntohl(*si));
		si++;
	}

	// skip number of transitions (assumed to be correct)
	si++;

	// mark final states
	header_written = false;
	for(sti = final.begin(); sti != final.end(); ++sti) {
		if(!header_written) {
			ret += "\tnode [shape=doublecircle, style=\"\", color=black];";
			header_written = true;
		}
		snprintf(buf, 128, " q%d", *sti);
		ret += buf;
	}
	if(header_written)
		ret += ";\n";

	sink = negative_sink();
	if(!sink.empty()) {
		ret += "\tnode [shape=circle, style=\"filled\", color=grey];";
		for(sti = sink.begin(); sti != sink.end(); ++sti) {
			snprintf(buf, 128, " q%d", *sti);
			ret += buf;
		}
		ret += ";\n";
	}

	// default
	if(final.size() + sink.size() < state_count) {
		ret += "\tnode [shape=circle, style=\"\", color=black];";
		for(unsigned int s = 0; s < state_count; s++){
			if(final.find(s) == final.end() && sink.find(s) == sink.end()) {
				snprintf(buf, 128, " q%d", s);
				ret += buf;
			}
		}
		ret += ";\n";
	}

	// add non-visible states for arrows to initial states
	header_written = false;
	for(sti = initial.begin(); sti != initial.end(); ++sti) {
		if(!header_written) {
			ret += "\tnode [shape=plaintext, label=\"\", style=\"\"];";
			header_written = true;
		}
		snprintf(buf, 128, " iq%d", *sti);
		ret += buf;
	}
	if(header_written)
		ret += ";\n";

	// and arrows to mark initial states
	for(sti = initial.begin(); sti != initial.end(); ++sti) {
		snprintf(buf, 128, "\tiq%d -> q%d [ color = blue ];\n", *sti, *sti);
		ret += buf;
	}

	// transitions
	while(si != serialized.end()) {
		int32_t src,label,dst;
		src = ntohl(*si);
		si++;
		label = ntohl(*si);
		si++;
		dst = ntohl(*si);
		si++;
		if(exclude_negative_sinks)
			if(sink.find(src) != sink.end() || sink.find(dst) != sink.end())
				continue;

		if(label != -1)
			snprintf(buf, 128, "\tq%d -> q%d [ label = \"%d\" ];\n", src, dst, label);
		else
			snprintf(buf, 128, "\tq%d -> q%d;\n", src, dst);
		ret += buf;
	}

	// end
	ret += "};\n";

	return ret;
}}}

finite_automaton *finite_automaton::co_determinize(bool minimize)
{{{
	finite_automaton *r, *rcod, *cod;
	r = this->reverse_language();
	rcod = r->determinize();
	if(minimize)
		rcod->minimize();
	cod = rcod->reverse_language();
	delete r;
	delete rcod;

	return cod;
}}}

set<int> finite_automaton::negative_sink()
{{{
	set<int> s, pre, post;
	set<int>::iterator si;

	// find states that can reach terminal states
	pre = get_final_states();
	while(pre != s) {
		s = pre;
		pre = predecessor_states(s);
		for(set<int>::iterator si = s.begin(); si != s.end(); si++)
			pre.insert(*si);
	}

	// find states reachable from initial states
	post = get_initial_states();
	s.clear();
	while(post != s) {
		s = post;
		post = successor_states(s);
		for(set<int>::iterator si = s.begin(); si != s.end(); si++)
			post.insert(*si);
	}

	s.clear();

	for(int i = get_state_count() - 1; i >= 0; i--)
		if(pre.find(i) == pre.end() || post.find(i) == post.end())
			s.insert(i);

	return s;
}}}


// inefficient (as it only wraps another interface), but it works for all automata implementations
// that implement serialize and deserialize. implementations may provide their own, more performant
// implementation of construct().
bool finite_automaton::construct(bool is_dfa, int alphabet_size, int state_count, set<int> &initial, set<int> &final, multimap<pair<int, int>, int> &transitions)
{{{
	basic_string<int32_t> ser;
	set<int>::iterator sit;
	multimap<pair<int, int>, int>::iterator tit;

	// serialize that data and call deserialize :)
	ser += 0;

	ser += htonl( is_dfa ? 1 : 0 );

	ser += htonl(alphabet_size);

	ser += htonl(state_count);

	ser += htonl(initial.size());
	for(sit = initial.begin(); sit != initial.end(); sit++)
		ser += htonl(*sit);

	ser += htonl(final.size());
	for(sit = final.begin(); sit != final.end(); sit++)
		ser += htonl(*sit);

	ser += htonl(transitions.size());

	for(tit = transitions.begin(); tit != transitions.end(); tit++) {
		ser += htonl(tit->first.first);  // source
		ser += htonl(tit->first.second); // label
		ser += htonl(tit->second);       // desination
	}

	ser[0] = htonl(ser.length() - 1);

	basic_string<int32_t>::iterator ser_begin = ser.begin();

	return this->deserialize(ser_begin, ser.end());
}}}






// Antichain-based algorithms. See
//    M. De Wulf, L. Doyen, J.-F. Raskin
//    Antichains: A New Algorithm for Checking Universality of Finite Automata

/*
bool antichain__is_universal(list<int> & counterexample)
{

}
*/

bool finite_automaton::antichain__is_equal(finite_automaton &other, list<int> & counterexample)
{{{
	if(!this->antichain__is_superset_of(other, counterexample))
		return false;
	return other.antichain__is_superset_of(*this, counterexample);
}}}


static inline void print_word(ostream &os, const list<int> &word)
{{{
	ostream_iterator<int> out(os, ".");
	os << ".";
	copy(word.begin(), word.end(), out);
}}}

static inline void print_gamestate(ostream &con, const pair<int, pair< set<int>, list<int> > > gamestate)
{{{
	con << "( " << gamestate.first << ", ";
	print_set(con, gamestate.second.first);
	con << ", ";
	print_word(con, gamestate.second.second);
	con << " )";
}}}

static inline void antichain_attractor_remove_obsolete(multimap<int, pair<set<int>, list<int> > > & attractor, list< pair<int, pair< set<int>, list<int> > > > obsolete)
{{{
	typedef multimap<int, pair<set<int>, list<int> > > attractor_t;
	typedef pair<int, pair< set<int>, list<int> > > gamestate_t;

	pair<attractor_t::iterator, attractor_t::iterator> atr_range;
	attractor_t::iterator ati;

	list<gamestate_t>::iterator obsi;

	for(obsi = obsolete.begin(); obsi != obsolete.end(); ++obsi) {
		atr_range = attractor.equal_range(obsi->first);
		for(ati = atr_range.first; ati != atr_range.second; ++ati) {
			if(ati->second == obsi->second) {
				attractor.erase(ati);
				break;
			}
		}
	}
}}}

//#define ANTICHAIN_DEBUG

bool finite_automaton::antichain__is_superset_of(finite_automaton &other, list<int> & counterexample)
// FIXME: this version does not work for automata with epsilon-transitions.
// they induce many special cases that need to be taken care of very thoroughly
{{{
	if(this->get_alphabet_size() != other.get_alphabet_size()) {
		cerr << "libamore++::finite_automaton::antichain__is_superset_of() expects that you only feed automata to it that have the same alphabet size!\n"
			<< "now i will just return false, without any further checks!\n";
		counterexample.clear();
		return false;
	}

	typedef multimap<int, pair<set<int>, list<int> > >  attractor_t;
	typedef pair< attractor_t::iterator, attractor_t::iterator > attractor_range_t;
	typedef pair<int, pair< set<int>, list<int> > > gamestate_t;

	// attractor and helper for single gamestate
	attractor_t attractor, extension;
	attractor_t::iterator ati, xti;
	gamestate_t gamestate;
	set<int> s; // state-set
	set<int>::iterator si, ti;


	// first obtain some static data that is required repeatedly:
	// sets of initial and final states and transition maps (epsilon closed)
	set<int> this_initial, this_final;
	set<int> other_initial, other_final;
	map<int, map<int, set<int> > > this_premap, this_postmap;
	map<int, map<int, set<int> > > other_premap, other_postmap;

	this_initial = this->get_initial_states();	this->epsilon_closure(this_initial);
	this_final = this->get_final_states();		this->inverted_epsilon_closure(this_final);
	other_initial = other.get_initial_states();	other.epsilon_closure(other_initial);
	other_final = other.get_final_states();		other.inverted_epsilon_closure(other_final);
	this->get_transition_maps(this_premap, this_postmap);
	other.get_transition_maps(other_premap, other_postmap);


	// compute initial attractor:
	// attractor := { ( Fb x ( A \ FinA ) | Fb in FinB }
	// FIXME: A \ FinA muss bei epsilon-erweiterungen via der epsilon-transition erzwungen werden.
	// ( stefans code -> this.epsilonAttr()  -- nur zustaende, die mit epsilon-tr. nicht aus der
	// menge rauskommen. )
	for(unsigned int i = 0; i < this->get_state_count(); ++i)
		if(this_final.find(i) == this_final.end())
			gamestate.second.first.insert(i); // A \ FinA
#ifdef ANTICHAIN_DEBUG
	cout << "initial attractor: {\n";
#endif
	for(si = other_final.begin(); si != other_final.end(); ++si) {
		gamestate.first = *si;
		attractor.insert(gamestate);
#ifdef ANTICHAIN_DEBUG
		cout << "\t"; print_gamestate(cout, gamestate); cout << "\n";
#endif
	}
#ifdef ANTICHAIN_DEBUG
	cout << "};\n\n";
#endif

	// check if (already) we're not a superset of other
	for(ati = attractor.begin(); ati != attractor.end(); ++ati)
		if(antichain__superset_check_winning_condition(this_initial, other_initial, *ati, counterexample))
			return false;

	// compute all states in this, that have - for a specific label l - no outgoing transitions.
	// (the l-successors of these states [the empty set] are always a subset of the current attractor)
	map<int, set<int> > this_states_without_successors;
	for(unsigned int sigma = 0; sigma < this->get_alphabet_size(); ++sigma)
		for(unsigned int state = 0; state < this->get_state_count(); ++state)
			if(this_postmap[state][sigma].empty())
				this_states_without_successors[sigma].insert(state);

	// extend attractor
	// until either we reach a winning gamestate or it is no longer extensible
	extension = attractor;
	while(!extension.empty()) {
		attractor_t new_extension;

#ifdef ANTICHAIN_DEBUG
		cout << "\nNEW CYCLE:\n";
#endif

		// iterate over all gamestates and extend the attractor
		for(ati = extension.begin(); ati != extension.end(); ++ati) {
#ifdef ANTICHAIN_DEBUG
			cout << "checking attr element ";
			print_gamestate(cout, *ati);
			cout << " :\n";
#endif
			for(unsigned int sigma = 0; sigma < other.get_alphabet_size(); ++sigma) {
				if( ! other_premap[ati->first][sigma].empty()) {
#ifdef ANTICHAIN_DEBUG
					cout << "\t for label: " << sigma << "\n";
#endif
					// get all predecessors
					set<int> this_pre;
					for(si = ati->second.first.begin(); si != ati->second.first.end(); ++si)
						set_insert(this_pre, this_premap[*si][sigma]);

					// check, which of them are controllable
					set<int> uncontrollable;
					for(si = this_pre.begin(); si != this_pre.end(); ++si) {
						if(!set_includes(ati->second.first, this_postmap[*si][sigma]))
							uncontrollable.insert(*si);
					}

					// create new gamestate(s)
					gamestate.second.first = set_without(this_pre, uncontrollable);
					set_insert(gamestate.second.first, this_states_without_successors[sigma]);
					gamestate.second.second = ati->second.second;
					gamestate.second.second.push_front(sigma);
					ti = other_premap[ati->first][sigma].end(); // for fast access
					for(si = other_premap[ati->first][sigma].begin(); si != ti; ++si) {
						gamestate.first = *si;
#ifdef ANTICHAIN_DEBUG
						cout << "\t\tnew   ";
						print_gamestate(cout, gamestate);
						cout << ": ";
						// check winning condition for new gamestate
#endif
						if(antichain__superset_check_winning_condition(this_initial, other_initial, gamestate, counterexample)) {
#ifdef ANTICHAIN_DEBUG
							cout << "is winning.\n";
#endif
							return false;
						}

						// insert it into extension of attractor, so that the extension still is an antichain
						{{{
							attractor_range_t nex_range = new_extension.equal_range(*si);
							list<gamestate_t> obsolete;
							bool antichain_new_is_obsolete = false;

							for(xti = nex_range.first; xti != nex_range.second; ++xti) {
								if(set_includes(xti->second.first, gamestate.second.first)) {
									antichain_new_is_obsolete = true;
									break;
								}
								if(set_includes(gamestate.second.first, xti->second.first))
									obsolete.push_back(*xti);
							}
							obsolete.unique();
							antichain_attractor_remove_obsolete(new_extension, obsolete);
							if(!antichain_new_is_obsolete) {
#ifdef ANTICHAIN_DEBUG
								cout << "was added.\n";
#endif
								new_extension.insert(gamestate);
#ifdef ANTICHAIN_DEBUG
							} else {
								cout << "is obsolete.\n";
#endif
							}
						}}}
					}

				}
			}
		}

		// now merge with complete attractor and only keep, what was not in attractor already.
		extension.clear();
		list<gamestate_t> obsolete;
		for(unsigned int other_state = 0; other_state < other.get_state_count(); other_state++) {
			attractor_range_t attr_range, nex_range;
			attr_range = attractor.equal_range(other_state);
			nex_range = new_extension.equal_range(other_state);
			for(xti = nex_range.first; xti != nex_range.second; ++xti) {
				// check if this element is already supersedet by old attractor or supersedes attractor-elements
				bool superficial = false;
				for(ati = attr_range.first; ati != attr_range.second; ++ati) {
					if(set_includes(ati->second.first, xti->second.first)) {
#ifdef ANTICHAIN_DEBUG
						cout << "gamestate ";
						print_gamestate(cout, *xti);
						cout << " is superficial due to known attractor-state ";
						print_gamestate(cout, *ati);
						cout << ".\n";
#endif
						superficial = true;
						break;
					}
					if(set_includes(xti->second.first, ati->second.first))
						obsolete.push_back(*ati);
				}
				if(!superficial) {
#ifdef ANTICHAIN_DEBUG
					cout << "scheduling new state ";
					print_gamestate(cout, *xti);
					cout << ".\n";
#endif
					attractor.insert(*xti);
					extension.insert(*xti);
				}
			}
			obsolete.unique();
			antichain_attractor_remove_obsolete(attractor, obsolete);
		}
	}

	// attractor is maximal, no winning gamestate was found.
	// thus this is really a superset of other.
	counterexample.clear();
	return true;
}}}

// antichain helper functions:

bool finite_automaton::antichain__superset_check_winning_condition(set<int> & this_initial, set<int> & other_initial, const pair<int, pair< set<int>, list<int> > > & gamestate, list<int> & counterexample)
// checks if the given <gamestate> is a winning state, i.e. <this> can NOT be a superset of <other>.
// the specific run is copied to <counterexample>.
{{{
	// gamestate.first has to be an initial state of other.
	if(other_initial.find(gamestate.first) == other_initial.end())
		return false;

	// this_initial has to be subsetEQ of gamestate.second.first
	if(!set_includes(gamestate.second.first, this_initial))
		return false;

	counterexample = gamestate.second.second;
	return true;
}}}

} // end namespace amore

