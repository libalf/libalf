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
// libalf uses (in construct) any int >= alphabet_size to indicate an epsilon
// transition and uses [0 .. size-1] as the alphabet.


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
	if(dfa_p)
		freedfa(dfa_p);
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
	get_sample_word(ret);
	return ret;
}}}

int deterministic_finite_automaton::get_alphabet_size()
{{{
	if(dfa_p)
		return dfa_p->alphabet_size;
	else
		return 0;
}}}

list<int> deterministic_finite_automaton::get_sample_word(bool & is_empty)
{{{
	std::set<int> visited_states;
	queue<automaton_run> state_fifo;
	automaton_run current;

	current.state = dfa_p->init;

	state_fifo.push(current);

	while(!state_fifo.empty()) {
		current = state_fifo.front();
		state_fifo.pop();

		// if state was visited before, skip it
		if(visited_states.find(current.state) == visited_states.end()) {
			visited_states.insert(current.state);

			// if state is final, return its prefix
			if(dfa_p->final[current.state] == TRUE) {
				is_empty = false;
				return current.prefix;
			}

			// otherwise check all possible successors
			int st = current.state;
			for(unsigned int i = 0; i < dfa_p->alphabet_size; i++) {
				// add possible successor with its prefix to fifo
				current.prefix.push_back(i);
				current.state = dfa_p->delta[i+1][st];
				state_fifo.push(current);
				current.prefix.pop_back();
			}
		}
	}

	is_empty = true;
	list<int> empty;
	return empty;
}}}

