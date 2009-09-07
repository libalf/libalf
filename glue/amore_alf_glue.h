/* $Id$
 * vim: fdm=marker
 *
 * glue between libalf and libAMoRE++
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __amore_alf_glue_h__
# define __amore_alf_glue_h__

// this is the glue between libAMoRE(++) and libalf,
// it gives everything to teach algorithms from automata (teacher and oracle)

#include <amore++/finite_automaton.h>

namespace amore_alf_glue {

using namespace std;
using namespace libalf;
using namespace amore;


inline bool automaton_equivalence_query(finite_automaton & automaton, finite_automaton & hypothesis, list<int> & counterexample)
{
	// there are two options:
	// either get automaton containing the symmetric difference of model and hypothesis (variant 1)
	// or first get model \ hypothesis and if this is empty, get hypothesis \ model (variant 2).
	// variant 1 seems more logically, but variant 2 seems to get better result.

	finite_automaton * difference;
	bool is_empty;

#if 0
	// variant 1:
	difference = automaton.lang_symmetric_difference(hypothesis);
	counterexample = difference->get_sample_word(is_empty);
#else
	// variant 2:
	difference = automaton.lang_difference(hypothesis);
	counterexample = difference->get_sample_word(is_empty);

	if(is_empty) {
		delete difference;
		difference = hypothesis.lang_difference(automaton);
		counterexample = difference->get_sample_word(is_empty);
	}
#endif

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

#endif // __amore_alf_glue_h__

