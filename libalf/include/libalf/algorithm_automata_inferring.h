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
 * (c) 2008 - 2012 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *             and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

/*
 * PUT Description of algorithm here (minimal automata)
 */

#ifndef __ALGORITHM_AUTOMATA_INFERRING__
#define __ALGORITHM_AUTOMATA_INFERRING__

// Standard includes 
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <assert.h>

// libALF includes
#include "libalf/knowledgebase.h"
#include "libalf/learning_algorithm.h"
#include "libalf/conjecture.h"
#include "libalf/prefix_tree.h"

namespace libalf {

template <class answer>
class automata_inferring : public learning_algorithm<answer> {

	protected:
	
	/**
	 * The default output of a conjecture Moore machine (or DFA) on states that
	 * are not determined by the sample, i.e., because the sample specifies no
	 * word leading to such this state.
	 */
	answer default_output;

	automata_inferring() : learning_algorithm<answer>() {
		default_output = answer();
	}
	
	public:

	/**
	 * By default, we use the linear seach method. with increment 1.
	 */
	conjecture * derive_conjecture() {

		return infer_linear(1);

	}

	conjecture * infer(unsigned int n) const {
	
		// Copy the data from the knowledgebase into our own internal data structure
		prefix_tree<answer> t(*this->my_knowledge, this->alphabet_size);
	
		return __infer(t, n);
		
	}
	
	conjecture * infer_linear(unsigned int increment = 1) const {
	
		// Check increment
		if(increment == 0) {
			(*this->my_logger)(LOGGER_ERROR, "The increment in the linear seach function has to be greater than 0.\n");
			return NULL;
		}
		
		// Copy the data from the knowledgebase into our own internal data structure
		prefix_tree<answer> t(*this->my_knowledge, this->alphabet_size);
		
		unsigned int left = 1;
		unsigned int right = 1;
		
		// First, increase the size until the problem is satisfiable
		conjecture * result = NULL;
		while(result == NULL) {
		
			//std::cout << "left=" << left << ", right= " << right << std::endl;
		
			result = __infer(t, right);
		
			if(result == NULL) {
				
				left = right + 1;
				right += increment;
				
			}
		
		}
		
		// Now, use a binary search to find the minimal value
		while(left < right) {
		
			unsigned int mid = left + (right - left) / 2;
		
			//std::cout << "left=" << left << ", right= " << right << ", mid=" << mid << std::endl;
		
			conjecture * tmp_result = __infer(t, mid);
			
			if(tmp_result == NULL) {
				left = mid + 1;
			} else {
			
				delete result;
				result = tmp_result;
				tmp_result = NULL;
			
				right = mid;
			}
		
		}
		
		assert(result != NULL);
		return result;
	
	}
	
	conjecture * infer_binarysearch() const {

		// Copy the data from the knowledgebase into our own internal data structure
		prefix_tree<answer> t(*this->my_knowledge, this->alphabet_size);
	
		unsigned int left = 1;
		unsigned int right = 1;
	
		// First, double the size until the problem is satisfiable
		conjecture * result = NULL;
		while(result == NULL) {
		
			result = __infer(t, right);
		
			if(result == NULL) {
				
				left = right + 1;
				right *= 2;
				
			}
		
		}
		
		// Now, use a binary search to find the minimal value
		while(left < right) {
		
			unsigned int mid = left + (right - left) / 2;
		
			conjecture * tmp_result = __infer(t, mid);
			
			if(tmp_result == NULL) {
				left = mid + 1;
			} else {
			
				delete result;
				result = tmp_result;
				tmp_result = NULL;
			
				right = mid;
			}
		
		}
		
		assert(result != NULL);
		return result;
	
	}

	/**
	 * Returns the default output used for conjectures if the output of a state
	 * is not determined by the given sample.
	 *
	 * @return Returns the default output used for conjectures.
	 */
	answer get_default_output() {
		return default_output;
	}
	
	/**
	 * Sets the default output used for conjectures if the output of a state
	 * is not determined by the given sample.
	 *
	 * @param new_default_output The new default output used for conjectures
	 */
	void set_default_output(answer new_default_output) {
		this->default_output = new_default_output;
	}
	
	private:
	
	virtual conjecture * __infer(const prefix_tree<answer> & t, unsigned int n) const = 0;

	
	/*
	 * The following code are method stubs for much of libALF's functionality
	 * that is not (yet) supported the the automata inferring algorithms.
	 * That might change in the future.
	 */
	public:
	 
	virtual enum learning_algorithm_type get_type() const
	{ return ALG_BIERMANN_ORIGINAL; };

	virtual enum learning_algorithm_type get_basic_compatible_type() const
	{ return ALG_BIERMANN_ORIGINAL; };

