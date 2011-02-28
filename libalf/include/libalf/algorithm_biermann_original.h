/* $Id: algorithm_biermann_minisat.h 1000 2009-10-13 10:09:11Z davidpiegdon $
 * vim: fdm=marker
 *
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
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *         David R. Piegdon <david-i2@piegdon.de>
 *
 */

/*
 * BIERMANN is an offline learning algorithm as described in
 *	* A.W. Biermann, J.A. Feldman - On the Synthesis of Finite-State Machines from Samples of their Behavior
 *
 * this implementation supports almost arbitrary <answer> types.
 */

#include <iostream>
#include <string>
#include <map>
#include <list>

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>

#ifndef MAX
# define MAX(a,b) ((a)>(b)?(a):(b))
#endif

namespace libalf {

template <class answer>
class original_biermann : public learning_algorithm<answer> {
	private:
	unsigned int nondeterminism;

	public:
	typedef typename knowledgebase<answer>::node node;

	original_biermann(knowledgebase<answer> *base, logger *log, int alphabet_size, unsigned int nondeterminism) {
std::cerr << "original_biermann constructed.\n";
		this->set_alphabet_size(alphabet_size);
		this->set_logger(log);
		this->set_knowledge_source(base);
		this->nondeterminism = nondeterminism;
	}

	virtual enum learning_algorithm_type get_type() const
	{ return ALG_BIERMANN_ORIGINAL; };

	virtual enum learning_algorithm_type get_basic_compatible_type() const
	{ return ALG_BIERMANN_ORIGINAL; };

	void increase_alphabet_size(int new_alphabet_size) {
		this->alphabet_size = new_alphabet_size;
	}

	void set_nondeterminism(unsigned int new_nondeterminism) {
		nondeterminism = new_nondeterminism;
	}

	unsigned int get_nondeterminism() {
		return nondeterminism;
	}

	memory_statistics get_memory_statistics() const {
		// get_memory_statistics() is obsolete and will be removed in the future.
		// use receive_generic_statistics() instead.
		memory_statistics ret;
		return ret;
	}

	virtual void receive_generic_statistics(generic_statistics & stat) const
	{
		
	}

	bool sync_to_knowledgebase() {
		return true;
	}

	bool supports_sync() const {
		return true;
	}

	bool conjecture_ready() {
		if(this->my_knowledge != NULL)
			return true;
		else return false;
	}

	bool complete() {
		// we're offline.
		return true;
	}

