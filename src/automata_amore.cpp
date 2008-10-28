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

automata_amore::automata_amore()
{{{
	dfa_p = NULL;
	nfa_p = NULL;
}}}

automata_amore::automata_amore(enum automata_type type)
{{{
	if(type == NONDETERMINISTIC_FINITE_AUTOMATA) {
		dfa_p = NULL;
		nfa_p = newnfa();
	}
	if(type == DETERMINISTIC_FINITE_AUTOMATA) {
		dfa_p = newdfa();
		nfa_p = NULL;
	};
}}}

automata_amore::~automata_amore()
{{{
	clear_automatas();
}}}

void automata_amore::set_nfa(nfa a)
{{{
	clear_automatas();
	nfa_p = a;
}}}

void automata_amore::set_dfa(dfa a)
{{{
	clear_automatas();
	dfa_p = a;
}}}

enum automata_type automata_amore::get_type()
{{{
	if(nfa_p)
		return NONDETERMINISTIC_FINITE_AUTOMATA;
	if(dfa_p)
		return DETERMINISTIC_FINITE_AUTOMATA;

	return NO_AUTOMATA;
}}}

automata* automata_amore::clone()
{{{
	automata_amore *n;

	n = new automata_amore();

	if(dfa_p)
		n->dfa_p = clonedfa(dfa_p);
	if(nfa_p)
		n->nfa_p = clonenfa(nfa_p);

	return n;
}}}

std::string automata_amore::generate_dotfile()
{
	std::string dot;

	// FIXME

	return dot;
}

bool automata_amore::is_empty()
{
}

list<int> automata_amore::get_sample_word()
{
}

bool automata_amore::operator==(automata &other)
// == will also nfa2dfa both automatas
{{{
	automata_amore *o = dynamic_cast<automata_amore*> (&other);
	// other has to be an automata_amore
	if(o) {
		make_deterministic();

		o->make_deterministic();

		return equiv(this->dfa_p, o->dfa_p);
	} else {
		// other automata is not an automata_amore!
		// FIXME throw some exception
		return false;
	}
}}}

bool automata_amore::includes(automata &subautomata)
{
}

bool automata_amore::is_subset_of(automata &superautomata)
{
}

bool automata_amore::contains(list<int>)
{
}

void automata_amore::make_deterministic()
{{{
	if(nfa_p) {
		dfa_p = nfa2dfa(nfa_p);
		freenfa(nfa_p);
		nfa_p = NULL;
	}
}}}

void automata_amore::make_undeterministic()
{{{
	if(dfa_p) {
		nfa_p = dfa2nfa(dfa_p);
		freedfa(dfa_p);
	}
}}}

void automata_amore::minimize()
{{{
	dfa_p = dfamdfa(dfa_p, true);
}}}

void automata_amore::lang_complement()
{{{
	make_deterministic();

	if(dfa_p) {
		dfa a;

		a = compldfa(dfa_p);
		freedfa(dfa_p);
		dfa_p = a;
	}
}}}

automata* automata_amore::lang_union(automata &other)
{{{
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
}}}

automata* automata_amore::lang_intersect(automata &other)
{
}

automata* automata_amore::lang_difference(automata &other)
{
}

automata* automata_amore::lang_without(automata &other)
{{{
	automata_amore *o = dynamic_cast<automata_amore*> (&other);
	// other has to be an automata_amore
	if(o) {
		automata_amore *a = new automata_amore();

		make_deterministic();
		o->make_deterministic();

		a->set_dfa(insecfa(dfa_p, o->dfa_p, true));

		return a;
	} else {
		// FIXME: throw exception?
		return NULL;
	}
}}}

automata* automata_amore::lang_concat(automata &other)
{{{
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
}}}

void automata_amore::clear_automatas()
{{{
	if(nfa_p)
		freenfa(nfa_p);
	if(dfa_p)
		freedfa(dfa_p);
}}}

} // end namespace libalf

