/* $Id$
 * vim: fdm=marker
 *
 * (space efficient) glue between libalf and libAMoRE++
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __amore_alf_automaton_holder_h__
# define __amore_alf_automaton_holder_h__

// this is the glue between libAMoRE(++) and libalf,
// it gives everything to teach algorithms from automata (teacher and oracle)

#include <amore++/finite_automaton.h>

namespace amore_alf_glue {

using namespace std;
using namespace libalf;
using namespace amore;


inline bool automaton_equivalence_query(finite_automaton & model, finite_automaton & hypothesis, list<int> & counterexample)
// model is expected to be a minimized DFA!
{
	finite_automaton *difference, *tmp;
	bool is_empty;

	tmp = hypothesis.determinize();
	tmp->minimize();

	if(model == *tmp) {
		delete tmp;
		return true;
	}

	difference = model.lang_difference(*tmp);
	counterexample = difference->get_sample_word(is_empty);
	delete difference;
	if(is_empty) {
		difference = tmp->lang_difference(model);
		counterexample = difference->get_sample_word(is_empty);
		delete difference;
	}
	if(is_empty) {
		printf("ERROR: model != hypothesis but m\\h == h\\m == {} !\n");
		exit(1);
	}

	return false;
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

