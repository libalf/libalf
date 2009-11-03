/* $Id: amore_alf_glue.h 992 2009-10-12 18:59:11Z davidpiegdon $
 * vim: fdm=marker
 *
 * This file is part of libalf.
 * glue between libalf and libAMoRE++
 * http://libalf.informatik.rwth-aachen.de/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __amore_alf_glue_h__
# define __amore_alf_glue_h__

// this is the glue between libAMoRE++ and libalf,
// it gives everything to teach algorithms from automata (teacher and oracle)

#include <amore++/finite_automaton.h>
#include <libalf/conjecture.h>

namespace amore_alf_glue {

using namespace std;
using namespace libalf;
using namespace amore;


inline bool automaton_equivalence_query(finite_automaton & model, finite_automaton & hypothesis, list<int> & counterexample)
{
	// there are two options:
	// either get automaton containing the symmetric difference of model and hypothesis (variant 1)
	// or first get model \ hypothesis and if this is empty, get hypothesis \ model (variant 2).
	// variant 1 seems more logically, but variant 2 seems to get better result.

	finite_automaton * difference;
	bool is_empty;

	counterexample.clear();

#if 0
	// variant 1:
	difference = model.lang_symmetric_difference(hypothesis);
	counterexample = difference->get_sample_word(is_empty);
#else
	// variant 2:
	difference = model.lang_difference(hypothesis);
	counterexample = difference->get_sample_word(is_empty);

	if(is_empty) {
		delete difference;
		difference = hypothesis.lang_difference(model);
		counterexample = difference->get_sample_word(is_empty);
	}
#endif

	delete difference;

	return is_empty;
};

inline bool automaton_equivalence_query(finite_automaton & model, conjecture *cj, list<int> & counterexample)
{
	simple_automaton *ba;
	finite_automaton *hypothesis;
	bool ret;

	counterexample.clear();

	ba = dynamic_cast<simple_automaton*>(cj);
	if(!ba) {
		fprintf(stderr, "equivalence query: hypothesis is not an automaton!\n");
		return false;
	}
	hypothesis = construct_amore_automaton(ba->is_deterministic, ba->alphabet_size, ba->state_count, ba->initial, ba->final, ba->transitions);

	ret = automaton_equivalence_query(model, *hypothesis, counterexample);

	delete hypothesis;

	return ret;

}



inline bool automaton_membership_query(finite_automaton & model, list<int> & word)
{ return model.contains(word); };


template<class answer>
inline int automaton_answer_knowledgebase(finite_automaton & model, knowledgebase<answer> & base)
{
	int count = 0;
	typename knowledgebase<answer>::iterator qi = base.qbegin();

	while(qi != base.qend()) {
		list<int> word;
		word = qi->get_word();
		qi->set_answer( (answer) automaton_membership_query(model, word) );
		qi = base.qbegin();
		count++;
	}

	return count;
};


}; // end of namespace amore_alf_glue

#endif // __amore_alf_glue_h__