		void increase_alphabet_size(int new_alphabet_size) {
		this->alphabet_size = new_alphabet_size;
	}

	memory_statistics get_memory_statistics() const {
		// get_memory_statistics() is obsolete and will be removed in the future.
		// use receive_generic_statistics() instead.
		memory_statistics ret;
		return ret;
	}

	// Not implemented
	virtual void receive_generic_statistics(generic_statistics __attribute__ ((__unused__)) & stat) const {
		(*this->my_logger)(LOGGER_WARN, "This feature is not implemented.\n");
	}

	// Not implemented
	bool sync_to_knowledgebase() {
		(*this->my_logger)(LOGGER_WARN, "This feature is not implemented.\n");
		return false;
	}

	// Not implemented
	bool supports_sync() const {
		(*this->my_logger)(LOGGER_WARN, "This feature is not implemented.\n");
		return false;
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
		(*this->my_logger)(LOGGER_WARN, "This is an offline-algorithm. Please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		return false;
	}

	// Not implemented
	std::basic_string<int32_t> serialize() const {
		(*this->my_logger)(LOGGER_WARN, "This feature is not implemented.\n");
		return std::basic_string<int32_t>();
	}

	// Not implemented
	bool deserialize(serial_stretch __attribute__ ((__unused__)) & serial) {
		(*this->my_logger)(LOGGER_WARN, "This feature is not implemented.\n");
		return false;
	}

	// Not implemented
	bool deserialize_magic(serial_stretch __attribute__ ((__unused__)) & serial, std::basic_string<int32_t> __attribute__ ((__unused__)) & result) {
		(*this->my_logger)(LOGGER_WARN, "This feature is not implemented.\n");
		return false;
	}
	
};


/**
 * Checks whether a given Moore machine is consistent with a prefix_tree,
 * i.e., whether the Moore machine classifies the words stored in the
 * prefix tree correctly.
 *
 * If the Moore machine is nondeterministic, the method checks whether the
 * output stored in the prefix tree is produced by one of the runs of the
 * Moore machine on the given word.
 *
 * For nondeterministic machines it might happen that no state is reached after
 * reading a word from the sample. In this case, the function compares the
 * output of the sample on this particular word with the default output, which
 * can be specified as a (non-mandatory) parameter.
 *
 * This method is mainly intended for debuging.
 *
 * @param t The prefix tree to compare the Moore machine with
 * @param machine The Moore machine to test
 * @param default_output The default output if no state is reached. Defaults to
 *                       the default constructor of the template class. (For
 *                       bools this is false.)
 *
 * @return Returns whether the given Moore machine agrees on all classified
 *         words represented by the given prefix tree.
 */
template <class answer>
bool is_consistent(const prefix_tree<answer> & t, const moore_machine<answer> & machine, answer default_output = answer()) {

	// Run through all nodes of the prefix tree and check whether the
	// corresponding word is classified correctly.
	std::list<std::pair<unsigned int, std::list<int> > > stack;
	stack.push_back(std::pair<unsigned int, std::list<int> >(prefix_tree<answer>::root, std::list<int>()));
	while(!stack.empty()) {
	
		// Get current node
		std::pair<unsigned int, std::list<int> > back = stack.back();
		unsigned int current_node = back.first;
		std::list<int> current_word = back.second;
		stack.pop_back();
	
		// Check current node
		if(t.specified[current_node]) {
			
			// Get the states reached by the machine on the given word
			std::set<int> states = machine.initial_states;
			machine.run(states, current_word.begin(), current_word.end());
			
			// At least one state is reached
			if(states.size() > 0) {	

				// Check whether the output stored in the prefix tree is also
				// produced by the machine
				bool output_found = false;
				for(std::set<int>::const_iterator it1=states.begin(); it1!=states.end(); it1++) {
				
					// Get output
					typename std::map<int, answer>::const_iterator it2 = machine.output_mapping.find(*it1);
					if(it2 != machine.output_mapping.end() && it2->second == t.output[current_node]) {
						
						output_found = true;
						break;
						
					}
				
				}

				if(!output_found) {
					return false;
				}
			
			}
			// No state is reached
			else {
			
				if(t.output[current_node] != default_output) {
					return false;
				}
			
			}
		
		}	

		// Process children
		for(unsigned int a=0; a<t.get_alphabet_size(); a++) {

			if(t.edges[current_node][a] != prefix_tree<answer>::no_edge) {

				std::list<int> next_word(current_word.begin(), current_word.end());
				next_word.push_back(a);
				stack.push_back(std::pair<unsigned int, std::list<int> >(t.edges[current_node][a], next_word));

			}

		}
	
	}
	
	return true;

}


/**
 * Checks whether a given Moore machine is consistent with a knowledgebase,
 * i.e., whether the Moore machine classifies the words stored in the
 * knowledgebase correctly.
 *
 * If the Moore machine is nondeterministic, the method checks whether the
 * output stored in the knowledgebase is produced by one of the runs of the
 * Moore machine on the given word.
 *
 * For nondeterministic machines it might happen that no state is reached after
 * reading a word from the sample. In this case, the function compares the
 * output of the sample on this particular word with the default output, which
 * can be specified as a (non-mandatory) parameter.
 *
 * Note that this function does not change the knowledgebase.
 *
 * This method is mainly intended for debuging.
 *
 * @param base The knowledgebase to compare the Moore machine with
 * @param machine The Moore machine to test
 * @param default_output The default output if no state is reached. Defaults to
 *                       the default constructor of the template class. (For
 *                       bools this is false.)
 *
 * @return Returns whether the given Moore machine agrees on all classified
 *         words represented by the given knowledgebase.
 */
template <class answer>
bool is_consistent(libalf::knowledgebase<answer> & base, const libalf::moore_machine<answer> & machine, answer default_output = answer()) {

	// Run through all nodes of the knowledgebase and check whether the
	// corresponding word is classified correctly.
	std::list<typename libalf::knowledgebase<answer>::node *> stack;
	stack.push_back(base.get_rootptr());
	while(!stack.empty()) {
	
		// Get next node
		typename libalf::knowledgebase<answer>::node * n = stack.back();
		stack.pop_back();
	
		// Check whether word is classified correctly by machine
		if(n->is_answered()) {
			
			std::list<int> word = n->get_word();
			std::set<int> states = machine.initial_states;
			machine.run(states, word.begin(), word.end());

			// At least one state is reached
			if(states.size() > 0) {

				bool output_found = false;
				for(std::set<int>::const_iterator it=states.begin(); it!=states.end(); it++) {

					typename std::map<int, answer>::const_iterator it2 = machine.output_mapping.find(*it);
					if(it2 != machine.output_mapping.end() && it2->second==n->get_answer()) {
						output_found = true;
						break;
					}
					
				}
				if(!output_found) {
					return false;
				}
				
			}
			// No state is reached
			else {
			
				if(n->get_answer() != default_output) {
					return false;
				}
			
			}			
			
		}
	
		// Add successors
		int min = n->max_child_count() > machine.input_alphabet_size ? machine.input_alphabet_size : n->max_child_count();
		for(int a=min-1; a>=0; a--) {
		
			typename libalf::knowledgebase<answer>::node * child = n->find_child(a);
			if(child != NULL) {
			
				// We do not need to perform any check since the knowledgebase is a tree!
				stack.push_back(child);
			
			}
		
		}
	
	}
	
	return true;
	
}

/**
 * Checks whether a given Moore machine is consistent with a sample,
 * i.e., whether the Moore machine classifies the words stored in the
 * sample correctly.
 *
 * If the Moore machine is nondeterministic, the method checks whether the
 * output stored in the sample is produced by one of the runs of the
 * Moore machine on the given word.
 *
 * For nondeterministic machines it might happen that no state is reached after
 * reading a word from the sample. In this case, the function compares the
 * output of the sample on this particular word with the default output, which
 * can be specified as a (non-mandatory) parameter.
 *
 * This method is mainly intended for debuging.
 *
 * @param sample The sample to compare the Moore machine with
 * @param machine The Moore machine to test
 * @param default_output The default output if no state is reached. Defaults to
 *                       the default constructor of the template class. (For
 *                       bools this is false.)
 *
 * @return Returns whether the given Moore machine agrees on all words
 *         of the given sample.
 */
template <class answer>
bool is_consistent(const std::map<std::list<int>, answer> & sample, const libalf::moore_machine<answer> & machine, answer default_output = answer()) {

	// Check all word in the sample
	for(typename std::map<std::list<int>, answer>::const_iterator it1=sample.begin(); it1!=sample.end(); it1++) {
	
		// Get states reached by the machine on the given word
		std::set<int> states = machine.initial_states;
		machine.run(states, it1->first.begin(), it1->first.end());	
	
		// At least one state is reached
		if(states.size() > 0) {
	
			// Check whether output of prefix tree is contained in the output of the reached states 
			bool found_output = false;
			for(std::set<int>::const_iterator it2=states.begin(); it2!=states.end(); it2++) {

				// Get and check output
				typename std::map<int, answer>::const_iterator it3 = machine.output_mapping.find(*it2);
				if(it3!=machine.output_mapping.end() && it3->second==it1->second) {
					found_output = true;
					break;
				}
				
			}
			if(!found_output) {
				return false;
			}

		}
		// No state is reached
		else {
		
			if(it1->second != default_output) {
				return false;
			}
		
		}
		
	}

	return true;

}

}; // End of libALF namespace

#endif
