/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
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

// for htonl / ntohl
# include <arpa/inet.h>

# include "libalf/automata.h"
# include "libalf/automata_amore.h"

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

// attention: stupid amore headers typedef string to be char*
// thus we have to use "std::string"...

namespace libalf {

using namespace std;

// implementation notes:
//
// libAMoRE is using '0' as epsilon, thus in amore, he alphabet is [1 .. size]
// and not [0 .. size-1]
//
// libalf uses (in construct) any int >= alphabet_size to indicate an epsilon
// transition and uses [0 .. size-1] as the alphabet.


deterministic_finite_amore_automaton::deterministic_finite_amore_automaton()
{{{
	dfa_p = NULL;
}}}
nondeterministic_finite_amore_automaton::nondeterministic_finite_amore_automaton()
{{{
	nfa_p = NULL;
}}}

deterministic_finite_amore_automaton::deterministic_finite_amore_automaton(dfa a)
{{{
	dfa_p = a;
}}}
nondeterministic_finite_amore_automaton::nondeterministic_finite_amore_automaton(nfa a)
{{{
	nfa_p = a;
}}}

static void amore_insanitize_regex(char* regex)
{{{
	while(*regex) {
		if(*regex == '|')
			*regex = 'U';
		regex++;
	}
}}}

nondeterministic_finite_amore_automaton::nondeterministic_finite_amore_automaton(char *rex, bool &success)
{{{
	char *p;
	char c = 'a';
	int alphabet_size;
	regex r;
	char *local_rex = strdup(rex);

	// calculate alphabet size
	p = rex;
	while(*p) {
		if(*p > c && *p < 'z')
			c = *p;
		p++;
	}
	alphabet_size = 1 + c - 'a';

	// transform rex to nfa
	amore_insanitize_regex(local_rex);
	r = rexFromString(alphabet_size, local_rex);
	free(local_rex);

	if(!r) {
		success = false;
		nfa_p = NULL;
	} else {
		nfa_p = rex2nfa(r);
		if(!nfa_p) {
			success = false;
		} else {
			freerex(r);
			success = true;
		}
	}
}}}
nondeterministic_finite_amore_automaton::nondeterministic_finite_amore_automaton(int alphabet_size, char *rex, bool &success)
{{{
	regex r;
	char *local_rex = strdup(rex);

	amore_insanitize_regex(local_rex);
	r = rexFromString(alphabet_size, local_rex);
	free(local_rex);

	if(!r) {
		success = false;
		nfa_p = NULL;
	} else {
		nfa_p = rex2nfa(r);
		if(!nfa_p) {
			success = false;
		} else {
			freerex(r);
			success = true;
		}
	}
}}}

deterministic_finite_amore_automaton::~deterministic_finite_amore_automaton()
{{{
	if(dfa_p)
		freedfa(dfa_p);
}}}
nondeterministic_finite_amore_automaton::~nondeterministic_finite_amore_automaton()
{{{
	if(nfa_p)
		freenfa(nfa_p);
}}}

deterministic_finite_amore_automaton * deterministic_finite_amore_automaton::clone()
{{{
	if(dfa_p)
		return new deterministic_finite_amore_automaton(clonedfa(dfa_p));
	else
		return new deterministic_finite_amore_automaton();
}}}
nondeterministic_finite_amore_automaton * nondeterministic_finite_amore_automaton::clone()
{{{
	if(nfa_p)
		return new nondeterministic_finite_amore_automaton(clonenfa(nfa_p));
	else
		return new nondeterministic_finite_amore_automaton();
}}}

int deterministic_finite_amore_automaton::get_state_count()
{{{
	if(dfa_p)
		return dfa_p->qno + 1;
	else
		return 0;
}}}
int nondeterministic_finite_amore_automaton::get_state_count()
{{{
	if(nfa_p)
		return nfa_p->qno + 1;
	else
		return 0;
}}}

bool deterministic_finite_amore_automaton::is_empty()
// note: calling minimize()es this
{{{
	bool ret;

	dfa empty = newdfa();
	empty->qno = 0;
	empty->init = 0;
	empty->sno = dfa_p->sno;
	empty->final = newfinal(empty->qno);
	empty->delta = newddelta(empty->sno, empty->qno);

	minimize();

	ret = equiv(dfa_p, empty);

	freedfa(empty);

	return ret;
}}}
bool nondeterministic_finite_amore_automaton::is_empty()
{{{
	bool ret;
	dfa empty = newdfa();
	dfa det;

	det = nfa2dfa(nfa_p);
	det = dfamdfa(det, true);

	ret = equiv(det, empty);

	freedfa(empty);
	freedfa(det);

	return ret;
}}}

int deterministic_finite_amore_automaton::get_alphabet_size()
{{{
	if(dfa_p)
		return dfa_p->sno;
	else
		return 0;
}}}
int nondeterministic_finite_amore_automaton::get_alphabet_size()
{{{
	if(nfa_p)
		return nfa_p->sno;
	else
		return 0;
}}}

list<int> deterministic_finite_amore_automaton::get_sample_word(bool & is_empty)
{{{
	set<int> visited_states;
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
			for(unsigned int i = 0; i < dfa_p->sno; i++) {
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
list<int> nondeterministic_finite_amore_automaton::get_sample_word(bool & is_empty)
{
	automaton_run current, next;

	queue<automaton_run> runs;

	set<automaton_run, automaton_run_less> next_runs;
	set<automaton_run, automaton_run_less>::iterator ri;

	int s; // state

	// init 
	for(s = 0; s <= nfa_p->qno; s++) {
		if(isinit(nfa_p->infin[s])) {
			current.state = s;
			runs.push(s);
		}
	}

}

bool deterministic_finite_amore_automaton::operator==(finite_language_automaton &other)
// note: calling operator== will minimize() this and other
// possibly avoid this?
{{{
	bool ret;

	deterministic_finite_amore_automaton * o_d;
	nondeterministic_finite_amore_automaton * o_n;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_amore_automaton*> (&other);

	if(!o_d) {
		o_n = dynamic_cast<nondeterministic_finite_amore_automaton*> (&other);
		if(!o_n) {
			// FIXME: non-compatible automaton
			// should throw exception
			return false;
		}

		// determinize
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_amore_automaton*>(o_n->determinize());
	}

	minimize();
	o_d->minimize();

	ret = equiv(this->dfa_p, o_d->dfa_p);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}
bool nondeterministic_finite_amore_automaton::operator==(finite_language_automaton &other)
{{{
	bool ret;

	deterministic_finite_amore_automaton * o_d;
	nondeterministic_finite_amore_automaton * o_n;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_amore_automaton*> (&other);

	if(!o_d) {
		o_n = dynamic_cast<nondeterministic_finite_amore_automaton*> (&other);
		if(!o_n) {
			// FIXME: non-compatible automaton
			// should throw exception
			return false;
		}

		// determinize
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_amore_automaton*>(o_n->determinize());
	}

	ret = ((*o_d) == *this);

	if(had_to_determinize) {
		delete o_d;
	}

	return ret;
}}}

bool deterministic_finite_amore_automaton::includes(finite_language_automaton &subautomaton)
{
	// -> amore::inclusion
}
bool nondeterministic_finite_amore_automaton::includes(finite_language_automaton &subautomaton)
{
	// -> amore::inclusion
}

void nondeterministic_finite_amore_automaton::epsilon_closure(set<int> & states)
{{{
	if(nfa_p->is_eps == FALSE)
		return;

	queue<int> new_states;
	set<int>::iterator sti;

	int current;

	for(sti = states.begin(); sti != states.end(); sti++)
		new_states.push(*sti);

	while(!new_states.empty()) {
		// find all new epsilon states
		current = new_states.front();
		new_states.pop();

		for(unsigned int s = 0; s <= nfa_p->qno; s++) // state
			if(testcon((nfa_p->delta), 0, current, s))
				if(states.find(s) == states.end()) {
					states.insert(s);
					new_states.push(s);
				};
	};
}}}
void nondeterministic_finite_amore_automaton::epsilon_closure(set<automaton_run, automaton_run_less> runs)
{{{
	if(nfa_p->is_eps == FALSE)
		return;

	queue<automaton_run> new_runs;

	set<automaton_run, automaton_run_less>::iterator ri;

	automaton_run current, ns;

	for(ri = runs.begin(); ri != runs.end(); ri++)
		new_runs.push(*ri);

	while(!new_runs.empty()) {
		current = new_runs.front();
		new_runs.pop();

		for(unsigned int s = 0; s <= nfa_p->qno; s++) // state
			if(testcon((nfa_p->delta), 0, current.state, s)) {
				ns.prefix = current.prefix;
				ns.state = s;
				if(runs.find(ns) == runs.end()) {
					runs.insert(ns);
					new_runs.push(ns);
				};
			};
	};
}}}


bool deterministic_finite_amore_automaton::accepts_suffix(int starting_state, list<int>::iterator suffix_begin, list<int>::iterator suffix_end)
{{{
	if(suffix_begin == suffix_end) {
		return (TRUE == dfa_p->final[starting_state]);
	} else {
		unsigned int l = (*suffix_begin);
		if(l >= dfa_p->sno)
			return false;
		suffix_begin++;
		return accepts_suffix(dfa_p->delta[l+1][starting_state], suffix_begin, suffix_end);
	}
}}}
bool nondeterministic_finite_amore_automaton::accepts_suffix(set<int> &starting_states, list<int>::iterator suffix_begin, list<int>::iterator suffix_end)
{{{
	set<int>::iterator sti;

	this->epsilon_closure(starting_states);

	// not accepting if there are no states
	if(starting_states.empty())
		return false;

	if(suffix_begin == suffix_end) {
		// check existence of at least one accepting state
		for(sti = starting_states.begin(); sti != starting_states.end(); sti++)
			if(isfinal(nfa_p->infin[*sti]))
				return true;

		return false;
	} else {
		set<int> next_states;

		unsigned int l = *suffix_begin;
		if(l >= nfa_p->sno)
			return false;

		// calculate successor states
		for(sti = starting_states.begin(); sti != starting_states.end(); sti++)
			for(unsigned int d = 0; d <= nfa_p->qno; d++)
				if(testcon((nfa_p->delta), l+1, *sti, d))
					next_states.insert(d);

		suffix_begin++;
		return accepts_suffix(next_states, suffix_begin, suffix_end);
	}
}}}

bool deterministic_finite_amore_automaton::contains(list<int> &word)
{{{
	if(dfa_p) {
		return accepts_suffix(dfa_p->init, word.begin(), word.end());
	} else {
		return false;
	}
}}}
bool nondeterministic_finite_amore_automaton::contains(list<int> &word)
{{{
	if(nfa_p) {
		set<int> initial_states;
		for(unsigned int s = 0; s < nfa_p->qno; s++)
			if(isinit(nfa_p->infin[s]))
				initial_states.insert(s);
		return accepts_suffix(initial_states, word.begin(), word.end());
	} else {
		return false;
	}
}}}

void deterministic_finite_amore_automaton::minimize()
{{{
	dfa_p = dfamdfa(dfa_p, true);
}}}
void nondeterministic_finite_amore_automaton::minimize()
{
	// nfa2mnfa ?!
}

void deterministic_finite_amore_automaton::lang_complement()
{{{
	dfa a;

	a = compldfa(dfa_p);
	freedfa(dfa_p);
	dfa_p = a;
}}}
void nondeterministic_finite_amore_automaton::lang_complement()
{{{
	dfa a,b;

	a = nfa2dfa(nfa_p);
	b = compldfa(a);
	freedfa(a);
	freenfa(nfa_p);
	nfa_p = dfa2nfa(b);
	freedfa(b);
}}}

nondeterministic_finite_automaton * deterministic_finite_amore_automaton::lang_union(finite_language_automaton &other)
{{{
	nondeterministic_finite_automaton * ret;
	deterministic_finite_amore_automaton * o_d;
	nondeterministic_finite_amore_automaton * o_n;
	bool had_to_nfa = false;

	o_n = dynamic_cast<nondeterministic_finite_amore_automaton*> (&other);

	if(!o_n){
		o_d = dynamic_cast<deterministic_finite_amore_automaton*> (&other);
		if(!o_d) {
			// FIXME: non-compatible automaton
			// should throw exception
			return NULL;
		}

		had_to_nfa = true;
		o_n = dynamic_cast<nondeterministic_finite_amore_automaton*>(o_d->nondeterminize());
	}

	ret = o_n->lang_union(*this);

	if(had_to_nfa)
		delete o_n;

	return ret;
}}}
nondeterministic_finite_automaton * nondeterministic_finite_amore_automaton::lang_union(finite_language_automaton &other)
{{{
	nondeterministic_finite_automaton * ret;
	deterministic_finite_amore_automaton * o_d;
	nondeterministic_finite_amore_automaton *o_n;
	bool had_to_nfa = false;

	o_n = dynamic_cast<nondeterministic_finite_amore_automaton*> (&other);

	if(!o_n) {
		o_d = dynamic_cast<deterministic_finite_amore_automaton*> (&other);
		if(!o_d) {
			// FIXME: non-compatible automaton
			// should throw exception
			return NULL;
		}

		had_to_nfa = true;
		o_n = dynamic_cast<nondeterministic_finite_amore_automaton*>(o_d->nondeterminize());
	}

	nfa a;

	// (AMoRE says:) alphabets need to be the same
	a = unionfa(nfa_p, o_n->nfa_p);
	ret = new nondeterministic_finite_amore_automaton(a);

	if(had_to_nfa)
		delete o_n;

	return ret;
}}}

finite_language_automaton * deterministic_finite_amore_automaton::lang_intersect(finite_language_automaton &other)
{
}
finite_language_automaton * nondeterministic_finite_amore_automaton::lang_intersect(finite_language_automaton &other)
{
}

nondeterministic_finite_automaton * deterministic_finite_amore_automaton::lang_difference(finite_language_automaton &other)
{{{
	// return L1\L2 + L2\L1
	deterministic_finite_amore_automaton * L1_without_L2;
	finite_language_automaton * L2_without_L1;
	nondeterministic_finite_automaton * ret = NULL;

	L1_without_L2 = lang_without(other);
	L2_without_L1 = other.lang_without(*this);

	ret = L1_without_L2->lang_union(*L2_without_L1);

	delete L1_without_L2;
	delete L2_without_L1;

	return ret;
}}}
nondeterministic_finite_automaton * nondeterministic_finite_amore_automaton::lang_difference(finite_language_automaton &other)
{
}

deterministic_finite_amore_automaton * deterministic_finite_amore_automaton::lang_without(finite_language_automaton &other)
{{{
	deterministic_finite_amore_automaton *ret = NULL;

	deterministic_finite_amore_automaton * o_d;
	nondeterministic_finite_amore_automaton * o_n;
	bool had_to_determinize = false;

	o_d = dynamic_cast<deterministic_finite_amore_automaton*> (&other);

	if(!o_d) {
		o_n = dynamic_cast<nondeterministic_finite_amore_automaton*> (&other);
		if(!o_n) {
			// FIXME: non-compatible automaton
			// should throw exception
			return NULL;
		}

		// determinize
		had_to_determinize = true;
		o_d = dynamic_cast<deterministic_finite_amore_automaton*>(o_n->determinize());
	}

	ret = new deterministic_finite_amore_automaton(insecfa(dfa_p, o_d->dfa_p, true));

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}
deterministic_finite_amore_automaton * nondeterministic_finite_amore_automaton::lang_without(finite_language_automaton &other)
{
	printf("nfaa::lang_without() not implemented\n");
}

finite_language_automaton * deterministic_finite_amore_automaton::lang_concat(finite_language_automaton &other)
{
/*
	automata_amore *o = dynamic_cast<automata_amore*> (&other);
	// other has to be an automata_amore
	if(o) {
		automata_amore *a = new automata_amore();

		make_undeterministic();
		o->make_undeterministic();

		a->set_nfa( concatfa(nfa_p, o->nfa_p) );

		return a;
	} else {
		// FIXME: throw exception?
		return NULL;
	}
*/
}
finite_language_automaton * nondeterministic_finite_amore_automaton::lang_concat(finite_language_automaton &other)
{
/*
	automata_amore *o = dynamic_cast<automata_amore*> (&other);
	// other has to be an automata_amore
	if(o) {
		automata_amore *a = new automata_amore();

		make_undeterministic();
		o->make_undeterministic();

		a->set_nfa( concatfa(nfa_p, o->nfa_p) );

		return a;
	} else {
		// FIXME: throw exception?
		return NULL;
	}
*/
}

std::basic_string<int32_t> deterministic_finite_amore_automaton::serialize()
{{{
	basic_string<int32_t> ret;
	basic_string<int32_t> temp;

	if(!dfa_p)
		return ret; // empty basic_string

	unsigned int s; // state id
	unsigned int l; // label only unsigned here because -1 == epsilon will not occur in dfa!

	// alphabet size
	ret += htonl(dfa_p->sno);
	// state count
	ret += htonl(dfa_p->qno+1);
	// number of initial states
	ret += htonl(1);
	// initial state
	ret += htonl(dfa_p->init);
	// sum up final states
	for(s = 0; s <= dfa_p->qno; s++)
		if(dfa_p->final[s] == TRUE)
			temp += htonl(s);
	// number of final states
	ret += htonl(temp.length());
	// final states
	ret += temp;
	// transitions
	temp.clear();
	for(l = 0; l < dfa_p->sno; l++) {
		for(s = 0; s <= dfa_p->qno; s++) {
			temp += htonl(s); // source state id
			temp += htonl(l); // label
			temp += htonl(dfa_p->delta[l+1][s]); // destination
		}
	}

	ret += htonl(temp.length() / 3);
	ret += temp;

	return ret;
}}}
std::basic_string<int32_t> nondeterministic_finite_amore_automaton::serialize()
{{{
	basic_string<int32_t> ret;
	basic_string<int32_t> temp;
	unsigned int s;
	int l;

	if(!nfa_p) {
		return ret; // empty basic_string
	}
	// alphabet size
	ret += htonl(nfa_p->sno);
	// state count
	ret += htonl(nfa_p->qno+1);

	for(s = 0; s <= nfa_p->qno; s++)
		if(isinit(nfa_p->infin[s]))
			temp += htonl(s);
	// number of initial states
	ret += htonl(temp.length());
	// initial states
	ret += temp;

	temp.clear();
	for(s = 0; s <= nfa_p->qno; s++)
		if(isfinal(nfa_p->infin[s]))
			temp += htonl(s);
	// number of final states
	ret += htonl(temp.length());
	// final states
	ret += temp;

	temp.clear();
	for(l = (nfa_p->is_eps == TRUE) ? 0 : 1; l <= ((int)nfa_p->sno); l++) { // label
		for(s = 0; s <= nfa_p->qno; s++) { // source state id
			for(unsigned int d = 0; d <= nfa_p->qno; d++) {
				// boolx testcon(delta, label, src, dst); in libAMoRE
				if(testcon((nfa_p->delta), l, s, d)) {
					temp += htonl(s);
					temp += htonl(l-1);
					temp += htonl(d);
				}
			}
		}
	}
	// number of transitions
	ret += htonl(temp.length() / 3);
	// transitions
	ret += temp;

	return ret;
}}}

bool deterministic_finite_amore_automaton::deserialize(basic_string<int32_t> &automaton)
{{{
	int s, count;
	basic_string<int32_t>::iterator si, end;

	if(dfa_p)
		freedfa(dfa_p);
	dfa_p = newdfa();

	if(automaton.size() < 6) // that's the shortest possible automaton
		goto dfaa_deserialization_failed;

	si = automaton.begin();
	end = automaton.end();

	// alphabet size
	dfa_p->sno = ntohl(*si);

	// state count
	si++;
	dfa_p->qno = ntohl(*si) - 1;

	// allocate data structures
	dfa_p->final = newfinal(dfa_p->qno);
	dfa_p->delta = newddelta(dfa_p->sno, dfa_p->qno);
	if(!(dfa_p->final) || !(dfa_p->delta))
		goto dfaa_deserialization_failed;

	// initial state
	si++;
	if(ntohl(*si) != 1) // dfa only allows exactly one initial state
		goto dfaa_deserialization_failed;

	si++;
	dfa_p->init = ntohl(*si);

	// final states
	si++;
	count = ntohl(*si);

	si++;
	for(s = 0; s < count; s++) {
		if(si == end)
			goto dfaa_deserialization_failed;
		setfinal(dfa_p->final[ntohl(*si)],1);
		si++;
	}

	// transitions
	if(si == end)
		goto dfaa_deserialization_failed;
	count = ntohl(*si);
	si++;
	for(s = 0; s < count; s++) {
		int32_t src, label, dst;

		if(si == end)
			goto dfaa_deserialization_failed;
		src = ntohl(*si);
		si++;
		if(si == end)
			goto dfaa_deserialization_failed;
		label = ntohl(*si);
		si++;
		if(si == end)
			goto dfaa_deserialization_failed;
		dst = ntohl(*si);
		si++;

		if( (label < 0) || (label >= (int)dfa_p->sno) || (src < 0) || (src > (int)dfa_p->qno) || (dst < 0) || (dst > (int)dfa_p->qno) ) // invalid transition
			goto dfaa_deserialization_failed;

		// transition function: delta[sigma][source] = destination
		dfa_p->delta[label+1][src] = dst;
	}

	if(si != end) // remainder at end of string
		goto dfaa_deserialization_failed;

	return true;

dfaa_deserialization_failed:
	freedfa(dfa_p);
	dfa_p = NULL;
	return false;
}}}
bool nondeterministic_finite_amore_automaton::deserialize(basic_string<int32_t> &automaton)
{{{
	int s, count;
	basic_string<int32_t>::iterator si, end;

	if(nfa_p)
		freenfa(nfa_p);
	nfa_p = newnfa();

	if(automaton.size() < 6) // that's the shortest possible automaton
		goto nfaa_deserialization_failed;

	si = automaton.begin();
	end = automaton.end();

	// alphabet size
	nfa_p->sno = ntohl(*si);

	// state count
	si++;
	nfa_p->qno = ntohl(*si) - 1;

	// allocate data structures
	nfa_p->infin = newfinal(nfa_p->qno);
	nfa_p->delta = newendelta(nfa_p->sno, nfa_p->qno);
	nfa_p->is_eps = TRUE;
	if(!(nfa_p->infin) || !(nfa_p->delta))
		goto nfaa_deserialization_failed;

	// initial states
	si++;
	count = ntohl(*si);

	si++;
	for(s = 0; s < count; s++) {
		if(si == end)
			goto nfaa_deserialization_failed;
		setinit(nfa_p->infin[ntohl(*si)]);
		si++;
	}

	// final states
	if(si == end)
		goto nfaa_deserialization_failed;
	count = ntohl(*si);
	si++;

	for(s = 0; s < count; s++) {
		if(si == end)
			goto nfaa_deserialization_failed;
		setfinalT(nfa_p->infin[ntohl(*si)]);
		si++;
	}

	// transitions
	if(si == end)
		goto nfaa_deserialization_failed;
	count = ntohl(*si);
	si++;
	for(s = 0; s < count; s++) {
		int32_t src, label, dst;

		if(si == end)
			goto nfaa_deserialization_failed;
		src = ntohl(*si);
		si++;
		if(si == end)
			goto nfaa_deserialization_failed;
		label = ntohl(*si);
		si++;
		if(si == end)
			goto nfaa_deserialization_failed;
		dst = ntohl(*si);
		si++;

		if( (label < -1) || (label >= (int)nfa_p->sno) || (src < 0) || (src > (int)nfa_p->qno) || (dst < 0) || (dst > (int)nfa_p->qno) ) // invalid transition
			goto nfaa_deserialization_failed;

		connect(nfa_p->delta, label+1, src, dst);
	}

	if(si != end) // remainder at end of string
		goto nfaa_deserialization_failed;

	return true;

nfaa_deserialization_failed:
	freenfa(nfa_p);
	nfa_p = NULL;
	return false;
}}}

bool deterministic_finite_amore_automaton::construct(int alphabet_size, int state_count, list<int> start, list<int> final, list<transition> transitions)
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
				// we could only create and NFA from this
				return false;
			}
		}
	}

	// CONSTRUCT AUTOMATON
	a = newdfa();

	a->qno = state_count - 1; // states [0 .. qno]
	a->init = start.front(); // initial states
	a->sno = alphabet_size; // alphabet size
	a->final = newfinal(a->qno); // final states
	for(list<int>::iterator i = final.begin(); i != final.end(); i++)
		setfinal((a->final[*i]), 1);
	a->delta = newddelta(a->sno, a->qno); // transition function: delta[sigma][source] = destination
	for(ti = transitions.begin(); ti != transitions.end(); ti++)
		a->delta[ti->label + 1][ti->source] = ti->destination;
	a->minimal = FALSE;

	if(dfa_p)
		freedfa(dfa_p);
	dfa_p = a;

	return true;
}}}

deterministic_finite_automaton * deterministic_finite_amore_automaton::determinize()
{{{
	return this->clone();
}}}
nondeterministic_finite_automaton * deterministic_finite_amore_automaton::nondeterminize()
{{{
	return new nondeterministic_finite_amore_automaton(dfa2nfa(dfa_p));
}}}
deterministic_finite_automaton * nondeterministic_finite_amore_automaton::determinize()
{{{
	return new deterministic_finite_amore_automaton(nfa2dfa(nfa_p));
}}}
nondeterministic_finite_automaton * nondeterministic_finite_amore_automaton::nondeterminize()
{{{
	return this->clone();
}}}

void deterministic_finite_amore_automaton::set_dfa(dfa a)
{{{
	if(dfa_p)
		freedfa(dfa_p);
	dfa_p = clonedfa(a);
}}}
void nondeterministic_finite_amore_automaton::set_nfa(nfa a)
{{{
	if(nfa_p)
		freenfa(nfa_p);
	nfa_p = clonenfa(a);
}}}

dfa deterministic_finite_amore_automaton::get_dfa()
{{{
	return dfa_p;
}}}
nfa nondeterministic_finite_amore_automaton::get_nfa()
{{{
	return nfa_p;
}}}

} // end namespace libalf

