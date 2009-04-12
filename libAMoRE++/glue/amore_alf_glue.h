/* $Id$
 * vim: fdm=marker
 *
 * libalf "value-add"
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
		{
			automaton = NULL;
		}
		virtual ~amore_automaton_holder()
		{
			clear_automaton();
		}
		virtual void clear_automaton()
		{
			if(automaton) {
				delete automaton;
				automaton = NULL;
			}
		}

		virtual bool can_construct_NFA()
		{ return true; }

		virtual bool can_construct_DFA()
		{ return false; }

		virtual bool construct(bool is_dfa, int alphabet_size, int state_count, set<int> &start, set<int> &final, transition_set &transitions)
		{
			clear_automaton();
			if(is_dfa)
				automaton = new deterministic_finite_automaton;
			else
				automaton = new nondeterministic_finite_automaton;
			automaton->construct(alphabet_size, state_count, start, final, transitions);
		}

		virtual finite_automaton & get_automaton()
		{
			return &(*automaton);
		}

};


template<answer>
inline void automaton_answer_knowledgebase(finite_automaton & automaton, knowledgebase<answer> & base)
{
	knowledgebase<answer>::iterator qi;
	qi = base.qbegin();
	while(qi != base.qend()) {
		list<int> word;
		word = qi->get_word();
		qi->set_answer( (answer) automaton_membership_query(automaton, word) );
		qi = base.qbegin();
	}
};


inline bool automaton_membership_query(finite_automaton & automaton, list<int> & word)
{
	return automaton->contains(word);
};


inline bool automaton_equivalence_query(finite_automaton & automaton, finite_automaton & hypothesis, list<int> & counterexample)
{
	finite_automaton * difference;
	bool is_empty;

	difference = automaton->lang_symmetric_difference(hypothesis);
	counterexample = difference->get_sample_word(is_empty);
	if(is_empty)
		return true;
	else
		return false;
};

}; // end of namespace amore_alf_glue

#endif

