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

#include <ostream>
#include <fstream>

// for htonl / ntohl
# include <arpa/inet.h>

# define LIBALF_LIBRARY_COMPILATION
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
# include <amore/nfa2mnfa.h>

// attention: stupid amore headers typedef string to be char*
// thus we have to use "std::string"...
// nfa2mnfa.h defines some 'set', so we have to use "std::set"

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
		switch(*regex) {
			case '|':
				*regex = 'U';
				break;
			default:
				break;
		}
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
		freerex(r);
		if(!nfa_p) {
			success = false;
		} else {
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
		return dfa_p->highest_state + 1;
	else
		return 0;
}}}
int nondeterministic_finite_amore_automaton::get_state_count()
{{{
	if(nfa_p)
		return nfa_p->highest_state + 1;
	else
		return 0;
}}}

bool deterministic_finite_amore_automaton::is_empty()
{{{
	bool ret;
	get_sample_word(ret);
	return ret;
}}}
bool nondeterministic_finite_amore_automaton::is_empty()
{{{
	bool ret;
	get_sample_word(ret);
	return ret;
}}}

int deterministic_finite_amore_automaton::get_alphabet_size()
{{{
	if(dfa_p)
		return dfa_p->alphabet_size;
	else
		return 0;
}}}
int nondeterministic_finite_amore_automaton::get_alphabet_size()
{{{
	if(nfa_p)
		return nfa_p->alphabet_size;
	else
		return 0;
}}}

list<int> deterministic_finite_amore_automaton::get_sample_word(bool & is_empty)
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
list<int> nondeterministic_finite_amore_automaton::get_sample_word(bool & is_empty)
{{{
	std::set<int> visited_states;
	list<automaton_run> run_fifo;
	list<automaton_run>::iterator ri;
	automaton_run current, next;
	unsigned int s, l;

	// put initial states into fifo
	for(s = 0; s <= nfa_p->highest_state; s++) {
		if(isinit(nfa_p->infin[s])) {
			current.state = s;
			// current.prefix is empty.
			run_fifo.push_back(current);
			visited_states.insert(s);
		}
	}

	while(!run_fifo.empty()) {
		// check final
		current = run_fifo.front();
		run_fifo.pop_front();

		if(isfinal(nfa_p->infin[current.state])) {
			is_empty = false;
			return current.prefix;
		}
		// epsilon-close
		if(nfa_p->is_eps == TRUE) {
			next.prefix = current.prefix;
			for(s = 0; s <= nfa_p->highest_state; s++) {
				if(testcon((nfa_p->delta), 0, current.state, s)) {
					if(visited_states.find(s) == visited_states.end()) {
						next.state = s;
						run_fifo.push_front(next);
						visited_states.insert(s);
					}
				}
			}
		}

		// advance to new states
		for(s = 0; s <= nfa_p->highest_state; s++) { // dst state
			if(visited_states.find(s) == visited_states.end()) {
				for(l = 0; l < nfa_p->alphabet_size; l++) { // label
					if(testcon((nfa_p->delta), l+1, current.state, s)) {
						next.prefix = current.prefix;
						next.prefix.push_back(l);
						next.state = s;
						run_fifo.push_back(next);
						visited_states.insert(s);
					}
				}
			}
		}
	}
	list<int> ret;
	is_empty = true;
	return ret; // empty word
}}}

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

bool deterministic_finite_amore_automaton::lang_subset_of(finite_language_automaton &other)
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

	ret = inclusion(this->dfa_p, o_d->dfa_p, true);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}
