/* $Id$
 * vim: fdm=marker
 *
 * libAMoRE "value-add": glue between libalf and libAMoRE++
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __amore_automaton_holder_h__
# define __amore_automaton_holder_h__

// this is the glue between libAMoRE(++) and libalf,
// it gives everything required to make algorithms construct amore automata
// or to teach algorithms from automata (teacher and oracle)

#include <amore++/finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>
#include <amore++/nondeterministic_finite_automaton.h>

#include <libalf/automaton_constructor.h>

namespace amore_alf_glue {

using namespace std;
using namespace libalf;
using namespace amore;


class amore_automaton_holder : public automaton_constructor {
	private:
		finite_automaton * automaton;

	public:
		amore_automaton_holder()
		{ automaton = NULL; }

		virtual ~amore_automaton_holder()
		{ clear_automaton(); }

		virtual void clear_automaton()
		{ if(automaton) { delete automaton; automaton = NULL; } }

		virtual void forget_automaton()
		{ automaton = NULL; }

		virtual bool can_construct_NFA()
		{ return true; }

		virtual bool can_construct_DFA()
		{ return false; }

		virtual bool construct(bool is_dfa, int alphabet_size, int state_count, set<int> &start, set<int> &final, libalf::transition_set &transitions)
		{
			amore::transition_set * transition_caster;
			transition_caster = (amore::transition_set *) &transitions;

			clear_automaton();
			if(is_dfa)
				automaton = new deterministic_finite_automaton;
			else
				automaton = new nondeterministic_finite_automaton;

			return automaton->construct(alphabet_size, state_count, start, final, *transition_caster);
		}

		virtual finite_automaton * get_automaton()
		// the returned automaton instance will be deleted at some point if you don't holder->forget_automaton().
		{ return automaton; }
};


inline bool automaton_equivalence_query(finite_automaton & automaton, finite_automaton & hypothesis, list<int> & counterexample)
{
	finite_automaton * difference;
	bool is_empty;

	difference = automaton.lang_symmetric_difference(hypothesis);
	counterexample = difference->get_sample_word(is_empty);
	delete difference;

	return is_empty;
};


inline bool automaton_membership_query(finite_automaton & automaton, list<int> & word)
{ return automaton.contains(word); };


template<class answer>
inline int automaton_answer_knowledgebase(finite_automaton & automaton, knowledgebase<answer> & base)
{
	int count = 0;
	typename knowledgebase<answer>::iterator qi = base.qbegin();

	while(qi != base.qend()) {
		list<int> word;
		word = qi->get_word();
		qi->set_answer( (answer) automaton_membership_query(automaton, word) );
		qi = base.qbegin();
		count++;
	}

	return count;
};


}; // end of namespace amore_alf_glue

#endif

