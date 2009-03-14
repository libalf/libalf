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
#include <map>

#include <math.h>

#include <libalf/knowledgebase.h>
#include <libalf/automata.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

template <class answer>
class simple_biermann : public learning_algorithm<answer> {
	protected: // data

	public:	// types
		class node_comparator {
			bool operator() (typename knowledgebase<answer>::node * a, typename knowledgebase<answer>::node * b)
			{ return a < b; };
		};
		typedef typename knowledgebase<answer>::node* knowledgebase_node_ptr;

		typedef map<knowledgebase_node_ptr, int, node_comparator> mapping;

		class clause {
			// very reduced clause: can only represent the following formulas:
			// l1 != l2
			// and
			// l1 != l2 || l3 == l4
			public:
				knowledgebase_node_ptr l1, l2, l3, l4;
				bool has_second;

				pair<bool, bool> verify(mapping & solution)
				// return <true, true> if all (required) information is already set and the clause is true.
				// return <true, false> if all information is already set and the clause is false.
				// return <false, ignore> if information is missing.
				{{{
					pair<bool, bool> ret;
					typename mapping::iterator mi1, mi2;

					if(has_second) {
						mi1 = solution.find(l3);
						if(mi1 == solution.end())
							goto incomplete;

						mi2 = solution.find(l4);
						if(mi2 == solution.end())
							goto incomplete;

						if(mi1->second == mi2->second) {
							ret.first = true;
							ret.second = true;
							return ret;
						}
					}

					mi1 = solution.find(l1);
					if(mi1 == solution.end())
						goto incomplete;

					mi2 = solution.find(l2);
					if(mi2 == solution.end())
						goto incomplete;

					ret.first = true;
					ret.second = (mi1->second != mi2->second);
					return ret;
				incomplete:
					ret.first = false;
					ret.second = false;
					return ret;
				}}}

				void print(ostream &os)
				{{{
					char buf1[64];
					char buf2[64];
					list<int> word;
					string s1,s2;

					word = l1->get_word();
					s1 = word2string(word, '.');
					word = l2->get_word();
					s2 = word2string(word, '.');

					snprintf(buf1, 64, "(%s != %s)", s1.c_str(), s2.c_str());
					if(has_second) {
						word = l3->get_word();
						s1 = word2string(word, '.');
						word = l4->get_word();
						s2 = word2string(word, '.');
						snprintf(buf2, 64, " || (%s == %s)", s1.c_str(), s2.c_str());
					}
					buf1[63] = 0;
					buf2[63] = 0;

					os << buf1;
					if(has_second)
						os << buf2;
				}}}
		};

	public: // methods
		simple_biermann(knowledgebase<answer> * base, logger * log, int alphabet_size)
		{{{
			this->alphabet_size = alphabet_size;
			this->set_logger(log);
			this->set_knowledge_source(NULL, base);
		}}}
		virtual ~simple_biermann()
		{{{
			// nothing.
			return;
		}}}

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
				this->my_teacher = NULL;
				this->my_knowledge = NULL;
			} else {
				this->my_teacher = NULL;
				this->my_knowledge = base;
			}
		}}}

		virtual void get_memory_statistics(statistics & stats)
		{ /* FIXME: maybe keep some stats from last run? */ }

		virtual bool sync_to_knowledgebase()
		{{{
			(*this->my_logger)(LOGGER_ERROR, "algorithm_biermann does not support undo operations, as it is an offline-algorithm.\n");
			return false;
		}}}

		virtual bool supports_sync()
		{{{
			return false;
		}}}

		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;

			// we don't have any internal data
			ret += htonl(1);
			ret += htonl(learning_algorithm<answer>::ALG_BIERMANN);

			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			if(ntohl(*it) != 1)
				return false;
			it++;

			if(ntohl(*it) != learning_algorithm<answer>::ALG_BIERMANN)
				return false;
			it++;

			return true;
		}}}

		virtual void print(ostream &os)
		{{{
			os << "biermann is an offline algorithm that does not have any persistent data structures.\n";
		}}}
		virtual string tostring()
		{{{
			string ret;
			ret = "biermann is an offline algorithm that does not have any persistent data structures.\n";
			return ret;
		}}}

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
		{
			int mdfa_size;
			int lfdfa_size;

			list<clause> clauses;

			mapping current_solution;
			mapping old_solution;

			if(this->my_knowledge == NULL)
				return false;

			// 1) create clauses from LoopFreeDFA (knowledgebase)
			create_clauses(clauses);
print_clauses(clauses);
return true;

			lfdfa_size = this->my_knowledge->count_answers();

			// 2) find CSP solution
			bool solved = false;
			bool failed_before = false;
			bool success_before = false;
			// pick an initial mDFA size [FIXME]
			mdfa_size = (int)sqrtf((float)lfdfa_size);
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
		}

		virtual void create_clauses(list<clause> & clauses)
		{{{
			clause clause;
			typename knowledgebase<answer>::iterator ki1, ki2;
			typename knowledgebase<answer>::node *suffix1, *suffix2;

			clauses.clear();

			for(ki1 = this->my_knowledge->begin(); ki1 != this->my_knowledge->end(); ++ki1) {
				if(!ki1->is_answered())
					continue;

				ki2 = ki1;
				++ki2;
				while(ki2 != this->my_knowledge->end()) {
					if(!ki2->is_answered())
						continue;

					// C1: O(u) != O(u') => S_u != S_u'
					if(ki1->get_answer() != ki2->get_answer()) {
						clause.l1 = ki1->get_selfptr();
						clause.l2 = ki2->get_selfptr();
						clause.has_second = false;
						clauses.push_back(clause);
					}

					// C2: a \in \Sigma,
					//     ua \in L(knowledgebase),
					//     u'a \in L(knowledgebase)
					//     => ( S_u != S_u' ) \or ( S_{ua} = S_{u'a} )
					for(int s = 0; s < this->get_alphabet_size(); s++) {
						// FIXME: if normalizer, normalize suffixes and
						// find corresponding node!

						suffix1 = ki1->find_child(s);
						if(suffix1 == NULL)
							continue;
						if(!suffix1->is_answered())
							continue;

						suffix2 = ki2->find_child(s);
						if(suffix2 == NULL)
							continue;
						if(!suffix2->is_answered())
							continue;

						if(suffix1->get_answer() == suffix2->get_answer()) {
							clause.l1 = ki1->get_selfptr();
							clause.l2 = ki2->get_selfptr();
							clause.has_second = true;
							clause.l3 = suffix1->get_selfptr();
							clause.l4 = suffix2->get_selfptr();
							clauses.push_back(clause);
						}
					}

					// next pair.
					++ki2;
				}
			}
		}}}
		virtual bool solve_clauses(int lfdfa_size, int mdfa_size, list<clause> & clauses, mapping & solution)
		{
			// FIXME
			return false;
		}
		virtual bool create_automaton(int size, mapping & solution, finite_language_automaton * automaton)
		{
			// FIXME
			return false;
		}

		virtual void print_clauses(list<clause> & clauses)
		{{{
			typename list<clause>::iterator ci;
			int n;

			cout << "clauses {\n";
			for(n=0, ci = clauses.begin(); ci != clauses.end(); n++, ci++) {
				printf("\t#%03d: ", n);
				ci->print(cout);
				printf("\n");
			}
			cout << "}\n";
		}}}

};

}; // end namespace libalf

#endif // __libalf_algorithm_biermann_h__

