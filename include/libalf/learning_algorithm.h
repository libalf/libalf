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

#include <libalf/teacher.h>
#include <libalf/structured_query_tree.h>
#include <libalf/automata.h>
#include <libalf/logger.h>
#include <libalf/normalizer.h>

namespace libalf {

using namespace std;

// basic interface for different implementations (e.g. one table and one tree)
template <class answer>
class learning_algorithm {

	public:

		enum algorithm {
			ALG_NONE = 0,
			ALG_ANGLUIN = 1,
			ALG_BIERMANN = 2,
			ALG_BIERMANN_ANGLUIN = 3,
			ALG_RFSA = 4
		};

		virtual ~learning_algorithm() { };

		// set_alphabet_size() is only for initial setting.
		// once any data is in the structure, use increase_alphabet_size ONLY.
		virtual void set_alphabet_size(int alphabet_size) = 0;
		virtual int get_alphabet_size() = 0;

		virtual void increase_alphabet_size(int new_asize) = 0;

		// if teacher is NULL (or unset), advance() will return a
		// structured query tree. an answer to a structured query tree
		// has to be given via learn_from_structured_query()
		virtual void set_teacher(teacher<answer> *) = 0;
		virtual teacher<answer> * get_teacher() = 0;
		virtual void unset_teacher() = 0;

		virtual void set_logger(logger *) = 0;
		virtual logger * get_logger() = 0;

		virtual void set_normalizer(normalizer * norm) = 0;
		virtual normalizer * get_normalizer() = 0;
		virtual void unset_normalizer() = 0;

		virtual void get_memory_statistics(statistics & stats) = 0;

		virtual void undo() = 0;
		virtual void redo() = 0;

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

		// check acceptance of word in data structure
		// if status unknown, return (false, ?)
		// otherwise return (true, <answer>)
//		virtual pair<bool, answer> check_entry(list<int>) = 0;

		// check if a hypothesis can be constructed without any further queries
		virtual bool conjecture_ready() = 0;

		// complete table and then derive automaton:
		// if not using a teacher:
		//	will either return an SQT or NULL in case that an automaton can be constructed.
		//	if so, the automaton will be constructed and stored into *automaton.
		// if using a teacher:
		//	will repeatedly query the stored teacher and then construct an automaton into *automaton.
		//
		// please note that the type of automaton must match the required automaton-type of the used learning algorithm
		// (e.g. angluin required a DFA)
		virtual structured_query_tree<answer> * advance(finite_language_automaton * automaton) = 0;

		// if no teacher was given, use this function to answer the SQT returned by advance()
		virtual bool learn_from_structured_query(structured_query_tree<answer> &) = 0;

		// in case the hypothesis is wrong, use this function to give a counter-example
		virtual void add_counterexample(list<int>, answer) = 0;
		virtual void add_counterexample(list<int>) = 0;

	protected:
		// complete table in such a way that an automata can be derived
		virtual structured_query_tree<answer> * complete() = 0;
		// derive an automaton from data structure
		virtual bool derive_automaton(finite_language_automaton * automaton) = 0;
};

}; // end namespace libalf

#endif

