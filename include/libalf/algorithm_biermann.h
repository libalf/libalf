/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */


/*
 * BIERMANN is an offline learning algorithm, described e.g. in
 *	* A.W. Biermann, J.A. Feldman - On the Synthesis of Finite-State Machines from Samples of their Behavior
 *	* Martin Leucker - Learning Meets Verification
 *	* Arlindo L. Oliveira and João P.M. Silva - Efficient Algorithms for the Inference of Minimum Size DFAs
 * NOTE that due to it being offline, only knowledgebases are supported, no teachers.
 * also, undo operations are not supported.
 */


#ifndef __libalf_algorithm_biermann_h__
# define __libalf_algorithm_biermann_h__

#include <string>
#include <list>

#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

namespace biermann {

	class clause {
		// very reduced clause: can only represent the following formulas:
		// l1 != l2
		// and
		// l1 != l2 || l3 == l4
		public:
			int l1;
			int l2;

			bool has_second;

			int l3;
			int l4;


			bool verify(vector<int> & literal_values)
			{{{
				if(has_second)
					if(literal_values[l3] == literal_values[l4])
						return true;
				return literal_values[l1] != literal_values[l2];
			}}}

			void print(ostream &os)
			{{{
				char buf1[64];
				char buf2[64];

				if(has_second)
					snprintf(buf2, 64, " || (S%d == S%d)", l3, l4);
				snprintf(buf1, 64, "(S%d != S%d)%s", has_second ? buf2 : "");
				buf1[63] = 0;

				os << buf1;
			}}}
	}

}

template <class answer>
class algorithm_biermann {
	protected: // data

	public:	// types

	public: // methods
		algorithm_biermann(knowledgebase<answer> * base, logger * log, int alphabet_size)
		{{{
			this->alphabet_size = alphabet_size;
			this->set_logger(log);
			this->set_knowledge_source(NULL, base);
		}}}
		virtual ~algorithm_biermann()
		{
			// nothing.
			return;
		}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			// we dont care
			return;
		}}}

		virtual void set_knowledge_source(teacher<answer> *teach, knowledgebase<answer> *base)
		// throw error to logger if teacher is set
		{{{
			if(teach) {
				(*this->my_logger)(LOGGER_ERROR, "algorithm_biermann does not support teachers, as it is an offline-algorithm. please either use an online-algorithm like angluin or use a knowledgebase.\n");
				my_teacher = NULL;
				this->my_knowledge = NULL;
			}
				my_teacher = NULL;
				this->my_knowledge = base;
			}
		}}}

		virtual void get_memory_statistics(statistics & stats)
		{
		}

		virtual bool sync_to_knowledgebase()
		{{{
			(*this->my_logger)(LOGGER_ERROR, "algorithm_biermann does not support undo operations, as it is an offline-algorithm.\n");
			return false;
		}}}

		virtual bool supports_sync()
		{{{
			return false;
		}}}

		/*
		 * format for serialization:
		 * all values in NETWORK BYTE ORDER!
		 * <serialized learning algorithm data>
		 *	length of string (excluding this length field; not in bytes but in int32_t)
		 *	type of learning algorithm (see enum learning_algorithm::algorithm)
		 *	algorithm-specific data
		 * </serialized learning algorithm data>
		 */
		virtual basic_string<int32_t> serialize()
		{
		}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
		}

		virtual void print(ostream &os)
		{
		}
		virtual string tostring()
		{
		}

		// conjecture is always ready if there is a knowledgebase
		virtual bool conjecture_ready()
		{{{
			return ( (this->my_knowledge != NULL) && (this->my_knowledge->count_answers() > 0) );
		}}}

		// stubs for counterexamples will throw a warning to the logger
		virtual void add_counterexample(list<int>, answer)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "algorithm_biermann does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		}}}
		virtual void add_counterexample(list<int>)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "algorithm_biermann does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		}}}

	protected:
		virtual bool complete()
		{{{
			// we're offline.
			return true;
		}}}
		// derive an automaton from data structure
		virtual bool derive_automaton(finite_language_automaton * automaton)
		{{{
			int mdfa_size;
			int lfdfa_size;

			set<biermann::clause> clauses;

			vector<int> current_solution;
			vector<int> old_solution;

			// 1) create clauses from LFDFA (knowledgebase)
			create_clauses(clauses);

			lfdfa_size = this->my_knowledge->count_answers();

			// 2) find CSP solution
			bool solved = false;
			bool failed_before = false;
			bool success_before = false;
			// pick an initial mDFA size [FIXME]
			mdfa_size = sqrt((float)lfdfa_size);
			while(!solved) {
				old_solution = current_solution;

				if( solve_clauses(lfdfa_size, mdfa_size, clauses, current_solution) ) {
					if(failed_before) {
						// we found the minimal solution
						solved = true;
					} else {
						success_before = true;
						mdfa_size--;
					}
				} else {

					if(success_before) {
						// we found the minimal solution in the iteration before.
						current_solution = old_solution;
						solved = true;
					} else {
						failed_before = true;
						mdfa_size++;
					}
				}
			}

			// 3) derive automaton from current_solution and mdfa_size
			return create_automaton(mdfa_size, current_solution, automaton);
		}}}

		virtual void create_clauses(set<biermann::clause> clauses)
		{{{
			clauses.clear();

			biermann::clause clause;
			knowledgebase::iterator ki1, ki2;
			knowledgebase::node suffix1, suffix2;

			for(ki1 = this->my_knowledge->begin(); ki1 != this->my_knowledge->end(); ++ki1) {
				if(!ki1->is_answered())
					continue;

				ki2 = ki1; ++ki2;
				while(ki2 != this->my_knowledge->end()) {
					if(!ki2->is_answered())
						continue;

					// C1: O(u) != O(u') => S_u != S_u'
					if(ki1->get_answer() == ki2->get_answer()) {
						clause.l1 = ki1->get_nodeid();
						clause.l2 = ki2->get_nodeid();
						clause.has_second = false;
						clauses.insert(clause);
					}

					// C2: a \in \Sigma,
					//     ua \in L(knowledgebase),
					//     u'a \in L(knowledgebase)
					//     => ( S_u != S_u' ) \or ( S_{ua} = S_{u'a} )
					for(int s = 0; s < this->get_alphabet_size(); s++) {
						// FIXME: if normalizer, normalize suffixes and
						// find corresponding node!

						suffix1 = ki1->get_child(s);
						if(suffix1 == NULL)
							continue;
						if(!suffix1->is_answered())
							continue;

						suffix2 = ki2->get_child(s);
						if(suffix2 == NULL)
							continue;
						if(!suffix2->is_answered())
							continue;

						if(suffix1->get_answer() == suffix2->get_answer()) {
							clause.l1 = ki1->get_nodeid();
							clause.l2 = ki2->get_nodeid();
							clause.has_second = true;
							clause.l3 = suffix1->get_nodeid();
							clause.l4 = suffix2->get_nodeid();
							clauses.insert(clause);
						}
					}

					// next pair.
					++ki2;
				}
			}
		}}}
		virtual bool solve_clauses(int lfdfa_size, int mdfa_size, set<biermann::clause> & clauses, vector<int> & solution)
		{
		}
		virtual bool create_automaton(int size, vector<int> & solution, finite_language_automaton * automaton)
		{
		}


};

}; // end namespace libalf

#endif // __libalf_algorithm_biermann_h__