bool nondeterministic_finite_amore_automaton::lang_subset_of(finite_language_automaton &other)
{{{
	bool ret;

	deterministic_finite_amore_automaton * o_d;
	nondeterministic_finite_amore_automaton * o_n;
	deterministic_finite_automaton * t_d;

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
		o_d = dynamic_cast<deterministic_finite_amore_automaton*> (o_n->determinize());
	}

	t_d = this->determinize();
	ret = t_d->lang_subset_of(*o_d);
	delete t_d;

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

bool deterministic_finite_amore_automaton::lang_disjoint_to(finite_language_automaton &other)
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

	ret = inclusion(this->dfa_p, o_d->dfa_p, false);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}
bool nondeterministic_finite_amore_automaton::lang_disjoint_to(finite_language_automaton &other)
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
		o_d = dynamic_cast<deterministic_finite_amore_automaton*> (o_n->determinize());
	}

	ret = o_d->lang_disjoint_to(*this);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

void nondeterministic_finite_amore_automaton::epsilon_closure(std::set<int> & states)
{{{
	if(nfa_p->is_eps == FALSE)
		return;

	queue<int> new_states;
	std::set<int>::iterator sti;

	int current;

	for(sti = states.begin(); sti != states.end(); sti++)
		new_states.push(*sti);

	while(!new_states.empty()) {
		// find all new epsilon states
		current = new_states.front();
		new_states.pop();

		for(unsigned int s = 0; s <= nfa_p->highest_state; s++) // state
			if(testcon((nfa_p->delta), 0, current, s))
				if(states.find(s) == states.end()) {
					states.insert(s);
					new_states.push(s);
				};
	};
}}}


bool deterministic_finite_amore_automaton::accepts_suffix(int starting_state, list<int>::iterator suffix_begin, list<int>::iterator suffix_end)
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
bool nondeterministic_finite_amore_automaton::accepts_suffix(std::set<int> &starting_states, list<int>::iterator suffix_begin, list<int>::iterator suffix_end)
{{{
	std::set<int>::iterator sti;

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
		std::set<int> next_states;

		unsigned int l = *suffix_begin;
		if(l >= nfa_p->alphabet_size)
			return false;

		// calculate successor states
		for(sti = starting_states.begin(); sti != starting_states.end(); sti++)
			for(unsigned int d = 0; d <= nfa_p->highest_state; d++)
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
		std::set<int> initial_states;
		for(unsigned int s = 0; s < nfa_p->highest_state; s++)
			if(isinit(nfa_p->infin[s]))
				initial_states.insert(s);
		return accepts_suffix(initial_states, word.begin(), word.end());
	} else {
		return false;
	}
}}}

