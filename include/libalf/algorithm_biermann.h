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

#ifndef __libalf_algorithm_biermann_h__
# define __libalf_algorithm_biermann_h__

#include <string>
#include <list>

#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

// basic interface for different implementations (e.g. one table and one tree)
template <class answer>
class learning_algorithm {
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
		{
			// we dont care
			return;
		}

		virtual void set_knowledge_source(teacher<answer> *teach, knowledgebase<answer> *base)
		// set a source for all membership information. either use teacher or knowledgebase.
		// teacher will overrule knowledgebase.
		{{{
			if(teach) {
				(*this->my_logger)(LOGGER_ERROR, "algorithm_biermann does not support teachers, as it is an offline-algorithm. please either use an online-algorithm like angluin or use a knowledgebase.\n");
				my_teacher = NULL;
				my_knowledge = NULL;
			}
				my_teacher = NULL;
				my_knowledge = base;
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

		// check if a hypothesis can be constructed without any further queries
		virtual bool conjecture_ready()
		{{{
			// we're offline.
			return true;
		}}}

		// in case the hypothesis is wrong, use this function to give a counter-example
		virtual void add_counterexample(list<int>, answer)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "algorithm_biermann does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		}}}
		virtual void add_counterexample(list<int>)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "algorithm_biermann does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		}}}

	protected:
		// complete table in such a way that an automata can be derived
		// return true if table is complete.
		// return false if table could not be completed due to missing knowledge
		virtual bool complete()
		{{{
			// we're offline.
			return;
		}}}
		// derive an automaton from data structure
		virtual bool derive_automaton(finite_language_automaton * automaton)
		{
		}
};

}; // end namespace libalf

#endif // __libalf_algorithm_biermann_h__

