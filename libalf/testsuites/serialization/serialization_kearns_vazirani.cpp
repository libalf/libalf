#include <libalf/algorithm_kearns_vazirani.h>
#include <libalf/knowledgebase.h>
#include <libalf/alphabet.h>
#include <list>

#include <libalf/alf.h>
#include <amore++/nondeterministic_finite_automaton.h>
#include "amore_alf_glue.h"

using namespace libalf;

int main(int argc, char**argv)
{
	int alphabet_size = 2;
	const char* regex = "aba(bb)*ab*";
	bool regex_ok;

	amore::finite_automaton *nfa, *dfa;
	nfa = new amore::nondeterministic_finite_automaton(alphabet_size, regex, regex_ok);
	dfa = nfa->determinize();
	dfa->minimize();
	delete nfa;

	knowledgebase<bool> base;

	kearns_vazirani<bool> alg1(&base, NULL, 2, true);
	kearns_vazirani<bool> alg2(NULL, NULL, 0, false);

	
	amore::finite_automaton * hypothesis = NULL;

	for(int iteration = 1; iteration <= 5; iteration++) {
		int c = 'a';
		conjecture * cj;

		std::basic_string<int32_t> serial2 = alg1.serialize();
		serial_stretch ss2(serial2);
		alg1.deserialize(ss2);


		while( NULL == (cj = alg1.advance()) ) {
			// resolve missing knowledge:

			// create query-tree
			knowledgebase<bool> * query;
			query = base.create_query_tree();

			// answer queries
			amore_alf_glue::automaton_answer_knowledgebase(*dfa, *query);

			// merge answers into knowledgebase
			base.merge_knowledgebase(*query);
			delete query;
			c++;
		}

		libalf::finite_automaton * ba = dynamic_cast<libalf::finite_automaton*>(cj);
		if(hypothesis)
			delete hypothesis;
		hypothesis = amore_alf_glue::automaton_libalf2amore(*ba);
		delete cj;
		if(!hypothesis) {
			printf("generation of hypothesis failed!\n");
			return -1;
		}

		std::list<int> counterexample;

		if(amore_alf_glue::automaton_equivalence_query(*dfa, *hypothesis, counterexample)) {
			// equivalent
			std::cout << "success.\n";
			break;
		}

		
		alg1.add_counterexample(counterexample);
	}


	alg1.print(std::cout);

	std::basic_string<int32_t> serial = alg1.serialize();
	serial_stretch ss(serial);

	if(alg2.deserialize(ss))
		std::cout << "done.";
	else
		std::cout << "failed.";
	

	alg2.print(std::cout);
	//alg2.generate_statistics();

	return 0;
}

