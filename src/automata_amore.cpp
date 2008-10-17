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

#include <amore/ext.h>

#include <libalf/automata.h>

namespace libalf {

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

virtual automata_amore::~automata()
{{{
	clear_automatas();
}}}

virtual automata_amore::set_nfa(nfa a)
{{{
	clear_automatas();
	nfa_p = a;
}}}

virtual automata_amore::set_dfa(dfa a)
{{{
	clear_automatas();
	dfa_p = a;
}}}

virtual enum automata_type automata_amore::get_type()
{{{
	if(nfa_p)
		return NONDETERMINISTIC_FINITE_AUTOMATA;
	if(dfa_p)
		return DETERMINISTIC_FINITE_AUTOMATA;

	return NO_AUTOMATA;
}}}

virtual automata* automata_amore::clone()
{
}

virtual bool automata_amore::is_empty()
{
}

virtual list<int> automata_amore::get_sample_word()
{
}

virtual bool automata_amore::operator==(automata &other)
// == will also nfa2dfa both automatas
{{{
	// other has to be an automata_amore
	if(dynamic_cast<automata_amore> other) {
		make_deterministic();

		other.make_deterministic();

		return equiv(this->dfa_p, other.dfa_p);
	} else {
		// other automata is not an automata_amore!
		// FIXME throw some exception
	}
}}}

virtual bool automata_amore::includes(automata &subautomata)
{
}

virtual bool automata_amore::is_subset_of(automata &superautomata)
{
}

virtual bool automata_amore::contains(list<int>)
{
}

virtual void automata_amore::make_deterministic()
{{{
	if(nfa_p) {
		dfa_p = nfa2dfa(nfa_p);
		freenfa(nfa_p);
		nfa_p = NULL;
	}
}}}

virtual void automata_amore::make_undeterministic()
{{{
	if(dfa_p) {
		nfa_p = nfa2dfa(dfa_p);
		freedfa(dfa_p);
	}
}}}

virtual void minimize()
{{{
	dfa_p = dfamdfa(dfa_p, true);
}}}

virtual void automata_amore::lang_complement()
{{{
	make_deterministic();

	if(dfa_p) {
		dfa a;

		a = compldfa(dfa_p);
		freedfa(dfa_p);
		dfa_p = a;
	}
}}}

virtual automata* automata_amore::lang_union(automata &other)
{{{
	automata *a = new automata();

	make_undeterministic();
	other.make_undeterministic();

	a->set_nfa( unionfa(nfa_p, other.nfa_p) );

	return a;
}}}

virtual automata* automata_amore::lang_intersect(automata &other)
{
}

virtual automata* automata_amore::lang_difference(automata &other)
{
}

virtual automata* automata_amore::lang_without(automata &other)
{{{
	automata *a = new automata();

	make_deterministic();
	other.make_deterministic();

	a->set_dfa(insecfa(dfa_p, other.dfa_p, true));

	return a;
}}}

virtual automata* automata_amore::lang_concat(automata &other)
{{{
	automata *a = new automata();

	make_undeterministic();
	other.make_undeterministic();

	a->set_nfa( concatfa(nfa_p, other.nfa_p) );

	return a;
}}}

virtual void automata_amore::clear_automatas()
{{{
	if(nfa_p)
		freenfa(nfa_p);
	if(dfa_p)
		freedfa(dfa_p);
}}}

} // end namespace libalf

