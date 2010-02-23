/* $Id$
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
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __mvca_alf_glue_h__
# define __mvca_alf_glue_h__

// this is the glue between libAMoRE++ and libalf,
// it gives everything to teach algorithms from automata (teacher and oracle)

#include <stdlib.h>

#include <iostream>

#include <libmVCA/mVCA.h>
#include <libalf/conjecture.h>
#include <libalf/knowledgebase.h>

#include <amore++/finite_automaton.h>
#include <amore_alf_glue.h>

namespace mVCA_alf_glue {

using namespace std;
using namespace libalf;
using namespace libmVCA;

inline bool automaton_partial_equivalence_query(mVCA & model, conjecture * cj, int m_bound, list<int> & counterexample)
// conjecture is expected to be a simple_automaton.
{
	bool f_is_deterministic;
	int f_alphabet_size, f_state_count;
	set<int> f_initial_states, f_final_states;
	multimap<pair<int,int>, int> f_transitions;

	amore::finite_automaton *partial_model;

	model.get_bounded_behaviour_graph(m_bound, f_is_deterministic, f_alphabet_size, f_state_count, f_initial_states, f_final_states, f_transitions);
	partial_model = amore::construct_amore_automaton(f_is_deterministic, f_alphabet_size, f_state_count, f_initial_states, f_final_states, f_transitions);

	bool ret;

	ret = amore_alf_glue::automaton_equivalence_query(*partial_model, cj, counterexample);

	delete partial_model;

	return ret;
}

inline bool automaton_partial_equivalence_query(mVCA & model, mVCA & hypothesis, int m_bound, list<int> & counterexample)
{
	bool f_is_deterministic;
	int f_alphabet_size, f_state_count;
	set<int> f_initial_states, f_final_states;
	multimap<pair<int,int>, int> f_transitions;

	amore::finite_automaton *partial_model, *partial_hypothesis;

	pushdown_alphabet a = model.get_alphabet();
	if(hypothesis.get_alphabet() != a) {
		cerr << "mVCA_alf_glue::automaton_partial_equivalence_query(...): alphabet mismatch for model and hypothesis. aborting program.\n";
		exit(-1);
	}

	model.get_bounded_behaviour_graph(m_bound, f_is_deterministic, f_alphabet_size, f_state_count, f_initial_states, f_final_states, f_transitions);
	partial_model = amore::construct_amore_automaton(f_is_deterministic, f_alphabet_size, f_state_count, f_initial_states, f_final_states, f_transitions);

	hypothesis.get_bounded_behaviour_graph(m_bound, f_is_deterministic, f_alphabet_size, f_state_count, f_initial_states, f_final_states, f_transitions);
	partial_hypothesis = amore::construct_amore_automaton(f_is_deterministic, f_alphabet_size, f_state_count, f_initial_states, f_final_states, f_transitions);

	bool ret;

	ret = amore_alf_glue::automaton_equivalence_query(*partial_model, *partial_hypothesis, counterexample);

	delete partial_model;
	delete partial_hypothesis;

	return ret;
}

inline bool automaton_equivalence_query(mVCA & model, mVCA & hypothesis, list<int> & counterexample)
{{{
	counterexample.clear();

	return model.lang_equal(hypothesis, counterexample);
}}};

inline bool automaton_equivalence_query(mVCA & model, conjecture *cj, list<int> & counterexample)
{{{
	simple_mVCA * a;
	mVCA * hypothesis;
	bool ret;

	a = dynamic_cast<simple_mVCA*>(cj);
	if(!a) {
		cerr << "mVCA_alf_glue::automaton_equivalence_query(...): hypothesis is not an m-bounded visible 1-counter automaton. aborting program.\n";
		exit(-1);
	}
	hypothesis = construct_mVCA(a->state_count, a->alphabet_size, a->alphabet_directions, a->initial_state, a->final_states, a->m_bound, a->transitions);

	ret = automaton_equivalence_query(model, *hypothesis, counterexample);

	delete hypothesis;

	return ret;
}}};

inline bool automaton_membership_query(mVCA & model, list<int> & word)
{ return model.contains(word); };


template<class answer>
inline int automaton_answer_knowledgebase(mVCA & model, knowledgebase<answer> & base)
{{{
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
}}};


}; // end of namespace mVCA_alf_glue

#endif // __mvca_alf_glue_h__

