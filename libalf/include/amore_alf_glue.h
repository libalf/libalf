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
#include <amore++/nondeterministic_finite_automaton.h>
#include <libalf/conjecture.h>
#include <libalf/knowledgebase.h>

namespace amore_alf_glue {

using namespace std;



inline amore::finite_automaton * automaton_libalf2amore(libalf::finite_automaton & automaton)
{{{
	set<int> final_states;
	automaton.get_final_states(final_states);
	return amore::construct_amore_automaton(automaton.is_deterministic, automaton.input_alphabet_size, automaton.state_count, automaton.initial_states, final_states, automaton.transitions);
}}}

inline libalf::finite_automaton * automaton_amore2libalf(amore::finite_automaton & automaton)
{{{
	basic_string<int32_t> ser = automaton.serialize();
	serial_stretch range(ser);
	libalf::finite_automaton * ret = new libalf::finite_automaton;
	if(!ret->deserialize(range)) {
		delete ret;
		ret = NULL;
	}
	return ret;
}}}



inline bool automaton_antichain_equivalence_query(amore::finite_automaton & model, amore::finite_automaton & hypothesis, list<int> & counterexample)
{{{
	if(!model.antichain__is_superset_of(hypothesis, counterexample))
		return false;
	return hypothesis.antichain__is_superset_of(model, counterexample);
}}};

inline bool automaton_classic_equivalence_query(amore::finite_automaton & model, amore::finite_automaton & hypothesis, list<int> & counterexample)
{{{
	amore::finite_automaton * difference;
	bool is_empty;

	counterexample.clear();

	difference = model.lang_difference(hypothesis);
	counterexample = difference->get_sample_word(is_empty);

	if(is_empty) {
		delete difference;
		difference = hypothesis.lang_difference(model);
		counterexample = difference->get_sample_word(is_empty);
	}

	delete difference;

	return is_empty;
}}};

inline bool automaton_equivalence_query(amore::finite_automaton & model, amore::finite_automaton & hypothesis, list<int> & counterexample)
{
	// use antichain-algorithm, if one of the automata is an NFA with size larger than 7
	if(    ( dynamic_cast<amore::nondeterministic_finite_automaton*>(&model)      != NULL && model.get_state_count()      >= 8 )
	    || ( dynamic_cast<amore::nondeterministic_finite_automaton*>(&hypothesis) != NULL && hypothesis.get_state_count() >= 8 ) ) {
		return automaton_antichain_equivalence_query(model, hypothesis, counterexample);
	} else {
		return automaton_classic_equivalence_query(model, hypothesis, counterexample);
	}
};

inline bool automaton_classic_equivalence_query(amore::finite_automaton & model, libalf::conjecture *cj, list<int> & counterexample)
{{{
	libalf::finite_automaton *ba;
	amore::finite_automaton *hypothesis;
	bool ret;

	counterexample.clear();

	ba = dynamic_cast<libalf::finite_automaton*>(cj);
	if(!ba) {
		fprintf(stderr, "equivalence query: hypothesis is not an automaton!\n");
		return false;
	}
	hypothesis = automaton_libalf2amore(*ba);

	ret = automaton_classic_equivalence_query(model, *hypothesis, counterexample);

	delete hypothesis;

	return ret;
}}};

inline bool automaton_antichain_equivalence_query(amore::finite_automaton & model, libalf::conjecture *cj, list<int> & counterexample)
{{{
	libalf::finite_automaton *ba;
	amore::finite_automaton *hypothesis;
	bool ret;

	counterexample.clear();

	ba = dynamic_cast<libalf::finite_automaton*>(cj);
	if(!ba) {
		fprintf(stderr, "equivalence query: hypothesis is not an automaton!\n");
		return false;
	}
	hypothesis = automaton_libalf2amore(*ba);

	ret = automaton_antichain_equivalence_query(model, *hypothesis, counterexample);

	delete hypothesis;

	return ret;
}}};

inline bool automaton_equivalence_query(amore::finite_automaton & model, libalf::conjecture *cj, list<int> & counterexample)
{{{
	libalf::finite_automaton *ba;
	amore::finite_automaton *hypothesis;
	bool ret;

	counterexample.clear();

	ba = dynamic_cast<libalf::finite_automaton*>(cj);
	if(!ba) {
		fprintf(stderr, "equivalence query: hypothesis is not an automaton!\n");
		return false;
	}
	hypothesis = automaton_libalf2amore(*ba);

	ret = automaton_equivalence_query(model, *hypothesis, counterexample);

	delete hypothesis;

	return ret;
}}};



inline bool automaton_membership_query(amore::finite_automaton & model, list<int> & word)
{ return model.contains(word); };

template<class answer>
inline int automaton_answer_knowledgebase(amore::finite_automaton & model, libalf::knowledgebase<answer> & base)
{
	int count = 0;
	typename libalf::knowledgebase<answer>::iterator qi = base.qbegin();

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

