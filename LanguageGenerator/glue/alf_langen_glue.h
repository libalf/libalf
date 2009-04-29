/* $Id$
 * vim: fdm=marker
 *
 * liblangen "value-add": glue between LanguageGenerator (liblangen) and libalf
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __alf_langen_automaton_holder_h__
# define __alf_langen_automaton_holder_h__

// this is the glue between libalf and liblangen,
// it gives everything required to make algorithms construct automata using libalf-classes

#include <LanguageGenerator/automaton_constructor.h>
#include <libalf/automaton_constructor.h>

namespace alf_langen_glue {

using namespace std;
using namespace LanguageGenerator;
using namespace libalf;

class langen_automaton_constructor_wrapper : public LanguageGenerator::automaton_constructor {
	private:
		libalf::automaton_constructor * libalf_automaton;

	public:
		langen_automaton_constructor_wrapper()
		{ }

		virtual ~langen_automaton_constructor_wrapper()
		{ }

		virtual bool can_construct_NFA()
		{ return libalf_automaton->can_construct_NFA(); }

		virtual bool can_construct_DFA()
		{ return libalf_automaton->can_construct_DFA(); }

		virtual bool construct(bool is_dfa, int alphabet_size, int state_count, set<int> &start, set<int> &final, LanguageGenerator::transition_set &transitions)
		{
			libalf::transition_set * transition_caster;
			transition_caster = (libalf::transition_set*) & transitions;
			return libalf_automaton->construct(is_dfa, alphabet_size, state_count, start, final, *transition_caster);
		}

		void set_constructor(libalf::automaton_constructor * constructor)
		{ libalf_automaton = constructor; }

		libalf::automaton_constructor * get_constructor()
		{ return libalf_automaton; }
};

}; // end of namespace alf_langen_glue

#endif