void deterministic_finite_amore_automaton::minimize()
{{{
	dfa_p = dfamdfa(dfa_p, TRUE);
}}}
void nondeterministic_finite_amore_automaton::minimize()
{{{
	dfa d;
	nfa n;

	d = nfa2dfa(nfa_p);
	d = dfamdfa(d, true);

	n = nfa2mnfa(nfa_p, d);

	freedfa(d);
	freenfa(nfa_p);

	nfa_p = n;
}}}

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

	ret = new deterministic_finite_amore_automaton(insecfa(dfa_p, o_d->dfa_p, false));

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}
finite_language_automaton * nondeterministic_finite_amore_automaton::lang_intersect(finite_language_automaton &other)
{{{
	deterministic_finite_amore_automaton * ret;

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
		o_d = dynamic_cast<deterministic_finite_amore_automaton*> (o_n->determinize());
	}

	ret = o_d->lang_difference(*this);

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

deterministic_finite_amore_automaton * deterministic_finite_amore_automaton::lang_difference(finite_language_automaton &other)
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
deterministic_finite_amore_automaton * nondeterministic_finite_amore_automaton::lang_difference(finite_language_automaton &other)
{{{
	deterministic_finite_amore_automaton * ret;

	deterministic_finite_amore_automaton * o_d;
	nondeterministic_finite_amore_automaton * o_n;
	deterministic_finite_amore_automaton * t_d;

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
		o_d = dynamic_cast<deterministic_finite_amore_automaton*> (o_n->determinize());
	}

	t_d = dynamic_cast<deterministic_finite_amore_automaton*>(this->determinize());
	ret = t_d->lang_difference(*o_d);
	delete t_d;

	if(had_to_determinize)
		delete o_d;

	return ret;
}}}

nondeterministic_finite_automaton * deterministic_finite_amore_automaton::lang_symmetric_difference(finite_language_automaton &other)
{{{
	// return L1\L2 + L2\L1
	deterministic_finite_amore_automaton * L1_without_L2;
	finite_language_automaton * L2_without_L1;
	nondeterministic_finite_automaton * ret = NULL;

	L1_without_L2 = lang_difference(other);
	L2_without_L1 = other.lang_difference(*this);

	ret = L1_without_L2->lang_union(*L2_without_L1);

	delete L1_without_L2;
	delete L2_without_L1;

	return ret;
}}}
nondeterministic_finite_automaton * nondeterministic_finite_amore_automaton::lang_symmetric_difference(finite_language_automaton &other)
{{{
	deterministic_finite_amore_automaton * L1_without_L2;
	finite_language_automaton * L2_without_L1;
	nondeterministic_finite_automaton * ret = NULL;

	L1_without_L2 = lang_difference(other);
	L2_without_L1 = other.lang_difference(*this);

	ret = L1_without_L2->lang_union(*L2_without_L1);

	delete L1_without_L2;
	delete L2_without_L1;

	return ret;
}}}

nondeterministic_finite_automaton * deterministic_finite_amore_automaton::lang_concat(finite_language_automaton &other)
{{{
	nondeterministic_finite_automaton * ret;

	deterministic_finite_amore_automaton * o_d;
	nondeterministic_finite_amore_automaton * o_n;
	nondeterministic_finite_automaton * t_n;

	bool had_to_nondeterminize = false;

	o_n = dynamic_cast<nondeterministic_finite_amore_automaton*> (&other);

	if(!o_n) {
		o_d = dynamic_cast<deterministic_finite_amore_automaton*> (&other);
		if(!o_d) {
			// FIXME: non-compatible automaton
			// should throw exception
			return NULL;
		}

		// nondeterminize
		had_to_nondeterminize = true;
		o_n = dynamic_cast<nondeterministic_finite_amore_automaton*> (o_n->nondeterminize());
	}

	t_n = this->nondeterminize();
	ret = dynamic_cast<nondeterministic_finite_automaton*>(t_n->lang_concat(*o_n));
	delete t_n;

	if(had_to_nondeterminize)
		delete o_n;

	return ret;
}}}
nondeterministic_finite_automaton * nondeterministic_finite_amore_automaton::lang_concat(finite_language_automaton &other)
{{{
	nondeterministic_finite_automaton * ret;

	deterministic_finite_amore_automaton * o_d;
	nondeterministic_finite_amore_automaton * o_n;

	bool had_to_nondeterminize = false;

	o_n = dynamic_cast<nondeterministic_finite_amore_automaton*> (&other);

	if(!o_n) {
		o_d = dynamic_cast<deterministic_finite_amore_automaton*> (&other);
		if(!o_d) {
			// FIXME: non-compatible automaton
			// should throw exception
			return NULL;
		}

		// nondeterminize
		had_to_nondeterminize = true;
		o_n = dynamic_cast<nondeterministic_finite_amore_automaton*> (o_n->nondeterminize());
	}

	ret = new nondeterministic_finite_amore_automaton(concatfa(nfa_p, o_n->nfa_p));

	if(had_to_nondeterminize)
		delete o_n;

	return ret;
}}}

std::basic_string<int32_t> deterministic_finite_amore_automaton::serialize()
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
std::basic_string<int32_t> nondeterministic_finite_amore_automaton::serialize()
{{{
	basic_string<int32_t> ret;
	basic_string<int32_t> temp;
	unsigned int s;
	int l;

	if(!nfa_p) {
		return ret; // empty basic_string
	}

	// stream length; will be filled in later
	ret += 0;

	// alphabet size
	ret += htonl(nfa_p->alphabet_size);
	// state count
	ret += htonl(nfa_p->highest_state+1);

	for(s = 0; s <= nfa_p->highest_state; s++)
		if(isinit(nfa_p->infin[s]))
			temp += htonl(s);
	// number of initial states
	ret += htonl(temp.length());
	// initial states
	ret += temp;

	temp.clear();
	for(s = 0; s <= nfa_p->highest_state; s++)
		if(isfinal(nfa_p->infin[s]))
			temp += htonl(s);
	// number of final states
	ret += htonl(temp.length());
	// final states
	ret += temp;

	temp.clear();
	for(l = (nfa_p->is_eps == TRUE) ? 0 : 1; l <= ((int)nfa_p->alphabet_size); l++) { // label
		for(s = 0; s <= nfa_p->highest_state; s++) { // source state id
			for(unsigned int d = 0; d <= nfa_p->highest_state; d++) {
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

	ret[0] = htonl(ret.length() - 1);

	return ret;
}}}

bool deterministic_finite_amore_automaton::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
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
bool nondeterministic_finite_amore_automaton::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{{{
	int size;
	int s, count;

	if(it == limit)
		goto nfaa_deserialization_failed;

	// string length (excluding length field)
	size = ntohl(*it);
	it++;
	if(size <= 0 || limit == it) goto nfaa_deserialization_failed;

	if(nfa_p)
		freenfa(nfa_p);
	nfa_p = newnfa();

	// alphabet size
	nfa_p->alphabet_size = ntohl(*it);

	// state count
	size--, it++; if(size <= 0 || limit == it) goto nfaa_deserialization_failed;
	nfa_p->highest_state = ntohl(*it) - 1;

	// allocate data structures
	nfa_p->infin = newfinal(nfa_p->highest_state);
	nfa_p->delta = newendelta(nfa_p->alphabet_size, nfa_p->highest_state);
	nfa_p->is_eps = TRUE;
	if(!(nfa_p->infin) || !(nfa_p->delta))
		goto nfaa_deserialization_failed;

	// initial states
	size--, it++; if(size <= 0 || limit == it) goto nfaa_deserialization_failed;
	count = ntohl(*it);

	for(s = 0; s < count; s++) {
		size--, it++; if(size <= 0 || limit == it) goto nfaa_deserialization_failed;
		if(ntohl(*it) > nfa_p->highest_state)
			goto nfaa_deserialization_failed;
		setinit(nfa_p->infin[ntohl(*it)]);
	}

	// final states
	size--, it++; if(size <= 0 || limit == it) goto nfaa_deserialization_failed;
	count = ntohl(*it);

	for(s = 0; s < count; s++) {
		size--, it++; if(size <= 0 || limit == it) goto nfaa_deserialization_failed;
		if(ntohl(*it) > nfa_p->highest_state)
			goto nfaa_deserialization_failed;
		setfinalT(nfa_p->infin[ntohl(*it)]);
	}

	// transitions
	size--, it++; if(size <= 0 || limit == it) goto nfaa_deserialization_failed;
	count = ntohl(*it);
	for(s = 0; s < count; s++) {
		int32_t src, label, dst;

		size--, it++; if(size <= 0 || limit == it) goto nfaa_deserialization_failed;
		src = ntohl(*it);
		size--, it++; if(size <= 0 || limit == it) goto nfaa_deserialization_failed;
		label = ntohl(*it);
		size--, it++; if(size <= 0 || limit == it) goto nfaa_deserialization_failed;
		dst = ntohl(*it);

		if(   (label < -1) || (label >= (int)nfa_p->alphabet_size)
		   || (src < 0) || (src > (int)nfa_p->highest_state)
		   || (dst < 0) || (dst > (int)nfa_p->highest_state) )
			goto nfaa_deserialization_failed;

		connect(nfa_p->delta, label+1, src, dst);
	}

	size--, it++;

	if(size != 0)
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

deterministic_finite_amore_automaton * deterministic_finite_amore_automaton::determinize()
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
nondeterministic_finite_amore_automaton * nondeterministic_finite_amore_automaton::nondeterminize()
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