	bool add_counterexample(std::list<int>) {
		(*this->my_logger)(LOGGER_ERROR, "biermann does not support counter-examples, as it is an offline-algorithm. Please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		return false;
	}

	conjecture* derive_conjecture() {
		// Create result
		moore_machine<answer> * automaton;
		if(typeid(answer) == typeid(bool))
			automaton = dynamic_cast<moore_machine<answer>*>(new finite_automaton);
		else
			automaton = new moore_machine<answer>;
		automaton->input_alphabet_size = this->alphabet_size;
		automaton->valid = true;

		// Create temporary variables
		std::map<int, node*> *eq_classes = new std::map<int, node*>;

		// Add sink state
		eq_classes->insert(std::pair<int, node*>(0, NULL));
		automaton->state_count++;
		for(int i=0; i<this->alphabet_size; i++)
			automaton->transitions[0][i].insert(0);
		if(typeid(answer) == typeid(bool))
			automaton->output_mapping[0] = false;

		// Compute conjecture
		int initial_state;
		initial_state = assign_equivalence_class(this->my_knowledge->get_rootptr(), automaton, eq_classes);
		automaton->initial_states.insert(initial_state);

		// Calc determinism
		automaton->calc_determinism();
		
		// Clean temporary variables
		delete eq_classes;

		// Return result
		return automaton;
	}

	std::basic_string<int32_t> serialize() const {
		std::basic_string<int32_t> ret;

		ret += ::serialize(2); // size
		ret += ::serialize(ALG_BIERMANN_ORIGINAL);
		ret += ::serialize(nondeterminism);

		return ret;
	}

	bool deserialize(serial_stretch & serial) {
		int s;

		if(!::deserialize(s, serial)) return false;
		if(s != 2) return false;
		if(!::deserialize(s, serial)) return false;
		if(s != ALG_BIERMANN_ORIGINAL) return false;
		return ::deserialize(nondeterminism, serial);
	}

	bool deserialize_magic(serial_stretch & serial, std::basic_string<int32_t> & result)
	// you may use magic to set the value of nondeterminism:
	//
	// expected data:
	//	bool: set?
	// only if above was true:
	//	nondeterminism [unsigned int]
	//
	// returns: (always)
	//	nondeterminism [unsigned int] (current value)
	//
	// added 2010-02-06 by David R. Piegdon
	{
		result.clear();
		if(serial.empty()) return false;

		if(ntohl(*serial)) {
			unsigned int val;
			++serial;
			if(serial.empty()) return false;
			val = ntohl(*serial);
			if(val < 1)
				this->set_nondeterminism(1);
			else
				this->set_nondeterminism(val);
		}

		++serial;

		result += htonl((int) nondeterminism);

		return serial.empty();
	}

	void print(std::ostream &os) const {
		os << "Biermann and Feldmann's offline algorithm. Alphabet size is " << this->alphabet_size;
		os << " Nondeterminism is " + nondeterminism << std::endl;
	}

	private:

	/*
	 * Computes a fingerprint of a node with respect to a given set of equivalence classes.
	 * If a new equivalence class is discovered, a new class is created and added to the
	 * set.
	 *
	 */
	int compute_fingerprint(node *current, std::map<int, node*> *eq_classes) {

		/*
		 * There is no equivalence class yet.
		 */
		if(eq_classes->empty()) {
			eq_classes->insert(std::pair<int, node*>(0, current));
			return 0;
		}

		/*
		 * Search whether the equivalence class belonging to the current node is already existing.
		 * If so, return its number.
		 */
		typename std::map<int, node*>::iterator it;
		for (it=eq_classes->begin() ; it!=eq_classes->end(); it++ ) {
			if(is_equivalent(current, (*it).second, nondeterminism)) {
				// An equivalence class has been found
				#ifdef DEBUG
					std::cout << "Equivalent to class " << it->first << std::endl;
				#endif
				return (*it).first;
			}

			#ifdef DEBUG
				else {
					std::cout << "NOT equivalent to class " << it->first << std::endl;
				}
			#endif
		}

		/*
		 * Create a new equivalence class
		 */
		int new_eq_number;
		new_eq_number = eq_classes->size();
		eq_classes->insert(std::pair<int, node*>(new_eq_number, current));

		return new_eq_number;
	}

	/*
	 * Check whether two nodes are in the same equivalence class. This is done by comparing
	 * both nodes and their children recursivley up to depth level.
	 */
	bool is_equivalent(node *n1, node *n2, int level) {

		/*
		 * Both nodes are NULL
		 */
		if(n1 == NULL && n2 == NULL) {
			#ifdef DEBUG
				std::cout << "EQ-check: Both are NULL" << std::endl;
			#endif
			return true;
		}

		/*
		 * Exactly one node is NULL. We swap both variables such that n2 == NULL.
		 */
		else if((n1 == NULL && n2 != NULL) || (n1 != NULL && n2 == NULL)) {
			#ifdef DEBUG
				std::cout << "EQ-check: One is NULL" << std::endl;
			#endif
			if(n1 == NULL) {
				// Swap
				n1 = n2;
				n2 = NULL;
			}

			// If n1 is answered then the nodes are not equivalent
			if(n1->is_answered())
				return false;

			if(level == 0)
				return true;

			// Check the children of n1 (which have to be unanswered in any case)
			for(int i=0; i<MAX(this->alphabet_size, n1->max_child_count()); i++) {
				node *child;
				child = n1->find_child(i);
				if(!is_equivalent(child, NULL, level - 1))
					return false;
			}

			return true;
		}

		/*
		 * Both nodes are not NULL
		 */
		else {
			#ifdef DEBUG
				std::cout << "EQ-check: Both are NOT NULL" << std::endl;
			#endif

			if((n1->is_answered() && !n2->is_answered()) || (!n1->is_answered() && n2->is_answered())) {
				return false;
			} else if(n1->is_answered() && n2->is_answered()) {
				if(n1->get_answer() != n2->get_answer())
					return false;
			}

			// End of recursion?
			if(level == 0)
				return true;

				int max;
				max = MAX(this->alphabet_size, n1->max_child_count());
				max = MAX(max, n2->max_child_count());

				for(int i=0; i<max; i++) {
				// Get child i
				node *child1, *child2;
				child1 = n1->find_child(i);
				child2 = n2->find_child(i);

				if(!is_equivalent(child1, child2, level-1))
					return false;
			}

			return true;
		}
	}

	/*
	 * Recursivley computing a nodes equivalence class.
	 */
	int assign_equivalence_class(node *current, moore_machine<answer> * automaton, std::map<int, node*> *eq_classes) {

		/*
		 * Recursively compute the equivalence classes of the current node's children
		 */
		int child_eq[this->alphabet_size];
		for(int i=0; i<this->alphabet_size; i++) {
			// Get child i
			node *child;
			child = current->find_child(i);

			// If this child exists, then compute equivalence class number recursively
			if(child != NULL) {
				int child_eq_nr;
				child_eq_nr = assign_equivalence_class(child, automaton, eq_classes);

				// Remember the child's eq class to proprly set the transitions
				child_eq[i] = child_eq_nr;
			}

			// If no child exists, point to the sink state
			else {
				child_eq[i] = 0;
			}
		}

		/*
		 * Compute equivalence class of current node
		 */
		int current_eq;
		current_eq = compute_fingerprint(current, eq_classes);

		// DEBUG
#ifdef DEBUG
		print_current(current);
		std::cout << "Assigned equivalence class: " << current_eq << std::endl;
#endif

		/*
		 * Update conjecture, i.e. add transitions from current equivalence class
		 */
		// State
		if(automaton->state_count <= current_eq)
			automaton->state_count++;
		// Output of states
		if(current->is_answered()) {
			automaton->output_mapping[current_eq] = current->get_answer();
		} else {
			if(typeid(answer) == typeid(bool))
				automaton->output_mapping[current_eq] = false;
		}
			
		// Transitions
		for(int i=0; i<this->alphabet_size; i++) {
			automaton->transitions[current_eq][i].insert(child_eq[i]);
#ifdef DEBUG
			std::cout << "Adding transition (" <<current_eq << ", " << i << ", " << child_eq[i] << ")" << std::endl;
#endif
		}

		return current_eq;
	}

	void print_current(node *current) {
		std::cout << "Currenty processing: '";

		std::list<int> w =  current->get_word();

		for (std::list<int>::iterator it = w.begin(); it != w.end(); it++) {
			std::cout << *it << " ";
		}
		std::cout << "'" << std::endl;
	}
};

}
