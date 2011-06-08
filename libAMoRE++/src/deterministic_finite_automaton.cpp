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

#include <string>
#include <queue>
#include <stdio.h>
#include <set>

#include <ostream>
#include <fstream>

// for htonl / ntohl
#ifdef _WIN32
# include <winsock.h>
#else
# include <arpa/inet.h>
#endif

# define LIBAMORE_LIBRARY_COMPILATION
# include "amore++/deterministic_finite_automaton.h"
# include "amore++/nondeterministic_finite_automaton.h"
# include "amore++/finite_automaton.h"
# include "amore++/monoid.h"

# include <amore/nfa.h>
# include <amore/dfa.h>
# include <amore/mon.h>
# include <amore/nfa2dfa.h>
# include <amore/dfa2nfa.h>
# include <amore/dfamdfa.h>
# include <amore/dfa2mon.h>
# include <amore/testBinary.h>
# include <amore/unaryB.h>
# include <amore/binary.h>
# include <amore/rexFromString.h>
# include <amore/rex2nfa.h>
# include <amore/nfa2mnfa.h>

namespace amore {

using namespace std;

// implementation notes:
//
// libAMoRE is using '0' as epsilon, thus in amore, he alphabet is [1 .. size]
// and not [0 .. size-1]
//
// libalf uses (in construct) -1 to indicate an epsilon transition and
// uses [0 .. size-1] as the alphabet.


deterministic_finite_automaton::deterministic_finite_automaton()
{{{
	dfa_p = NULL;
}}}

deterministic_finite_automaton::deterministic_finite_automaton(dfa a)
{{{
	dfa_p = a;
}}}

deterministic_finite_automaton::deterministic_finite_automaton(const deterministic_finite_automaton & other)
{{{
	dfa_p = clonedfa(other.dfa_p);
}}}

deterministic_finite_automaton::~deterministic_finite_automaton()
{{{
	if(dfa_p) {
		freedfa(dfa_p);
		free(dfa_p);
	}
}}}

deterministic_finite_automaton * deterministic_finite_automaton::clone() const
{{{
	if(dfa_p)
		return new deterministic_finite_automaton(clonedfa(dfa_p));
	else
		return new deterministic_finite_automaton();
}}}

string deterministic_finite_automaton::to_regex() const
{{{
	string regex;
	finite_automaton * n;

	n = this->nondeterminize();
	regex = n->to_regex();
	delete n;

	return regex;
}}}

unsigned int deterministic_finite_automaton::get_state_count() const
{{{
	if(dfa_p)
		return dfa_p->highest_state + 1;
	else
		return 0;
}}}

bool deterministic_finite_automaton::is_empty() const
{{{
	bool ret;
	// libAMoRE-1.0 has empty_full_lan(), but it says
	// that it requires a minimized DFA as input
	get_sample_word(ret);
	return ret;
}}}

bool deterministic_finite_automaton::is_universal() const
// NOTE: checks for emptiness of complement, as complementing DFAs is efficient.
{{{
	bool is_uni;
	dfa c;
	deterministic_finite_automaton * C;

	c = compldfa(dfa_p);
	C = new deterministic_finite_automaton(c);
	is_uni = C->is_empty();

	delete C;
	// c is deleted by ~C.

	return is_uni;
}}}

set<int> deterministic_finite_automaton::get_initial_states() const
{{{
	set<int> ret;
	ret.insert(dfa_p->init);
	return ret;
}}}
set<int> deterministic_finite_automaton::get_final_states() const
{{{
	set<int> ret;
	for(unsigned int i = 0; i <= dfa_p->highest_state; i++)
		if(dfa_p->final[i])
			ret.insert(i);
	return ret;
}}}

void deterministic_finite_automaton::set_initial_states(set<int> &states)
{{{
	set<int>::const_iterator si;
	for(si = states.begin(); si != states.end(); si++)
		dfa_p->init = *si;
}}}
void deterministic_finite_automaton::set_final_states(set<int> &states)
{{{
	for(unsigned int s = 0; s <= dfa_p->highest_state; s++)
		if(states.find(s) != states.end())
			dfa_p->final[s] = TRUE;
		else
			dfa_p->final[s] = FALSE;
}}}


bool deterministic_finite_automaton::contains_initial_states(set<int> states) const
{{{
	return (states.find(dfa_p->init) != states.end());
}}}
bool deterministic_finite_automaton::contains_final_states(set<int> states) const
{{{
	set<int>::const_iterator si;

	for(si = states.begin(); si != states.end(); si++)
		if(dfa_p->final[*si])
			return true;

	return false;
}}}

set<int> deterministic_finite_automaton::successor_states(set<int> states) const
{{{
	set<int> ret;
	set<int>::const_iterator si;
	unsigned int sigma;

	for(si = states.begin(); si != states.end(); si++)
		for(sigma = 0; sigma < dfa_p->alphabet_size; sigma++)
			ret.insert(dfa_p->delta[sigma+1][*si]);

	return ret;
}}}
set<int> deterministic_finite_automaton::successor_states(set<int> states, int label) const
{{{
	set<int> ret;
	set<int>::const_iterator si;

	if(label < 0 || label >= (int)this->get_alphabet_size())
		return ret;

	for(si = states.begin(); si != states.end(); si++)
		ret.insert(dfa_p->delta[label+1][*si]);

	return ret;
}}}
set<int> deterministic_finite_automaton::predecessor_states(set<int> states) const
{{{
	set<int> ret;
	unsigned int state;
	unsigned int sigma;

	for(state = 0; state <= dfa_p->highest_state; state++) {
		for(sigma = 0; sigma < dfa_p->alphabet_size; sigma++) {
			if(states.find(dfa_p->delta[sigma+1][state]) != states.end()) {
				ret.insert(state);
			}
		}
	}

	return ret;
}}}
set<int> deterministic_finite_automaton::predecessor_states(set<int> states, int label) const
{{{
	set<int> ret;
	unsigned int state;

	if(label < 0 || label >= (int)this->get_alphabet_size())
		return ret;

	for(state = 0; state <= dfa_p->highest_state; state++) {
		if(states.find(dfa_p->delta[label+1][state]) != states.end()) {
			ret.insert(state);
		}
	}

	return ret;
}}}

void deterministic_finite_automaton::epsilon_closure(set<int> & states) const
{ /* just do something so we don't get a warning about unused parameters: */ states.empty(); };
void deterministic_finite_automaton::inverted_epsilon_closure(set<int> & states) const
{ /* just do something so we don't get a warning about unused parameters: */ states.empty(); };

unsigned int deterministic_finite_automaton::get_alphabet_size() const
{{{
	if(dfa_p)
		return dfa_p->alphabet_size;
	else
		return 0;
}}}

list<int> deterministic_finite_automaton::shortest_run(set<int> from, set<int> &to, bool &reachable) const
{{{
	set<int>::const_iterator si;
	queue<automaton_run> run_fifo;
	automaton_run current;

	// put initial states into fifo
	for(si = from.begin(); si != from.end(); si++) {
		current.state = *si;
		run_fifo.push(current);
	}
	from.clear();

	while(!run_fifo.empty()) {
		current = run_fifo.front();
		run_fifo.pop();

		// if state was visited before, skip it
		if(from.find(current.state) == from.end()) {
			from.insert(current.state);

			// if state is final, return its prefix
			if(to.find(current.state) != to.end()) {
				reachable = true;
				return current.prefix;
			}

			// otherwise check all possible successors
			int st = current.state;
			for(unsigned int i = 0; i < dfa_p->alphabet_size; i++) {
				// add possible successor with its prefix to fifo
				current.prefix.push_back(i);
				current.state = dfa_p->delta[i+1][st];
				run_fifo.push(current);
				current.prefix.pop_back();
			}
		}
	}

	reachable = false;
	list<int> empty;
	return empty;
}}}

bool deterministic_finite_automaton::is_reachable(set<int> &from, set<int> &to) const
{{{
	bool reachable;
	shortest_run(from, to, reachable);
	return reachable;
}}}

list<int> deterministic_finite_automaton::get_sample_word(bool & is_empty) const
{{{
	set<int> initial_states, final_states;
	list<int> ret;
	bool reachable;

	initial_states = get_initial_states();
	final_states = get_final_states();

	ret = shortest_run(initial_states, final_states, reachable);
	is_empty = !reachable;

	return ret;
}}}

bool deterministic_finite_automaton::operator==(const finite_automaton &other) const
{{{
	bool ret = false;

	const deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<const deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<const deterministic_finite_automaton*>(other.determinize());
	}

#if 0
	// note: using this will minimize() this and other!
	// but we don't want changing behaviour!

	minimize();
	o_d->minimize();

	// equiv REQUIRES minimized DFAs ... (see libAMoRE-1.0 tests/testeBinary.c::equiv
	ret = equiv(this->dfa_p, o_d->dfa_p);
#else
	if(this->lang_subset_of(*o_d))
		if(o_d->lang_subset_of(*this))
			ret = true;
#endif

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

bool deterministic_finite_automaton::lang_subset_of(const finite_automaton &other) const
{{{
	bool ret;

	const deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<const deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<const deterministic_finite_automaton*>(other.determinize());
	}

	ret = inclusion(this->dfa_p, o_d->dfa_p, true);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

bool deterministic_finite_automaton::lang_disjoint_to(const finite_automaton &other) const
{{{
	bool ret;

	const deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<const deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<const deterministic_finite_automaton*>(other.determinize());
	}

	ret = inclusion(this->dfa_p, o_d->dfa_p, false);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

bool deterministic_finite_automaton::contains(list<int> &word) const
{{{
	if(dfa_p) {
		set<int> states;
		set<int>::const_iterator si;

		states = get_initial_states();

		states = run(states, word.begin(), word.end());

		for(si = states.begin(); si != states.end(); si++)
			if(TRUE == dfa_p->final[*si])
				return true;
		return false;
	} else {
		return false;
	}
}}}

void deterministic_finite_automaton::minimize()
{{{
	dfa_p = dfamdfa(dfa_p, TRUE);
}}}

void deterministic_finite_automaton::lang_complement()
{{{
	dfa a;

	a = compldfa(dfa_p);
	freedfa(dfa_p);
	free(dfa_p);
	dfa_p = a;
}}}

finite_automaton * deterministic_finite_automaton::reverse_language() const
{{{
	nfa rev_p;

	rev_p = newnfa();
	rev_p->alphabet_size = dfa_p->alphabet_size;
	rev_p->highest_state = dfa_p->highest_state;
	rev_p->infin = newfinal(dfa_p->highest_state);
	rev_p->delta = newndelta(dfa_p->alphabet_size, dfa_p->highest_state);
	rev_p->is_eps = FALSE;

	// make initial state final
	setfinalT(rev_p->infin[dfa_p->init]);

	// copy reversed transitions
	unsigned int src, sigma;

	for(src = 0; src <= dfa_p->highest_state; src++) {
		// copy reversed transitions
		for(sigma = 1; sigma <= dfa_p->alphabet_size; sigma++)
			connect(rev_p->delta, sigma, dfa_p->delta[sigma][src], src);
		// make final states initial
		if(dfa_p->final[src] == TRUE)
			setinit(rev_p->infin[src]);
	}

	return new nondeterministic_finite_automaton(rev_p);
}}}

finite_automaton * deterministic_finite_automaton::lang_union(const finite_automaton &other) const
// libAMoRE says: alphabets need to be the same
{{{
	finite_automaton * n;
	finite_automaton * ret;

	n = this->nondeterminize();
	ret = n->lang_union(other);
	delete n;

	return ret;
}}}

finite_automaton * deterministic_finite_automaton::lang_intersect(const finite_automaton &other) const
{{{
	deterministic_finite_automaton * ret;
	const deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<const deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<const deterministic_finite_automaton*>(other.determinize());
	}

	ret = new deterministic_finite_automaton(insecfa(dfa_p, o_d->dfa_p, false));

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

deterministic_finite_automaton * deterministic_finite_automaton::lang_difference(const finite_automaton &other) const
{{{
	deterministic_finite_automaton * ret;
	const deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<const deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<const deterministic_finite_automaton*>(other.determinize());
	}

	ret = new deterministic_finite_automaton(insecfa(dfa_p, o_d->dfa_p, true));

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

finite_automaton * deterministic_finite_automaton::lang_symmetric_difference(const finite_automaton &other) const
{{{
	// return L1\L2 + L2\L1
	finite_automaton * L1_without_L2;
	finite_automaton * L2_without_L1;
	finite_automaton * ret = NULL;

	L1_without_L2 = lang_difference(other);
	L2_without_L1 = other.lang_difference(*this);

	ret = L1_without_L2->lang_union(*L2_without_L1);

	delete L1_without_L2;
	delete L2_without_L1;

	return ret;
}}}

finite_automaton * deterministic_finite_automaton::lang_concat(const finite_automaton &other) const
{{{
	finite_automaton * ret;
	finite_automaton * n;

	n = this->nondeterminize();
	ret = n->lang_concat(other);
	delete n;

	return ret;
}}}

std::basic_string<int32_t> deterministic_finite_automaton::serialize() const
{{{
	basic_string<int32_t> ret;
	basic_string<int32_t> temp;

	if(!dfa_p)
		return ret; // empty basic_string

	unsigned int s; // state id
	unsigned int l; // label only unsigned here because -1 == epsilon will not occur in dfa!

	// stream length; will be filled in later
	ret += 0;

	// is deterministic!
	ret += htonl(1);
	// alphabet size
	ret += htonl(dfa_p->alphabet_size);
	// state count
	ret += htonl(dfa_p->highest_state+1);
	// number of initial states
	ret += htonl(1);
	// initial state
	ret += htonl(dfa_p->init);
	// sum up final states
	for(s = 0; s <= dfa_p->highest_state; s++)
		if(dfa_p->final[s] == TRUE)
			temp += htonl(s);
	// number of final states
	ret += htonl(temp.length());
	// final states
	ret += temp;
	// transitions
	temp.clear();
	for(l = 0; l < dfa_p->alphabet_size; l++) {
		for(s = 0; s <= dfa_p->highest_state; s++) {
			temp += htonl(s); // source state id
			temp += htonl(l); // label
			temp += htonl(dfa_p->delta[l+1][s]); // destination
		}
	}
	ret += htonl(temp.length() / 3); // number of transtions
	ret += temp; // transitions

	ret[0] = htonl(ret.length() - 1);

	return ret;
}}}

bool deterministic_finite_automaton::deserialize(basic_string<int32_t>::const_iterator &it, basic_string<int32_t>::const_iterator limit)
{{{
	int size;
	int s, count;
	bool sink_required = false;

	if(dfa_p) {
		freedfa(dfa_p);
		free(dfa_p);
	}

	if(it == limit)
		goto dfaa_deserialization_failed_fast;

	// string length (excluding length field)
	size = ntohl(*it);
	it++;
	if(size <= 0 || limit == it) goto dfaa_deserialization_failed_fast;

	dfa_p = newdfa();

	// deterministic flag
	s = ntohl(*it);
	if(s != 1) goto dfaa_deserialization_failed;

	// alphabet size
	size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
	s = ntohl(*it);
	if(s < 1)
		return false;
	dfa_p->alphabet_size = s;

	// state count
	size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
	s = ntohl(*it);
	if(s < 1)
		return false;
	dfa_p->highest_state = s - 1;

	// allocate data structures
	dfa_p->final = newfinal(dfa_p->highest_state);
	dfa_p->delta = newddelta(dfa_p->alphabet_size, dfa_p->highest_state);
	if(!(dfa_p->final) || !(dfa_p->delta))
		goto dfaa_deserialization_failed;

	// initial state
	size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
	if(ntohl(*it) != 1) // dfa only allows exactly one initial state
		goto dfaa_deserialization_failed;

	size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
	dfa_p->init = ntohl(*it);
	if(dfa_p->init > dfa_p->highest_state)
		goto dfaa_deserialization_failed;

	// final states
	size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
	count = ntohl(*it);

	for(s = 0; s < count; s++) {
		size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
		if(ntohl(*it) > dfa_p->highest_state)
			goto dfaa_deserialization_failed;
		setfinal(dfa_p->final[ntohl(*it)],1);
	}

	// transitions
	//
	// currently, delta[n] = 0. thus all undefined transitions would lead to
	// state 0. we don't want this, thus we reset the value here and add a
	// sink later on, if required.
	for(unsigned int s = 0; s <= dfa_p->highest_state; s++)
		for(unsigned int i = 1; i <= dfa_p->alphabet_size; i++)
			dfa_p->delta[i][s] = dfa_p->highest_state+1;

	size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
	count = ntohl(*it);
	for(s = 0; s < count; s++) {
		int32_t src, label, dst;

		size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
		src = ntohl(*it);
		size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
		label = ntohl(*it);
		size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
		dst = ntohl(*it);

		if(   (label < 0) || (label >= (int)dfa_p->alphabet_size)
		   || (src < 0) || (src > (int)dfa_p->highest_state)
		   || (dst < 0) || (dst > (int)dfa_p->highest_state) )
			goto dfaa_deserialization_failed;

		// transition function: delta[sigma][source] = destination
		if(dfa_p->delta[label+1][src] != dfa_p->highest_state+1)
			if(dfa_p->delta[label+1][src] != (unsigned int)dst)
				return false; // nondeterministic transitions!
		dfa_p->delta[label+1][src] = dst;
	}

	for(unsigned int s = 0; s <= dfa_p->highest_state && !sink_required; s++)
		for(unsigned int i = 1; i <= dfa_p->alphabet_size && !sink_required; i++)
			if(dfa_p->delta[i][s] > dfa_p->highest_state)
				sink_required = true;
	if(sink_required) {
//		printf("AMoRE++ deterministic_finite_automaton::deserialize adding implicit sink\n");

		ddelta tmp;
		tmp = dfa_p->delta;

		// add one state
		dfa_p->highest_state++;
		// create new ddelta
		dfa_p->delta = newddelta(dfa_p->alphabet_size, dfa_p->highest_state);
		for(unsigned int s = 0; s < dfa_p->highest_state; s++)
			for(unsigned int i = 1; i <= dfa_p->alphabet_size; i++)
				dfa_p->delta[i][s] = tmp[i][s];
		for(unsigned int i = 1; i <= dfa_p->alphabet_size; i++)
			dfa_p->delta[i][dfa_p->highest_state] = dfa_p->highest_state;
		// remove old ddelta
		for(unsigned int i = 1; i <= dfa_p->alphabet_size; i++)
			dispose(tmp[i]);
		dispose(tmp);
	}

	size--, it++;

	if(size != 0)
		goto dfaa_deserialization_failed;

	return true;

dfaa_deserialization_failed:
	freedfa(dfa_p);
	free(dfa_p);
dfaa_deserialization_failed_fast:
	dfa_p = NULL;
	return false;
}}}

bool deterministic_finite_automaton::construct(bool is_dfa, int alphabet_size, int state_count, set<int> &initial, set<int> &final, map<int, map<int, set<int> > > &transitions)
{{{
	if(!is_dfa)
		return false;
	dfa a;
	multimap<pair<int, int>, int>::const_iterator ti,tj;

	map<int, map<int, set<int> > >::const_iterator mmsi;
	map<int, set<int> >::const_iterator msi;
	set<int>::const_iterator si;

	// DO SOME SANITY CHECKS

	// - check if initial only contains one element
	if(initial.size() != 1) {
		// we could only create an NFA from this
		return false;
	}
	// - check if transitions don't contain nondeterministic choices
	for(mmsi = transitions.begin(); mmsi != transitions.end(); ++mmsi) {
		for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
			if(msi->first == -1) {
				return false; // epsilon transition
			}
			if(msi->second.size() > 1) {
				return false; // nondeterministic transition
			}
		}
	}

	// CONSTRUCT AUTOMATON
	a = newdfa();

	a->highest_state = state_count - 1; // states [0 .. highest_state]
	si = initial.begin();
	a->init = *si; // initial states
	a->alphabet_size = alphabet_size; // alphabet size
	a->delta = newddelta(a->alphabet_size, a->highest_state); // transition function: delta[sigma][source] = destination
	// currently, delta[n] = 0. thus all undefined transitions would lead to
	// state 0. obviously stupid, thus we reset the value here and add a
	// sink later on, if required.
	bool sink_required = false;
	for(unsigned int s = 0; s <= a->highest_state; s++)
		for(unsigned int i = 1; i <= a->alphabet_size; i++)
			a->delta[i][s] = a->highest_state+1;

	for(mmsi = transitions.begin(); mmsi != transitions.end(); ++mmsi) {
		int src = mmsi->first;
		for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
			int label = msi->first;
			for(si = msi->second.begin(); si != msi->second.end(); ++si) {
				int dst = *si;
#ifdef DEBUG // sanity-check transitions {{{
				if(src >= state_count) {
					printf("deterministic_finite_automaton::construct(): in transition %d-(%d)->%d: bad src %d with state_count %d\n",
							src, label, dst,
							src, state_count);
					printf("state count: %d\nalphabet size: %d\n", state_count, alphabet_size);
					printf("throwing a NULL-ptr exception, so you get a stack-trace...\n");
					fflush(stdout);
					is_dfa = *((int*)NULL); // panik
				}
				if(dst >= state_count) {
					printf("deterministic_finite_automaton::construct(): in transition %d-(%d)->%d: bad dst %d with state_count %d\n",
							src, label, dst,
							dst, state_count);
					printf("state count: %d\nalphabet size: %d\n", state_count, alphabet_size);
					printf("throwing a NULL-ptr exception, so you get a stack-trace...\n");
					fflush(stdout);
					is_dfa = *((int*)NULL); // panik
				}
				if(label >= alphabet_size) {
					printf("deterministic_finite_automaton::construct(): in transition %d-(%d)->%d: bad label %d with asize %d\n",
							src, label, dst,
							label, alphabet_size); fflush(stdout);
					printf("state count: %d\nalphabet size: %d\n", state_count, alphabet_size);
					printf("throwing a NULL-ptr exception, so you get a stack-trace...\n");
					fflush(stdout);
					is_dfa = *((int*)NULL); // panik
				}
#endif // }}}

				a->delta[label + 1][src] = dst;
			}
		}
	}
	for(unsigned int s = 0; s <= a->highest_state && !sink_required; s++)
		for(unsigned int i = 1; i <= a->alphabet_size && !sink_required; i++)
			if(a->delta[i][s] > a->highest_state)
				sink_required = true;
	if(sink_required) {
		ddelta tmp;
		tmp = a->delta;

		// add one state
		a->highest_state++;
		// create new ddelta
		a->delta = newddelta(a->alphabet_size, a->highest_state);
		for(unsigned int s = 0; s < a->highest_state; s++)
			for(unsigned int i = 1; i <= a->alphabet_size; i++)
				a->delta[i][s] = tmp[i][s];
		for(unsigned int i = 1; i <= a->alphabet_size; i++)
			a->delta[i][a->highest_state] = a->highest_state;
		// remove old ddelta
		for(unsigned int i = 1; i <= a->alphabet_size; i++)
			dispose(tmp[i]);
		dispose(tmp);
	}
	a->final = newfinal(a->highest_state); // final states
	for(set<int>::const_iterator i = final.begin(); i != final.end(); i++)
		setfinal((a->final[*i]), 1);

	a->minimal = FALSE;

	if(dfa_p) {
		freedfa(dfa_p);
		free(dfa_p);
	}
	dfa_p = a;

	return true;
}}}

bool deterministic_finite_automaton::is_deterministic() const
{ return true; };

deterministic_finite_automaton * deterministic_finite_automaton::determinize() const
{{{
	return this->clone();
}}}

finite_automaton * deterministic_finite_automaton::nondeterminize() const
{{{
	nondeterministic_finite_automaton *a;
	a = new nondeterministic_finite_automaton(dfa2nfa(dfa_p));
	return a;
}}}

void deterministic_finite_automaton::set_dfa(dfa a)
{{{
	if(dfa_p) {
		freedfa(dfa_p);
		free(dfa_p);
	}
	dfa_p = clonedfa(a);
}}}

dfa deterministic_finite_automaton::get_dfa()
{{{
	return dfa_p;
}}}

amore_monoid * deterministic_finite_automaton::to_monoid() {
	
	monoid m = dfa2mon(dfa_p);

	return new amore_monoid(m);
}

} // end namespace amore

