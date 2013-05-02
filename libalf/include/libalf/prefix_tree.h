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

#ifndef __prefix_tree_h__
#define __prefix_tree_h__

// Standard includes
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <assert.h>

// libALF includes
#include "libalf/knowledgebase.h"

namespace libalf {

/**
 * We want to have a prefix tree that indexes the sample.
 *
 * The alphabet size is fixed and cannot be changed afterwards.
 * This implementation does not check whether you access its data in a valid
 * and consistent way. You have to check it for yourself; it is meant only
 * to be used internally by someone who knows what he is doing.
 *
 * The root node always has the ID 0.
 */
template <class answer>
class prefix_tree {

	private:
	
	/**
	 * The alphabet size used by this tree.
	 */
	unsigned int alphabet_size;

	public:

	/**
	 * Defines a constant for "no edge".
	 */
	static const unsigned int no_edge;
	
	/**
	 * Defines a constant ID for the root node of the tree.
	 */
	static const unsigned int root;

	/**
	 * The number of nodes in the tree.
	 */
	unsigned int node_count;
	
	/**
	 * The edges of the tree. This a mapping from the ID of a node to the IDs of
	 * its successor nodes.
	 */
	std::vector<unsigned int *> edges;
	
	/**
	 * Indicated whether a word in the tree is specified.
	 */
	std::vector<bool> specified;
	
	/**
	 * Stores the output of a node (a word).
	 */
	std::vector<answer> output;

	/**
	 * Creates a new prefix tree from the given samples.
	 *
	 * @param sample The sample to build the prefix tree from
	 * @param alphabet_size The size of the alphabet to use
	 */
	prefix_tree(const std::map<std::list<int>, answer> & sample, unsigned int alphabet_size) {
		
		// Assign alphabet size
		assert(alphabet_size>0);
		this->alphabet_size = alphabet_size;
		
		// Initialize empty tree
		init();
		
		// Add samples
		for(typename std::map<std::list<int>, answer>::const_iterator it=sample.begin(); it!=sample.end(); it++) {
			add(it->first, it->second);
		}
		
	}
	
	/**
	 * Creates a new prefix tree from the given samples.
	 *
	 * @param sample The sample to build the prefix tree from. The knowledgebase
	 *               is not altered.
	 * @param alphabet_size The size of the alphabet to use
	 */
	prefix_tree(libalf::knowledgebase<answer> & base, unsigned int alphabet_size) {
		
		// Assign alphabet size
		assert(alphabet_size>0);
		this->alphabet_size = alphabet_size;
		
		// Initialize empty tree
		init();
		
		// Copy samples from knowledgebase
		add_knowledge(base.get_rootptr(), prefix_tree::root);
		
	}
	
	/**
	 * Destructor.
	 */
	~prefix_tree() {
		
		for(std::vector<unsigned int *>::iterator it=edges.begin(); it!=edges.end(); it++) {
			delete[] *it;
		}
		
	}
	
	/**
	 * Returns the alphabet size of this prefix tree.
	 *
	 * @return Returns the alphabet size of this prefix tree.
	 */
	unsigned int get_alphabet_size() const {
		return alphabet_size;
	}

	/**
	 * Returns a Graphviz Dot representation of this prefix tree.
	 *
	 * @return Returns a Graphviz Dot representation of this prefix tree.
	 */
	std::string to_dot() const {
		
		std::stringstream out;
		
		// Header
		out << "digraph prefix_tree {" << std::endl;
		
		// States
		for(unsigned int i=0; i<node_count; i++) {
			out << "\t" << i << " [color=\"";
			
			if(specified[i]) {
			
				// If bools are stored, return grren and red colored nodes
				if(typeid(answer) == typeid(bool)) {
					if(output[i]) {
						out << "green";
					} else {
						out << "red";
					}
				
				}
				
				// Otherwise, return the output
				else {
				
					out << "black\", label=\"" << output[i] << "\"";
				
				}
			
			} else {
				out << "black";
			}
			
			out << "\"];" << std::endl;
		}
		
		// Transitions
		for(unsigned int i=0; i<node_count; i++) {
			for(unsigned int j=0; j<alphabet_size; j++) {
			
				if(edges[i][j] != prefix_tree::no_edge) {
					out << "\t" << i << " -> " << edges[i][j] << " [label=\"" << j << "\"];" << std::endl;
				}
				
			}
		}
		
		// Footer
		out << "}";
		
		return out.str();
		
	}
	
