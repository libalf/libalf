/*
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *                and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * (c) 2012 Daniel Neider Lehrstuhl, Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 *
 * Author: Daniel Neider (neider@automata.rwth-aachen.de),  Florian Richter
 *
 */

/**
 * @file
 * This file implements a variant of Angluin's learning algorithm that works
 * with unexperienced teacher. Such a teacher answers membership queries with
 * "yes", "no", and "don't care". On an equivalence query, the teacher checks
 * whether a given deterministic finite automaton (DFA) works correct on all
 * words classified as "yes" and "no"; the behavior on "don't cares" is ignored.
 *
 * The result of this algorithm is a minimal DFA that passes the equivalence
 * check. Internally, the algorithm uses an automaton inferring algorithm, which
 * in turn utilizes a SAT or SMT solver. Thus, be prepared of longer runtimes.
 *
 * The algorithm is comprehensively described in
 *
 * - O. Grinchtein, M. Leucker, and N. Piterman. Inferring network invariants
 *   automatically. In IJCAR 2006, LNAI 4130, pp. 483-497. Springer, 2006.
 *
 * - M. Leucker and D. Neider. Learning Minimal Deterministic Finite Automata
 *   from Inexperienced Teachers. In ISoLA 2012, LNCS 7609, pp. 524-538.
 *   Springer, 2012.
 */

#ifndef __libalf_algorithm_angluin_inexperienced_teacher_h__
#define __libalf_algorithm_angluin_inexperienced_teacher_h__

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <ostream>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <libalf/answer.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>
#include <libalf/algorithm_angluin.h>
#include "libalf/algorithm_automata_inferring.h"

namespace libalf {

/**
 * This class implements an Angluin-based version of a learning algorithm that
 * works with inexperienced teachers. The result is a (non necessarily unique)
 * minimal DFA (with respect to the number of states) that passes the teacher's
 * equivalence check.
 *
 * This algorithm changes Angluin's original algorithm in two aspects: 
 * - First, the algorithm works with weak_bool values encoding the values "yes",
 *   "no", and "don't care". By using weak_bools, the closedness and consistency
 *   checks become a check for weak-closedness and weak-consistency (see the
 *   cited papers above).
 *
 * - Second, a (passive) DFA inferring algorithms is utilized to derive a
 *   conjecture. 
 *
 * Thanks to the implementation of weak_bool, this algorithm does only need to
 * replace the derive_conjecture() method to work with inexperienced teacher.
 * Using weak_bools, the original is_closed() and is_consistent() methods are
 * directly turned into the the weak versions.
 *
 * @author Daniel Neider (neider@automata.rwth-aachen.de)
 * @version 1.0
 */
//template <class automata_inferring>
class angluin_inexperienced_teacher : public angluin_simple_table<weak_bool> {

	private:

	/**
	 * Stores a ponter to the DFA inferring algorithm.
	 */
	automata_inferring<bool> * inferring_algorithm;
	
	public:
	
	/**
	 * Creates an empty algorithm. This migt be useful if you want to set
	 * all parameters later.
	 */
	angluin_inexperienced_teacher() : angluin_simple_table<weak_bool>() {
		
		this->inferring_algorithm = NULL;

	}

	angluin_inexperienced_teacher(knowledgebase<weak_bool> * base, logger * log, int alphabet_size, automata_inferring<bool> * inferring_algorithm) : angluin_simple_table<weak_bool>(base, log, alphabet_size) {

		this->inferring_algorithm = inferring_algorithm;

	}

	/**
	 * Sets the internal DFA inferring algorithm used to derive conjectures.
	 *
	 * @param inferring_algorithm The new DFA inferring algorithm to use
	 */
	void set_inferring_algorithm(automata_inferring<bool> * inferring_algorithm) {

		this->inferring_algorithm = inferring_algorithm;

	}

	/**
	 * Derives a smallest deterministic finite automaton from a (not
	 * necessarily weakliy closed and weakly consistent) observation table
	 * using a DFA inferring algorithm.
	 *
	 * The idea is to use the non-unknown entries in the table as a finite
	 * sample and run a passive inferring algorithm to obtain a smallest
	 * DFA (with respect to the number of states) that is consistent with
	 * this sample and, hence, with the data in the table.
	 *
	 * @return The derived conjecture.
	 */
	virtual conjecture * derive_conjecture() {
		
		/*
		 * First, check whether a DFA inferring algorithm has been
		 * selected.
		 */
		if(!inferring_algorithm) {
			(*this->my_logger)(LOGGER_ERROR, "No inferring algorithm is given.\n");
			return NULL;
		}
		assert(inferring_algorithm != NULL);

		// Create knowledgebase 
		knowledgebase<bool> base;
		
		/*
		 * Copy knowledge from Angluin table to internal knowledgebase
		 */
		typename std::list<algorithm_angluin::simple_row<weak_bool, std::vector<weak_bool> > >::const_iterator ti;
		typename std::vector<std::list<int> >::const_iterator ci;
		typename std::vector<weak_bool>::const_iterator acci;

		// Copy upper table
		for(ti = this->upper_table.begin(); ti != this->upper_table.end(); ti++) {
		
			for(acci = ti->acceptance.begin(), ci = column_names.begin(); acci != ti->acceptance.end() && ci != column_names.end(); acci++, ci++) {
				
				assert(!(acci == ti->acceptance.end() || ci == column_names.end()));			

				if(!acci->is_unknown()) {
				
					std::list<int> word = ti->index;
					word.insert(word.end(), ci->begin(), ci->end());

					if(acci->is_true()) {
						base.add_knowledge(word, true);
					} else {
						base.add_knowledge(word, false);
					}
				
				}
				
			}
		
		}
		// Copy lower table
		for(ti = this->lower_table.begin(); ti != this->lower_table.end(); ti++) {
		
			for(acci = ti->acceptance.begin(), ci = column_names.begin(); acci != ti->acceptance.end() && ci != column_names.end(); acci++, ci++) {
				
				assert(!(acci == ti->acceptance.end() || ci == column_names.end()));			

				if(!acci->is_unknown()) {
				
					std::list<int> word = ti->index;
					word.insert(word.end(), ci->begin(), ci->end());

					if(acci->is_true()) {
						base.add_knowledge(word, true);
					} else {
						base.add_knowledge(word, false);
					}
				
				}
				
			}
		
		}
		
		/*
		 * Prepare inferring algorithm
		 */
		inferring_algorithm->set_alphabet_size(this->alphabet_size);
		inferring_algorithm->set_knowledge_source(&base);
		inferring_algorithm->set_logger(this->my_logger);

		// Infer
		conjecture * result = this->inferring_algorithm->derive_conjecture();
		
		return result;
		
	}

};

}; // end of namespace libalf

#endif

