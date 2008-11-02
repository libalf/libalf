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

#include <list>
#include <utility>

#include <libalf/teacher.h>

#include <libalf/automata.h>

namespace libalf {

using namespace std;

// basic interface for different implementations (e.g. one table and one tree)
template <class answer>
class learning_algorithm {

	public:
		virtual ~learning_algorithm() { };

		virtual void set_alphabet_size(int alphabet_size) = 0;
		virtual int get_alphabet_size() = 0;

		virtual void set_teacher(teacher<answer> *) = 0;
		virtual teacher<answer> * get_teacher() = 0;
		virtual void set_logger(logger *) = 0;
		virtual logger * get_logger() = 0;

		virtual void undo() = 0;
		virtual void redo() = 0;

		virtual void savetofile(char* filename) = 0;
		virtual void loadfromfile(char* filename) = 0;

		virtual void print(ostream &os) = 0;

		// check acceptance of word in data structure
		// if status unknown, return (false, ?)
		// otherwise return (true, <answer>)
		virtual pair<bool, answer> check_entry(list<int>) = 0;

		// complete table and then derive automaton
		virtual bool derive_hypothesis(finite_language_automaton * automaton) = 0;

		virtual void add_counterexample(list<int>, answer) = 0;
		virtual void add_counterexample(list<int>) = 0;

	protected:
		// complete table in such a way that an automata can be derived
		virtual void complete() = 0;
		// derive an automaton from data structure
		virtual bool derive_automaton(finite_language_automaton * automaton) = 0;
};

}; // end namespace libalf

#endif

