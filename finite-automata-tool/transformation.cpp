/* $Id$
 * vim: fdm=marker
 *
 * Finite Automata Tools
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>

#include <libalf/algorithm_NLstar.h>
//#include <libalf/algorithm_ULstar.h>

#include "fat.h"

#include "amore_alf_glue.h"

//#define SANITY_CHECK_TRANSFORMATION

bool a2rfsa(finite_automaton *& automaton)
{{{
	knowledgebase<bool> base;
	NLstar_table<bool> tbl(&base, NULL, automaton->get_alphabet_size());
	finite_automaton * hypothesis = NULL;

	bool equal = false;

	while(!equal) {
		conjecture *cj;

		while( NULL == (cj = tbl.advance()) )
			amore_alf_glue::automaton_answer_knowledgebase(*automaton, base);

		list<int> cex;
		if(amore_alf_glue::automaton_equivalence_query(*automaton, cj, cex)) {
			equal = true;
		} else {
			tbl.add_counterexample(cex);
		}

		delete cj;
	}

	delete automaton;
	automaton = hypothesis;

	return true;
}}}

bool do_transformation(finite_automaton *& automaton, transformation trans)
{{{
	finite_automaton * tmp;

#ifdef SANITY_CHECK_TRANSFORMATION
	finite_automaton * cl;
	if(automaton)
		cl = automaton->clone();
#endif

	switch(trans) {
		case trans_none:
			break;
		case trans_mdfa:
			tmp = automaton->determinize();
			delete automaton;
			automaton = tmp;
			automaton->minimize();
			break;
		case trans_minimize:
			automaton->minimize();
			break;
		case trans_determinize:
			tmp = automaton->determinize();
			delete automaton;
			automaton = tmp;
			break;
		case trans_rfsa:
			if(!a2rfsa(automaton))
				return false;
			break;
		default:
			return false;
	}

#ifdef SANITY_CHECK_TRANSFORMATION
	if(automaton) {
		finite_automaton * difference;
		list<int> w;
		bool empty;

		difference = automaton->lang_symmetric_difference(*cl);
		w = difference->get_sample_word(empty);
		if(!empty) {
			cerr << "transformation-error! mismatching word " << word2string(w) << "\n";
		} else {
			cerr << "transformation ok.\n";
		}

		delete difference;
		delete cl;
	}
#endif

	return true;;
}}}