	private:
	
	/**
	 * Creates an empty tree with only the root node.
	 */
	void init() {
	
		// Create root node with ID 0
		unsigned int * root_edges = new unsigned int[alphabet_size];
		for(unsigned int i=0; i<alphabet_size; i++) {
			root_edges[i] = prefix_tree::no_edge;
		}
		edges.push_back(root_edges);
		specified.push_back(false);
		output.push_back(answer());
		node_count = 1;
		
	}
	
	/**
	 * Adds a new word to the prefix tree. If the word is already existing, its
	 * classification is overridden.
	 *
	 * @param word The word to add
	 * @param classification The classification of the word to add
	 */
	void add(const std::list<int> & word, bool answer) {
		
		unsigned int current_state = 0;
		
		for(std::list<int>::const_iterator it=word.begin(); it!=word.end(); it++) {
			assert(*it>=0 && (unsigned int)*it<alphabet_size);
			
			unsigned int child = edges[current_state][*it];
			
			// Create new child
			if(child == prefix_tree::no_edge) {
				
				unsigned int * new_edges = new unsigned int[alphabet_size];
				for(unsigned int i=0; i<alphabet_size; i++) {
					new_edges[i] = prefix_tree::no_edge;
				}
				edges[current_state][*it] = node_count;
				edges.push_back(new_edges);
				specified.push_back(false);
				output.push_back(answer());
				child = node_count;
				node_count++;
				
			}

			current_state = child;
			
		}
		
		// Set classification
		specified[current_state] = true;
		output[current_state] = classification;
	}
	
	/**
	 * Recursively traverses the knowledgebase and adds the knowledge to the
	 * prefix tree.
	 *
	 * @param current The current node in the knowledgebase
	 */
	void add_knowledge(typename libalf::knowledgebase<answer>::node * kn_node, unsigned int pt_node) {
	
		// Label current node
		if(kn_node->is_answered()) {
		
			specified[pt_node] = true;
			output[pt_node] = kn_node->get_answer();
		
		}
	
		// Process children
		assert(kn_node->max_child_count() >= 0);
		unsigned int min = alphabet_size <= (unsigned int)kn_node->max_child_count() ? alphabet_size : kn_node->max_child_count();
		for(unsigned int a=0; a<min; a++) {
		
			typename libalf::knowledgebase<answer>::node * kn_child = kn_node->find_child(a);
			
			// Child exists
			if(kn_child != NULL) {
			
				// Check whether new node in prefix tree needs to be created
				unsigned int pt_child = edges[pt_node][a];
			
				// Create new child if necessary
				if(pt_child == prefix_tree<answer>::no_edge) {
					
					unsigned int * new_edges = new unsigned int[alphabet_size];
					for(unsigned int i=0; i<alphabet_size; i++) {
						new_edges[i] = prefix_tree<answer>::no_edge;
					}
					edges[pt_node][a] = node_count;
					edges.push_back(new_edges);
					specified.push_back(false);
					output.push_back(answer());
					pt_child = node_count;
					node_count++;
					
				}

				// Recursive call
				add_knowledge(kn_child, pt_child);
			
			}
		
		}
	
	}
	
};

}; // End of libalf namespace

/**
 * Defines prefix_tree::no_edge constant.
*/
template <class answer> const unsigned int libalf::prefix_tree<answer>::no_edge = 0;

/**
 * Defines prefix_tree::root constant.
 */
template <class answer> const unsigned int libalf::prefix_tree<answer>::root = 0;

/**
 * << operator to write a textual representation of a prefix tree to an output
 * stream.
 */
template <class answer>
std::ostream & operator<<(std::ostream & out, const libalf::prefix_tree<answer> & t) {
	out << t.to_dot();
	return out;
}

#endif
