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

#ifndef __libalf_learning_algorithm_h__
# define __libalf_learning_algorithm_h__

#include <string>
#include <list>
#include <utility>

#include <libalf/logger.h>
#include <libalf/teacher.h>
#include <libalf/knowledgebase.h>
#include <libalf/normalizer.h>
#include <libalf/automata.h>

namespace libalf {

using namespace std;

// basic interface for different implementations (e.g. one table and one tree)
template <class answer>
class learning_algorithm {
	protected: // data
		ignore_logger ignore;
		logger * my_logger;

		teacher<answer> * my_teacher;
		knowledgebase<answer> * my_knowledge;

		normalizer * norm;

	public:	// types
		enum algorithm {
			ALG_NONE = 0,
			ALG_ANGLUIN = 1,
			ALG_BIERMANN = 2,
			ALG_BIERMANN_ANGLUIN = 3,
			ALG_RFSA = 4
		};

	public: // methods
		learning_algorithm()
		{{{
			my_logger = &(this->ignore);
			my_teacher = NULL;
			my_knowledge = NULL;
			norm = NULL;
		}}}
		virtual ~learning_algorithm() { };

		// set_alphabet_size() is only for initial setting.
		// once any data is in the structure, use increase_alphabet_size ONLY.
		virtual void set_alphabet_size(int alphabet_size) = 0;
		virtual int get_alphabet_size() = 0;

		virtual void increase_alphabet_size(int new_asize) = 0;


		virtual void set_logger(logger * l)
		{{{
			my_logger = l;
		}}}

		virtual void set_knowledge_source(teacher<answer> *teach, knowledgebase<answer> *base)
		// set a source for all membership information. either use teacher or knowledgebase.
		// teacher will overrule knowledgebase.
		{{{
			if(teach) {
				my_teacher = teach;
				my_knowledge = NULL;
			} else {
				my_teacher = NULL;
				my_knowledge = base;
			}
		}}}

		virtual void set_normalizer(normalizer * norm)
		{{{
			if(this->norm)
				delete this->norm;
			this->norm = norm;
		}}}
		virtual normalizer * get_normalizer()
		{{{
			  return norm;
		}}}
		virtual void unset_normalizer()
		{{{
			if(norm)
				delete norm;
			norm = NULL;
		}}}

		virtual void get_memory_statistics(statistics & stats) = 0;

		// knowledgebase supports undo-operations. this callback should be called after
		// an undo operation to inform the algorithm that some knowledge may be obsolete
		// now. the algorithm should check all internal knowledge, remove obsolete and
		// possibly go back in its state (delete rows/columns).
		// please use knowledgebase->get_timestamp() for book-keeping and to check
		// which changes to revert.
		// you may choose not to support undo operations. then supports_sync must return false.
		virtual bool sync_to_knowledgebase() = 0;

		// supports_sync() must return true, if undo/sync is supported. false otherwise.
		virtual bool supports_sync() = 0;

		/*
		 * format for serialization:
		 * all values in NETWORK BYTE ORDER!
		 * <serialized learning algorithm data>
		 *	length of string (excluding this length field; not in bytes but in int32_t)
		 *	type of learning algorithm (see enum learning_algorithm::algorithm)
		 *	algorithm-specific data
		 * </serialized learning algorithm data>
		 */
		virtual basic_string<int32_t> serialize() = 0;
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		virtual void print(ostream &os) = 0;
		virtual string tostring() = 0;

		// check if a hypothesis can be constructed without any further queries
		virtual bool conjecture_ready() = 0;

		// complete table and then derive automaton:
		// if using a knowledgebase:
		//	will resolve required knowledge from knowledgebase. if the knowledge was unknown
		//	it will be marked as required and false will be returned.
		//	knowledgebase needs to be updated by you, then.
		//
		//	if all knowledge was found and an automaton is ready, it will be stored into
		//	automaton and true will be returned.
		// if using a teacher:
		//	will repeatedly query the stored teacher and then construct an automaton into *automaton.
		//	will always return true
		//
		// please note that the type of automaton must match the required automaton-type of the used learning algorithm
		// (e.g. angluin required a DFA)
		virtual bool advance(finite_language_automaton * automaton)
		{{{
			if(complete()) {
				if(!derive_automaton(automaton)) {
					(*my_logger)(LOGGER_ERROR, "learning_algorithm::advance(): derive from completed tabled failed! wrong kind of automaton passed or internal error.\n");
					return false;
				} else {
					return true;
				}
			} else {
				return false;
			}
		}}};

		// in case the hypothesis is wrong, use this function to give a counter-example
		virtual void add_counterexample(list<int>, answer) = 0;
		virtual void add_counterexample(list<int>) = 0;

	protected:
		// complete table in such a way that an automata can be derived
		// return true if table is complete.
		// return false if table could not be completed due to missing knowledge
		virtual bool complete() = 0;
		// derive an automaton from data structure
		virtual bool derive_automaton(finite_language_automaton * automaton) = 0;
};

}; // end namespace libalf

#endif

