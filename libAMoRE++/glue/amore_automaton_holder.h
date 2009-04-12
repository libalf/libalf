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
			if(automaton)
				delete automaton;
		}

		virtual bool can_construct_NFA()
		{ return true; }

		virtual bool can_construct_DFA()
		{ return false; }

		virtual bool construct(bool dfa, int alphabet_size, int state_count, set<int> &start, set<int> &final, transition_set &transitions)
		{
			if(automaton)
				delete automaton;
			if(dfa) {
				automaton = new deterministic_finite_automaton;
			} else {
				automaton = new nondeterministic_finite_automaton;
			}
			automaton->construct(alphabet_size, state_count, start, final, transitions);
		}

		virtual finite_automaton & get_automaton()
		{
			return &(*automaton);
		}
};

template<answer>
inline void answer_knowledgebase_from_automaton(knowledgebase<answer> & base, finite_automaton & automaton)
{
	knowledgebase<answer>::iterator qi;
	qi = base.qbegin();
	while(qi != base.qend()) {
		list<int> word;
		word = qi->get_word();
		qi->set_answer( (answer) membership_query(automaton, word) );
		qi = base.qbegin();
	}
};

inline bool membership_query(finite_automaton & automaton, list<int> & word)
{
	return automaton->contains(word);
};

inline bool equivalence_query(finite_automaton & automaton, finite_automaton & hypothesis, list<int> & counterexample)
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

#endif

