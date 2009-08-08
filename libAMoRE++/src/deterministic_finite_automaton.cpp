/* $Id$
 * vim: fdm=marker
 *
 * amore++
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <string>
#include <queue>
#include <stdio.h>
#include <set>

#include <ostream>
#include <fstream>

// for htonl / ntohl
# include <arpa/inet.h>

# define LIBAMORE_LIBRARY_COMPILATION
# include "amore++/deterministic_finite_automaton.h"
# include "amore++/nondeterministic_finite_automaton.h"
# include "amore++/finite_automaton.h"

# include <amore/nfa.h>
# include <amore/dfa.h>
# include <amore/nfa2dfa.h>
# include <amore/dfa2nfa.h>
# include <amore/dfamdfa.h>
# include <amore/testBinary.h>
# include <amore/unaryB.h>
# include <amore/binary.h>
# include <amore/rexFromString.h>
# include <amore/rex2nfa.h>
# include <amore/nfa2mnfa.h>

// attention: stupid amore headers typedef string to be char*
// thus we have to use "std::string"...
// nfa2mnfa.h defines some 'set', so we have to use "std::set"

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

deterministic_finite_automaton::~deterministic_finite_automaton()
{{{
	if(dfa_p) {
		freedfa(dfa_p);
		free(dfa_p);
	}
}}}

deterministic_finite_automaton * deterministic_finite_automaton::clone()
{{{
	if(dfa_p)
		return new deterministic_finite_automaton(clonedfa(dfa_p));
	else
		return new deterministic_finite_automaton();
}}}

int deterministic_finite_automaton::get_state_count()
{{{
	if(dfa_p)
		return dfa_p->highest_state + 1;
	else
		return 0;
}}}

bool deterministic_finite_automaton::is_empty()
{{{
	bool ret;
	// libAMoRE-1.0 has empty_full_lan(), but it requires
	// a minimized DFA as input
	get_sample_word(ret);
	return ret;
}}}

std::set<int> deterministic_finite_automaton::get_initial_states()
{{{
	std::set<int> ret;
	ret.insert(dfa_p->init);
	return ret;
}}}

std::set<int> deterministic_finite_automaton::get_final_states()
{{{
	std::set<int> ret;
	for(unsigned int i = 0; i <= dfa_p->highest_state; i++)
		if(dfa_p->final[i])
			ret.insert(i);
	return ret;
}}}

void deterministic_finite_automaton::set_initial_states(std::set<int> &states)
{{{
	std::set<int>::iterator si;
	for(si = states.begin(); si != states.end(); si++)
		dfa_p->init = *si;
}}}
void deterministic_finite_automaton::set_final_states(std::set<int> &states)
{{{
	for(unsigned int s = 0; s <= dfa_p->highest_state; s++)
		if(states.find(s) != states.end())
			dfa_p->final[s] = TRUE;
		else
			dfa_p->final[s] = FALSE;
}}}

int deterministic_finite_automaton::get_alphabet_size()
{{{
	if(dfa_p)
		return dfa_p->alphabet_size;
	else
		return 0;
}}}

list<int> deterministic_finite_automaton::shortest_run(std::set<int> from, std::set<int> &to, bool &reachable)
{{{
	std::set<int>::iterator si;
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

bool deterministic_finite_automaton::is_reachable(std::set<int> &from, std::set<int> &to)
{{{
	bool reachable;
	shortest_run(from, to, reachable);
	return reachable;
}}}

list<int> deterministic_finite_automaton::get_sample_word(bool & is_empty)
{{{
	std::set<int> initial_states, final_states;
	list<int> ret;
	bool reachable;

	initial_states = get_initial_states();
	final_states = get_final_states();

	ret = shortest_run(initial_states, final_states, reachable);
	is_empty = !reachable;

	return ret;
}}}

bool deterministic_finite_automaton::operator==(finite_automaton &other)
// note: calling operator== will minimize() this and other
// FIXME: avoid this? efficient algorithm?
{{{
	bool ret;

	deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(other.determinize());
	}

	minimize();
	o_d->minimize();

	// equiv REQUIRES minimized DFAs ... (see libAMoRE-1.0 tests/testeBinary.c::equiv
	ret = equiv(this->dfa_p, o_d->dfa_p);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

bool deterministic_finite_automaton::lang_subset_of(finite_automaton &other)
{{{
	bool ret;

	deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(other.determinize());
	}

	ret = inclusion(this->dfa_p, o_d->dfa_p, true);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

bool deterministic_finite_automaton::lang_disjoint_to(finite_automaton &other)
{{{
	bool ret;

	deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(other.determinize());
	}

	ret = inclusion(this->dfa_p, o_d->dfa_p, false);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

std::set<int> deterministic_finite_automaton::transition(std::set<int> from, int label)
{{{
	std::set<int> ret;
	std::set<int>::iterator si;

	for(si = from.begin(); si != from.end(); si++)
		ret.insert(dfa_p->delta[label+1][*si]);

	return ret;
}}}

bool deterministic_finite_automaton::contains(list<int> &word)
{{{
	if(dfa_p) {
		std::set<int> states;
		std::set<int>::iterator si;

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

finite_automaton * deterministic_finite_automaton::lang_union(finite_automaton &other)
// libAMoRE says: alphabets need to be the same
{{{
	finite_automaton * n;
	finite_automaton * ret;

	n = this->nondeterminize();	// 1st (2)
	ret = n->lang_union(other);
	delete n;

	return ret;
}}}

finite_automaton * deterministic_finite_automaton::lang_intersect(finite_automaton &other)
{{{
	deterministic_finite_automaton * ret;
	deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(other.determinize());
	}

	ret = new deterministic_finite_automaton(insecfa(dfa_p, o_d->dfa_p, false));

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

deterministic_finite_automaton * deterministic_finite_automaton::lang_difference(finite_automaton &other)
{{{
	deterministic_finite_automaton * ret;
	deterministic_finite_automaton * o_d;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(other.determinize());
	}

	ret = new deterministic_finite_automaton(insecfa(dfa_p, o_d->dfa_p, true));	// XXX 2nd (2)

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

finite_automaton * deterministic_finite_automaton::lang_symmetric_difference(finite_automaton &other)
{{{
	// return L1\L2 + L2\L1
	finite_automaton * L1_without_L2;
	finite_automaton * L2_without_L1;
	finite_automaton * ret = NULL;

	L1_without_L2 = lang_difference(other);			// XXX 2nd (1)
	L2_without_L1 = other.lang_difference(*this);

	ret = L1_without_L2->lang_union(*L2_without_L1);	// XXX 1st (1)

	delete L1_without_L2;
	delete L2_without_L1;

	return ret;
}}}

finite_automaton * deterministic_finite_automaton::lang_concat(finite_automaton &other)
{{{
	finite_automaton * ret;
	finite_automaton * n;

	n = this->nondeterminize();
	ret = n->lang_concat(other);
	delete n;

	return ret;
}}}

std::basic_string<int32_t> deterministic_finite_automaton::serialize()
{{{
	basic_string<int32_t> ret;
	basic_string<int32_t> temp;

	if(!dfa_p)
		return ret; // empty basic_string

	unsigned int s; // state id
	unsigned int l; // label only unsigned here because -1 == epsilon will not occur in dfa!

	// stream length; will be filled in later
	ret += 0;

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

	ret += htonl(temp.length() / 3);
	ret += temp;

	ret[0] = htonl(ret.length() - 1);

	return ret;
}}}

bool deterministic_finite_automaton::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
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

	// alphabet size
	dfa_p->alphabet_size = ntohl(*it);

	// state count
	size--, it++; if(size <= 0 || limit == it) goto dfaa_deserialization_failed;
	dfa_p->highest_state = ntohl(*it) - 1;

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

bool deterministic_finite_automaton::construct(int alphabet_size, int state_count, std::set<int> &initial, std::set<int> &final, multimap<pair<int, int>, int> &transitions)
{{{
	dfa a;
	std::set<int>::iterator si;
	multimap<pair<int, int>, int>::iterator ti,tj;

	// DO SOME SANITY CHECKS

	// - check if initial only contains one element
	if(initial.size() != 1) {
		// we could only create an NFA from this
		return false;
	}
	// - check if transitions don't contain nondeterministic choices
	for(ti = transitions.begin(); ti != transitions.end(); ti++) {
		if(transitions.count(ti->first) > 2) {
			tj = ti;
			for(++tj ; (tj != transitions.end()) && (ti->first == tj->first) ; ++tj)
				if(ti->second != tj->second)
					return false;
		}
	}

	// CONSTRUCT AUTOMATON
	a = newdfa();

	a->highest_state = state_count - 1; // states [0 .. highest_state]
	si = initial.begin();
	a->init = *si; // initial states
	a->alphabet_size = alphabet_size; // alphabet size
	a->final = newfinal(a->highest_state); // final states
	for(std::set<int>::iterator i = final.begin(); i != final.end(); i++)
		setfinal((a->final[*i]), 1);
	a->delta = newddelta(a->alphabet_size, a->highest_state); // transition function: delta[sigma][source] = destination
	// currently, delta[n] = 0. thus all undefined transitions would lead to
	// state 0. we don't want this, thus we reset the value here and add a
	// sink later on, if required.
	bool sink_required = false;
	for(unsigned int s = 0; s <= a->highest_state; s++)
		for(unsigned int i = 1; i <= a->alphabet_size; i++)
			a->delta[i][s] = a->highest_state+1;
	for(ti = transitions.begin(); ti != transitions.end(); ti++)
		a->delta[ti->first.second + 1][ti->first.first] = ti->second;
	for(unsigned int s = 0; s <= a->highest_state && !sink_required; s++)
		for(unsigned int i = 1; i <= a->alphabet_size && !sink_required; i++)
			if(a->delta[i][s] > a->highest_state)
				sink_required = true;
	if(sink_required) {
//		printf("AMoRE++ deterministic_finite_automaton::construct adding implicit sink\n");

		ddelta tmp;
		tmp = a->delta;

		// add one state
		a->highest_state++;
		// create new ddelta
		a->delta = newddelta(a->alphabet_size, a->highest_state);
		for(unsigned int s = 0; s <= a->highest_state; s++)
			for(unsigned int i = 1; i <= a->alphabet_size; i++)
				a->delta[i][s] = tmp[i][s];
		for(unsigned int i = 1; i <= a->alphabet_size; i++)
			a->delta[i][a->highest_state] = a->highest_state;
		// remove old ddelta
		for(unsigned int i = 1; i <= a->alphabet_size; i++)
			dispose(tmp[i]);
		dispose(tmp);
	}

	a->minimal = FALSE;

	if(dfa_p) {
		freedfa(dfa_p);
		free(dfa_p);
	}
	dfa_p = a;

	return true;
}}}

bool deterministic_finite_automaton::is_deterministic()
{ return true; };

deterministic_finite_automaton * deterministic_finite_automaton::determinize()
{{{
	return this->clone();
}}}

finite_automaton * deterministic_finite_automaton::nondeterminize()
{{{
	nondeterministic_finite_automaton *a;
	a = new nondeterministic_finite_automaton(dfa2nfa(dfa_p));	// XXX 1st (3)
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

} // end namespace amore

