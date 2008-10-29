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

#include "libalf/automata.h"
#include "libalf/automata_amore.h"

// attention: stupid amore headers typedef string to be char*
// thus we have to use "std::string"...

namespace libalf {

using namespace std;

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
	if(dfa_p)
		freedfa(dfa_p);
	dfa_p = a;
}}}
nondeterministic_finite_amore_automaton::nondeterministic_finite_amore_automaton(nfa a)
{{{
	if(nfa_p)
		freenfa(nfa_p);
	nfa_p = a;
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
	return new deterministic_finite_amore_automaton(clonedfa(dfa_p));
}}}
nondeterministic_finite_amore_automaton * nondeterministic_finite_amore_automaton::clone()
{{{
	return new nondeterministic_finite_amore_automaton(clonenfa(nfa_p));
}}}

std::string deterministic_finite_amore_automaton::generate_dotfile()
{
}
std::string nondeterministic_finite_amore_automaton::generate_dotfile()
{
}

bool deterministic_finite_amore_automaton::is_empty()
// note: calling minimize()es this
{{{
	bool ret;
	dfa a = newdfa();

	minimize();

	ret = equiv(dfa_p, a);

	freedfa(a);

	return ret;
}}}
bool nondeterministic_finite_amore_automaton::is_empty()
{
}

list<int> deterministic_finite_amore_automaton::get_sample_word()
{
}
list<int> nondeterministic_finite_amore_automaton::get_sample_word()
{
}

bool deterministic_finite_amore_automaton::operator==(finite_language_automaton &other)
// note: calling minimize()es this and other
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
{
}

bool deterministic_finite_amore_automaton::includes(finite_language_automaton &subautomaton)
{
	// -> amore::inclusion
}
bool nondeterministic_finite_amore_automaton::includes(finite_language_automaton &subautomaton)
{
	// -> amore::inclusion
}

bool deterministic_finite_amore_automaton::contains(list<int>)
{
}
bool nondeterministic_finite_amore_automaton::contains(list<int>)
{
}

void deterministic_finite_amore_automaton::minimize()
{{{
	dfa_p = dfamdfa(dfa_p, true);
}}}
void nondeterministic_finite_amore_automaton::minimize()
{
}

void deterministic_finite_amore_automaton::lang_complement()
{{{
		dfa a;

		a = compldfa(dfa_p);
		freedfa(dfa_p);
		dfa_p = a;
}}}
void nondeterministic_finite_amore_automaton::lang_complement()
{
}

finite_language_automaton * deterministic_finite_amore_automaton::lang_union(finite_language_automaton &other)
{
// FIXME
/*
	automata_amore *o = dynamic_cast<automata_amore*> (&other);
	// other has to be an automata_amore
	if(o) {
		automata_amore *a = new automata_amore();

		make_undeterministic();
		o->make_undeterministic();

		a->set_nfa( unionfa(nfa_p, o->nfa_p) );

		return a;
	} else {
		// FIXME: throw exception?
		return NULL;
	}
*/
}
finite_language_automaton * nondeterministic_finite_amore_automaton::lang_union(finite_language_automaton &other)
{
}

finite_language_automaton * deterministic_finite_amore_automaton::lang_intersect(finite_language_automaton &other)
{
}
finite_language_automaton * nondeterministic_finite_amore_automaton::lang_intersect(finite_language_automaton &other)
{
}

finite_language_automaton * deterministic_finite_amore_automaton::lang_difference(finite_language_automaton &other)
{
}
finite_language_automaton * nondeterministic_finite_amore_automaton::lang_difference(finite_language_automaton &other)
{
}

deterministic_finite_amore_automaton * deterministic_finite_amore_automaton::lang_without(finite_language_automaton &other)
{{{
	deterministic_finite_amore_automaton *ret;

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

nondeterministic_finite_automaton * deterministic_finite_amore_automaton::nondeterminize()
{{{
	return new nondeterministic_finite_amore_automaton(dfa2nfa(dfa_p));
}}}
deterministic_finite_automaton * nondeterministic_finite_amore_automaton::determinize()
{{{
	return new deterministic_finite_amore_automaton(nfa2dfa(nfa_p));
}}}

void deterministic_finite_amore_automaton::set_dfa(dfa a)
{{{
	dfa_p = a;
}}}
void nondeterministic_finite_amore_automaton::set_nfa(nfa a)
{{{
	nfa_p = a;

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