bool deterministic_finite_automaton::operator==(finite_automaton &other)
// note: calling operator== will minimize() this and other
// possibly avoid this?
{{{
	bool ret;

	deterministic_finite_automaton * o_d;
	finite_automaton * o_n;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		o_n = dynamic_cast<finite_automaton*> (&other);
		if(!o_n) {
			// FIXME: non-compatible automaton
			// should throw exception
			return false;
		}

		// determinize
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(o_n->determinize());
	}

	minimize();
	o_d->minimize();

	ret = equiv(this->dfa_p, o_d->dfa_p);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

bool deterministic_finite_automaton::lang_subset_of(finite_automaton &other)
{{{
	bool ret;

	deterministic_finite_automaton * o_d;
	finite_automaton * o_n;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		o_n = dynamic_cast<finite_automaton*> (&other);
		if(!o_n) {
			// FIXME: non-compatible automaton
			// should throw exception
			return false;
		}

		// determinize
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(o_n->determinize());
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
	finite_automaton * o_n;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		o_n = dynamic_cast<finite_automaton*> (&other);
		if(!o_n) {
			// FIXME: non-compatible automaton
			// should throw exception
			return false;
		}

		// determinize
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(o_n->determinize());
	}

	ret = inclusion(this->dfa_p, o_d->dfa_p, false);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

bool deterministic_finite_automaton::accepts_suffix(int starting_state, list<int>::iterator suffix_begin, list<int>::iterator suffix_end)
{{{
	if(suffix_begin == suffix_end) {
		return (TRUE == dfa_p->final[starting_state]);
	} else {
		unsigned int l = (*suffix_begin);
		if(l >= dfa_p->alphabet_size)
			return false;
		suffix_begin++;
		return accepts_suffix(dfa_p->delta[l+1][starting_state], suffix_begin, suffix_end);
	}
}}}

bool deterministic_finite_automaton::contains(list<int> &word)
{{{
	if(dfa_p) {
		return accepts_suffix(dfa_p->init, word.begin(), word.end());
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
	dfa_p = a;
}}}

finite_automaton * deterministic_finite_automaton::lang_union(finite_automaton &other)
{{{
	finite_automaton * ret;
	deterministic_finite_automaton * o_d;
	finite_automaton * o_n;
	bool had_to_nfa = false;

	o_n = dynamic_cast<finite_automaton*> (&other);

	if(!o_n){
		o_d = dynamic_cast<deterministic_finite_automaton*> (&other);
		if(!o_d) {
			// FIXME: non-compatible automaton
			// should throw exception
			return NULL;
		}

		had_to_nfa = true;
		o_n = dynamic_cast<finite_automaton*>(o_d->nondeterminize());
	}

	ret = o_n->lang_union(*this);

	if(had_to_nfa)
		delete o_n;

	return ret;
}}}

finite_automaton * deterministic_finite_automaton::lang_intersect(finite_automaton &other)
{{{
	deterministic_finite_automaton *ret = NULL;

	deterministic_finite_automaton * o_d;
	finite_automaton * o_n;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		o_n = dynamic_cast<finite_automaton*> (&other);
		if(!o_n) {
			// FIXME: non-compatible automaton
			// should throw exception
			return NULL;
		}

		// determinize
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(o_n->determinize());
	}

	ret = new deterministic_finite_automaton(insecfa(dfa_p, o_d->dfa_p, false));

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

deterministic_finite_automaton * deterministic_finite_automaton::lang_difference(finite_automaton &other)
{{{
	deterministic_finite_automaton *ret = NULL;

	deterministic_finite_automaton * o_d;
	finite_automaton * o_n;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_automaton*> (&other);

	if(!o_d) {
		o_n = dynamic_cast<finite_automaton*> (&other);
		if(!o_n) {
			// FIXME: non-compatible automaton
			// should throw exception
			return NULL;
		}

		// determinize
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_automaton*>(o_n->determinize());
	}

	ret = new deterministic_finite_automaton(insecfa(dfa_p, o_d->dfa_p, true));

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

finite_automaton * deterministic_finite_automaton::lang_symmetric_difference(finite_automaton &other)
{{{
	// return L1\L2 + L2\L1
	deterministic_finite_automaton * L1_without_L2;
	finite_automaton * L2_without_L1;
	finite_automaton * ret = NULL;

	L1_without_L2 = lang_difference(other);
	L2_without_L1 = other.lang_difference(*this);

	ret = L1_without_L2->lang_union(*L2_without_L1);

	delete L1_without_L2;
	delete L2_without_L1;

	return ret;
}}}

finite_automaton * deterministic_finite_automaton::lang_concat(finite_automaton &other)
{{{
	finite_automaton * ret;

	deterministic_finite_automaton * o_d;
	finite_automaton * o_n;
	finite_automaton * t_n;

	bool had_to_nondeterminize = false;

	o_n = dynamic_cast<finite_automaton*> (&other);

	if(!o_n) {
		o_d = dynamic_cast<deterministic_finite_automaton*> (&other);
		if(!o_d) {
			// FIXME: non-compatible automaton
			// should throw exception
			return NULL;
		}

		// nondeterminize
		had_to_nondeterminize = true;
		o_n = dynamic_cast<finite_automaton*> (o_n->nondeterminize());
	}

	t_n = this->nondeterminize();
	ret = dynamic_cast<finite_automaton*>(t_n->lang_concat(*o_n));
	delete t_n;

	if(had_to_nondeterminize)
		delete o_n;

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

	if(it == limit)
		goto dfaa_deserialization_failed;

	// string length (excluding length field)
	size = ntohl(*it);
	it++;
	if(size <= 0 || limit == it) goto dfaa_deserialization_failed;

	if(dfa_p)
		freedfa(dfa_p);
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

	size--, it++;

	if(size != 0)
		goto dfaa_deserialization_failed;

	return true;

dfaa_deserialization_failed:
	freedfa(dfa_p);
	dfa_p = NULL;
	return false;
}}}

bool deterministic_finite_automaton::construct(int alphabet_size, int state_count, list<int> &start, list<int> &final, list<transition> &transitions)
{{{
	dfa a;
	list<transition>::iterator ti, tj;

	// DO SOME SANITY CHECKS

	// - check if start only contains one element
	if(start.size() != 1) {
		// we could only create an NFA from this
		return false;
	}
	// - check if transitions don't contain duplicate source,sigma tuples
	for(ti = transitions.begin(); ti != transitions.end(); ti++) {
		tj = ti;
		for(tj++; tj != transitions.end(); tj++) {
			if(*ti << *tj) {
				// we could only create an NFA from this
				return false;
			}
		}
	}

	// CONSTRUCT AUTOMATON
	a = newdfa();

	a->highest_state = state_count - 1; // states [0 .. highest_state]
	a->init = start.front(); // initial states
	a->alphabet_size = alphabet_size; // alphabet size
	a->final = newfinal(a->highest_state); // final states
	for(list<int>::iterator i = final.begin(); i != final.end(); i++)
		setfinal((a->final[*i]), 1);
	a->delta = newddelta(a->alphabet_size, a->highest_state); // transition function: delta[sigma][source] = destination
	for(ti = transitions.begin(); ti != transitions.end(); ti++)
		a->delta[ti->label + 1][ti->source] = ti->destination;
	a->minimal = FALSE;

	if(dfa_p)
		freedfa(dfa_p);
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
	a = new nondeterministic_finite_automaton(dfa2nfa(dfa_p));
	return a;
}}}

void deterministic_finite_automaton::set_dfa(dfa a)
{{{
	if(dfa_p)
		freedfa(dfa_p);
	dfa_p = clonedfa(a);
}}}

dfa deterministic_finite_automaton::get_dfa()
{{{
	return dfa_p;
}}}

} // end namespace amore

