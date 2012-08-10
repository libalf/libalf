/* $Id$
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
 * (c) 2008-2011 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *               and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */



#ifndef __libalf_algorithm_DFAminiSAT_h__
# define __libalf_algorithm_DFAminiSAT_h__

#define NO_EDGE 0 //Set to 0 because the root is labeled 0 and has no incoming transition

#include <vector>
#include <list>
#include <map>

#include <string.h>

#include <libalf/minisat_Solver.h>

namespace libalf {


using namespace libalf::MiniSat;

/*
 * A straight-forward SAT encoding for DFA identification with fixed number of
 * states.
 * The encoding is described in
 *
 *   M. Heule and S. Verwer - Exact DFA Identification using SAT Solvers,
 *
 * but is really really straight-forward: label the nodes of the prefix tree
 * according to an accepting run of some automaton, whose transitions are to be
 * defined by the SAT solver.
 *
 * Variables:
 *===========
 * x_w,q   : node corresponding to word w is labeled with state q
 * d_p,a,q : transition \delta(p,a) = q
 * z_q     : state q is accepting
 *
 * Constraints:
 *=============
 * (x_w,q \vee x_w,q' \vee ...)                 - Each node of the tree is labeled
 * 
 * (d_p,a,q \vee \neg x_w,p \vee \neg x_wa,q)   - Transitions has been applied correctly
 *                                                i.e. if x_w is labaled by p 
 *                                                and x_wa is labeled by q, then
 *                                                transition d_p,a,q must be applied
 *                                                (i.e. the transition from state
 *                                                p with a leads to q)
 *
 * (\neg d_p,a,q \vee \neg d_p,a,q')            - Transitions are deterministic
 *
 * (\neg x_w,q \vee z_q)                        - All accepting words are accepted
 *                                                i.e. if w is to accepted, then
 *                                                state q must be accepting
 *
 * (\neg x_w,q \vee \neg z_q)                   - All rejecting words are rejected
 *
 * NOTE: this version does only support bool as <answer>.
 */

template <class answer>
class DFAminiSAT : public learning_algorithm<answer> {
	
	protected:
		// Internal data structures to copy the prefix tree to
		std::vector<unsigned int *> tree;
		std::vector<bool> accept;
		std::vector<bool> reject;
		
		// Shall we use redundant clauses (they may speed up solving the SAT instance)
		bool use_redundant;
	
	public: // types
		typedef typename knowledgebase<answer>::node node;
		
	public:
		DFAminiSAT(knowledgebase<answer> * base, logger * log, int alphabet_size, bool use_redundant = true) {
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(base);
			this->use_redundant = use_redundant;
			
			// If answer is something different than bool, give a warning.
			if(typeid(answer) != typeid(bool)) {
				(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: this algorithm does only work with bool-type. The behaviour now is undefined.).\n");
			}
		}
		
		virtual ~DFAminiSAT() {
			for(std::vector<unsigned int *>::iterator it=tree.begin(); it!=tree.end(); it++) {
				delete[] *it;
			}
			return;
		}

	/**
	 * Copies the data stored in the knowledgebase into an internal
	 * representation. This is useful, because
	 * - SAT solving takes long and we may want to alter the knowledgebase
	 *   during this time
	 * - we need to index the nodes in the nowledgebase anyways.
	 */
	void copy_knowledge() {
		
		// Clear internal data structures
		tree.clear();
		accept.clear();
		reject.clear();

		// Copy root
		unsigned int * root_trans = new unsigned int[this->alphabet_size];
		memset(root_trans, NO_EDGE, sizeof(unsigned int) * this->alphabet_size);
		tree.push_back(root_trans);
		if(this->my_knowledge->get_rootptr()->is_answered()) {
			if(this->my_knowledge->get_rootptr()->get_answer() == true) {
				accept.push_back(true);
				reject.push_back(false);
			} else {
				accept.push_back(false);
				reject.push_back(true);
			}
		} else {
			accept.push_back(false);
			reject.push_back(false);
		}
		
		// Copy the rest
		std::list<std::pair<node *, unsigned int> > stack;
		stack.push_back(std::pair<node *, unsigned int>(this->my_knowledge->get_rootptr(), 0));
		while(!stack.empty()) {
		
			// Get top of stack
			std::pair<node *, unsigned int> cur_pair = stack.back();
			node * cur = cur_pair.first;
			unsigned int id = cur_pair.second;
			stack.pop_back();
			
			// Copy children
			int min = this->alphabet_size;
			if(cur->max_child_count() < min) {
				min = cur->max_child_count();
			}
			for(int i=0; i<min; i++) {
				node * child = cur->find_child(i);
				
				if(child) {
					
					// Create new entry in internal data structure
					unsigned int * trans = new unsigned int[this->alphabet_size];
					memset(trans, NO_EDGE, sizeof(unsigned int) * this->alphabet_size);
					tree.push_back(trans);
					if(child->is_answered()) {
						if(child->get_answer() == true) {
							accept.push_back(true);
							reject.push_back(false);
						} else {
							accept.push_back(false);
							reject.push_back(true);
						}
					} else {
						accept.push_back(false);
						reject.push_back(false);
					}
					
					// Add transition to current node
					tree[id][i] = tree.size() - 1;
					
					// Add child to stack
					stack.push_back(std::pair<node *, unsigned int>(child, tree.size()-1));
				}
			}
		}
		
		#ifdef DEBUG
		// Display internal data structure
		if(tree.size() != accept.size() || tree.size() != reject.size()) {
			std::cerr << "Unequal sizes of internal data structures!" << std::endl;
		}
		for(unsigned int i=0; i<tree.size(); i++) {
			std::cout << i << ":\t" << "a=" << accept[i] << ", r=" << reject[i] << ", [";
			for(int j=0; j<this->alphabet_size; j++) {
				std::cout << tree[i][j];
				if(j<this->alphabet_size-1) {
					std::cout << " ";
				}
			}
			std::cout << "]" << std::endl;
		}
		#endif
	}

	void copy_knowledge_if_necessary() {
		if(this->tree.size()==0) {
			copy_knowledge();
		}
	}

	/**
	 * Tries to find an automaton with state_count states that is consistent
	 * with the data stored internally.
	 */
	virtual finite_automaton * compute_automaton(unsigned int state_count) = 0;

	conjecture * derive_conjecture() {

		// Upper and lower bounds for binary search
		unsigned int lower = 1;
		unsigned int upper = 1;
		
		// Search for an automaton that is consistent with the data
		finite_automaton * dfa = NULL;
		do {
		
			dfa = compute_automaton(upper);
			if(!dfa) {
				lower = upper;
				upper *= 2;
			}
		
		} while(!dfa);
		(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: found automaton with %d states consistent with the data.\n", upper);
		
		
		// 1 state DFA found
		if(upper == lower) {
			return dfa;
		}
		
		// Try binary search to find minimal automaton
		while(upper > lower+1) {

			unsigned int mid = lower + ((upper - lower) / 2);
			finite_automaton * dfa_tmp = compute_automaton(mid);

			//std::cout << "lower=" << lower << ", upper =" << upper << ", mid=" << mid << std::endl;
			
			if(dfa_tmp) {
				delete dfa;
				dfa = dfa_tmp;
				upper = mid;
			} else {
				lower = mid;
			}
		}
		(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: minimal automaton consistent with the data has %d states.\n", upper);
		
		return dfa;
	}
	
	/**
	 * Checks whether the given DFA is consistent with the knowledgebase
	 * associated to this learning algorithm. Note that, if the knowledgebase
	 * was altered, then this method may return false. Moreover, if the
	 * knowledgebase contains words that have symbols greater that the alphabet
	 * size of this algorithm, they are ignored in this check.
	 *
	 * @param dfa The DFA to check for consistency.
	 *
	 * @returns Return true, if the DFa is consistent with the data in the 
	 *          knowledgebase.
	 */
	bool check_consistency_with_knowledgebase(finite_automaton * dfa) {
	
		for(typename knowledgebase<answer>::iterator it=this->my_knowledge->begin(); it!=this->my_knowledge->end(); it++) {
			if((*it).is_answered()) {
				
				// We want to skip words that contain symbols greater than our alphabet size
				if((*it).get_label() >= this->alphabet_size) {
					continue;
				}
				
				// check the word
				std::list<int> word = (*it).get_word();
				if((*it).get_answer() == true && !dfa->contains(word)) {
					(*this->my_logger)(LOGGER_ERROR, "DFAminiSAT: computed automaton is not consistent with the data!\n");
					return false;
				} else if((*it).get_answer() == false && dfa->contains(word)) {
					(*this->my_logger)(LOGGER_ERROR, "DFAminiSAT: computed automaton is not consistent with the data!\n");
					return false;
				}
			}
		}
	
		return true;
	}
	
	enum learning_algorithm_type get_type() const {
		return ALG_NONE;
	};

	enum learning_algorithm_type get_basic_compatible_type() const {
		return ALG_NONE;
	};
	

	void increase_alphabet_size(int new_asize) {
		// Clear internal data structure
		for(std::vector<unsigned int *>::iterator it=tree.begin(); it!=tree.end(); it++) {
			delete[] *it;
		}
		tree.clear();
		accept.clear();
		reject.clear();
		
		this->set_alphabet_size(new_asize);
	}

	memory_statistics get_memory_statistics() const {
		memory_statistics ret;
		return ret;
	}

	void receive_generic_statistics(generic_statistics & stat) const {
	}

	virtual bool sync_to_knowledgebase() {
		return false;
	}

	virtual bool supports_sync() const {
		return false;
	}
	
	// stubs for counterexample will throw a warning to the logger
	virtual bool add_counterexample(std::list<int>) {
		(*this->my_logger)(LOGGER_ERROR, "DFA MiniSAT does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		return false;
	}
	
	void print(std::ostream &os) const {
		os << "To be implemented!" << std::endl;
	}
	
	std::basic_string<int32_t> serialize() const {
		std::basic_string<int32_t> ret;

		// we don't have any internal, persistent data
		ret += ::serialize(1);
		ret += ::serialize(ALG_NONE);

		return ret;
	}
	
	bool deserialize(serial_stretch & serial) {
		// TODO: To be done
		return false;
	}
	
	bool complete() {
		// we're offline.
		return true;
	}

	virtual bool conjecture_ready() {
		if((this->my_knowledge != NULL) && (this->my_knowledge->count_answers() > 0)) {
			if(this->get_alphabet_size() != this->my_knowledge->get_largest_symbol())
				(*this->my_logger)(LOGGER_WARN, "DFAMiniSAT: differing alphabet size between this (%d) and knowledgebase (%d)!\n",
						this->get_alphabet_size(), this->my_knowledge->get_largest_symbol());

			return true;
		} else {
			return false;
		};
	}
};

template <class answer>
class DFAminiSAT_transition_unary : public DFAminiSAT<answer> {

	public:
	DFAminiSAT_transition_unary(knowledgebase<answer> * base, logger * log, int alphabet_size, bool use_redundant = true) : DFAminiSAT<answer>(base, log, alphabet_size, use_redundant) {
	}

	bool create_sat(Solver & solver, unsigned int state_count, std::vector<std::vector<Var> > & x, std::vector<std::vector<std::vector<Var> > > & d, std::vector<Var> & z) {
	
		// Clear variable vectors
		x.clear();
		d.clear();
		z.clear();
	
		// Create internal data structure if called for the first time
		this->copy_knowledge_if_necessary();
		
		/*
		 * Create arrays for variables
		 */
		// x
		for(unsigned int i=0; i<this->tree.size(); i++) {
			x.push_back(std::vector<Var>());
				
			for(unsigned int j=0; j<state_count; j++) {
				x[i].push_back(solver.newVar());
			}
		}
		#ifdef DEBUG
		for(unsigned int i=0; i<this->tree.size(); i++) {
			for(unsigned int j=0; j<state_count; j++) {
				std::cout << "x[" << i << "][" << j << "]=" << x[i][j] << "   ";
			}
			std::cout << std::endl;
		}
		#endif
		
		
		// d
		for(unsigned int i=0; i<state_count; i++) {
			d.push_back(std::vector<std::vector<Var> >());
		
			for(int j=0; j<this->alphabet_size; j++) {
				d[i].push_back(std::vector<Var>());
				
				for(unsigned int k=0; k<state_count; k++) {
					d[i][j].push_back(solver.newVar());
				}
			}
		}
		
		// z
		for(unsigned int i=0; i<state_count; i++) {
			z.push_back(solver.newVar());
		}
		
		/*
		 * Create clauses
		 */
		 
		// Each node in the prefix tree is labeled with at least one state
		for(unsigned int w=0; w<this->tree.size(); w++) {
			vec<Lit> clause;
			
			clause.growTo(state_count);
			for(unsigned int p=0; p<state_count; p++) {
				clause[p] = Lit(x[w][p]);
			}
			solver.addClause(clause);
			
			#ifdef DEBUG
			std::cout << "Adding every node is labeled ";
			for(unsigned int p=0; p<state_count; p++) {
				std::cout << " x[" << w << "][" << p << "] ";
			}
			std::cout << std::endl;
			#endif
			
			if(!solver.okay()) {
				(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: created conflicting clause (each node need to be labeled).\n");
				return false;
			}
		}
		
		// Correct accepting
		for(unsigned int i=0; i<this->tree.size(); i++) {
			if(this->accept[i]) {
			
				for(unsigned int j=0; j<state_count; j++) {
					
					#ifdef DEBUG
					std:: cout << "Adding accepting ~x[" << i << "][" << j << "], z[" << j << "]" << std::endl;
					#endif
					solver.addBinary(~Lit(x[i][j]), Lit(z[j]));
					if(!solver.okay()) {
						(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: created conflicting clause (correct accepting).\n");
						return false;
					}
				}
			
			} else if(this->reject[i]) {
			
				for(unsigned int j=0; j<state_count; j++) {
					
					#ifdef DEBUG
					std:: cout << "Adding rejecting ~x[" << i << "][" << j << "], ~z[" << j << "]" << std::endl;
					#endif
					solver.addBinary(~Lit(x[i][j]), ~Lit(z[j]));
					if(!solver.okay()) {
						(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: created conflicting clause (correct rejecting).\n");
						return false;
					}
				}
			}
		}
		
		// Correct transition labeling
		for(unsigned int w=0; w<this->tree.size(); w++) {
			for(int a=0; a<this->alphabet_size; a++) {
				
				if(this->tree[w][a] != NO_EDGE) {
			
					for(unsigned int p=0; p<state_count; p++) {
						for(unsigned int q=0; q<state_count; q++) {
			
							#ifdef DEBUG
							std::cout << "Adding transition d[" << p << "][" << a << "][" << q << "] ~x[" << w << "][" << p << "] ~x[" << tree[w][a] << "][" << q << "]" << std::endl;
							#endif
							solver.addTernary(Lit(d[p][a][q]), ~Lit(x[w][p]), ~Lit(x[this->tree[w][a]][q]));
							if(!solver.okay()) {
								(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: created conflicting clause (transition applied correctly).\n");
								return false;
							}
						}
					}
				}
			}
		}
		
		// No nondeterminism
		for(unsigned int p=0; p<state_count; p++) {
			for(int a=0; a<this->alphabet_size; a++) {
				for(unsigned int q1=0; q1<state_count; q1++) {
					for(unsigned int q2=q1+1; q2<state_count; q2++) {

						#ifdef DEBUG
						std::cout << "Adding nondeterminism " << "~d[" << p << "][" << a << "][" << q1 << "] ~d[" << p << "][" << a << "][" << q2 << "]" << std::endl;
						#endif
						solver.addBinary(~Lit(d[p][a][q1]), ~Lit(d[p][a][q2]));
						if(!solver.okay()) {
							(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: created conflicting clause (no nondeterminism).\n");
							return false;
						}
					}
				}
			}
		}
		
		/*
		 * Redundant clauses.
		 * They are not needed to find a solution, but add further knowledge
		 * the SAT solver may find useful. This can speed up the solving.
		 */
		if(!this->use_redundant) {
			return true;
		}
		
		// Each node is labeled with at most one state
		for(unsigned int w=0; w<this->tree.size(); w++) {
			for(unsigned int p=0; p<state_count; p++) {
				for(unsigned int q=p+1; q<state_count; q++) {

					solver.addBinary(~Lit(x[w][p]), ~Lit(x[w][q]));
					if(!solver.okay()) {
						(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: created conflicting clause (at most one state per node).\n");
						return false;
					}
				}
			}
		}
		
		// Each transition must be defined
		for(unsigned int p=0; p<state_count; p++) {
			for(int a=0; a<this->alphabet_size; a++) {
				
				vec<Lit> clause;
				clause.growTo(state_count);
				
				for(unsigned int q=0; q<state_count; q++) {
					clause[q] = Lit(d[p][a][q]);
				}
				
				solver.addClause(clause);
				if(!solver.okay()) {
					(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: created conflicting clause (each transition must be defined).\n");
					return false;
				}
			}
		}
		
		// Transitions are applied correctly
		for(unsigned int w=0; w<this->tree.size(); w++) {
			for(int a=0; a<this->alphabet_size; a++) {
				
				if(this->tree[w][a] != NO_EDGE) {
			
					for(unsigned int p=0; p<state_count; p++) {
						for(unsigned int q=0; q<state_count; q++) {
						
							solver.addTernary(~Lit(d[p][a][q]), ~Lit(x[w][p]), Lit(x[this->tree[w][a]][q]));
							if(!solver.okay()) {
								(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: created conflicting clause (transitions are applied correctly).\n");
								return false;
							}
						}
					}
				}
			}
		}
		
		return true;
	}
	
	finite_automaton * compute_automaton(unsigned int state_count) {
	
		// Check parameter
		if(state_count == 0) {
			return NULL;
		}
	
		// Prepare solver and variables
		Solver s;
		std::vector<std::vector<Var> > x;
		std::vector<std::vector<std::vector<Var> > > d;
		std::vector<Var> z;
		
		/*
		 * Create the SAT instance.
		 * This may result in conflicting clauses. If so, then there is no
		 * automaton with that many states consistent with the data.
		 */
		if(create_sat(s, state_count, x, d, z)) {

			// Simplify the problem
			s.simplifyDB();
		
			// Force 0 as initial state.
			#if 0
			vec<Lit> assumptions;
			Lit force_initial(x[0][0], true);
			assumptions.push(force_initial);
			#endif

			// Solve it!
			if(!s.solve()) {
				(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: There exists no automaton with %d states consistent with the data.\n", state_count);
				return NULL;
			}

			// Create automaton
			return sat2automaton(s, state_count, x, d, z);
		
		} else {
			(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: There exists no automaton with %d states consistent with the data.\n", state_count);
			return NULL;
		}
	}

	finite_automaton * sat2automaton(Solver & s, unsigned int state_count, std::vector<std::vector<Var> > & x, std::vector<std::vector<std::vector<Var> > > & d, std::vector<Var> & z) {
	
		// Check SAT solver
		if(!s.model) {
			return NULL;
		}
	
		// Initial state
		int initial = -1;
		for(unsigned int i=0; i<state_count; i++) {
			if(s.model[x[0][i]] == l_True) {
				if(initial == -1) {
					initial = i;
				} else {
					(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT: discovered more than one initial state.\n");
					return NULL;
				}
			}
		}
	
		// Final states
		std::set<int> final;
		for(unsigned int i=0; i<state_count; i++) {
			if(s.model[z[i]] == l_True) {
				final.insert(i);
			}
		}
	
		// Transitions
		std::map<int, std::map<int, std::set<int> > > trans;
		for(unsigned int p=0; p<state_count; p++) {
			for(int a=0; a<this->alphabet_size; a++) {
				
				bool defined = false;
				
				for(unsigned int q=0; q<state_count; q++) {
				
					if(s.model[d[p][a][q]] == l_True) {
					
						if(defined) {
							(*this->my_logger)(LOGGER_ERROR, "DFAminiSAT: SAT solver provided nondeterministic automaton.\n");
							return NULL;
						}
						
						defined = true;
					
						trans[p][a].insert(q);
					}
				}
				
				#ifdef DEBUG
				if(!defined) {
					std::cerr << "Transition " << p << ", " << a << " undefined!" << std::endl;
				}
				#endif
			}
		}
	
		// Create automaton
		finite_automaton * result = new finite_automaton();
		result->input_alphabet_size = this->alphabet_size;
		result->state_count = state_count;
		result->initial_states.insert(initial);
		result->set_final_states(final);
		result->transitions = trans;
		
		// Final stuff
		result->valid = true;
		result->calc_determinism();
	
		return result;
	}
	
};

/*******************************************************************************
 *
 * Bierman unary encoding
 *
 ******************************************************************************/

template <class answer>
class DFAminiSAT_bierman_unary : public DFAminiSAT<answer> {

	public:
	DFAminiSAT_bierman_unary(knowledgebase<answer> * base, logger * log, int alphabet_size, bool use_redundant = true) : DFAminiSAT<answer>(base, log, alphabet_size, use_redundant) {
	}

	bool create_sat(Solver & solver, unsigned int state_count, std::vector<std::vector<Var> > & x) {
	
		// Clear variable vectors
		x.clear();
	
		// Create internal data structure if called for the first time
		this->copy_knowledge_if_necessary();
		
		/*
		 * Create arrays for variables
		 */
		// x
		for(unsigned int i=0; i<this->tree.size(); i++) {
			x.push_back(std::vector<Var>());
				
			for(unsigned int j=0; j<state_count; j++) {
				x[i].push_back(solver.newVar());
			}
		}
		
		/*
		 * Create Clauses
		 */
		 
		// Labeling consistent
		for(unsigned int w1=0; w1<this->tree.size(); w1++) {
			for(unsigned int w2=0; w2<this->tree.size(); w2++) {
				for(int a=0; a<this->alphabet_size; a++) {
				
					if(this->tree[w1][a] != NO_EDGE && this->tree[w2][a] != NO_EDGE) {
					
						for(unsigned int p=0; p<state_count; p++) {
							for(unsigned int q=0; q<state_count; q++) {

								solver.addTernary(~Lit(x[w1][p]), ~Lit(x[w2][p]), Lit(x[this->tree[w1][a]][q]));
								if(!solver.okay()) {
									(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT Bierman unary: created conflicting clause (consistent labeling).\n");
									return false;
								}
								
								solver.addTernary(~Lit(x[w1][p]), ~Lit(x[w2][p]), Lit(x[this->tree[w2][a]][q]));
								if(!solver.okay()) {
									(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT Bierman unary: created conflicting clause (consistent labeling).\n");
									return false;
								}
							}
						}
					}
				}
			}
		}
		
		// All nodes are labeled
		for(unsigned int w=0; w<this->tree.size(); w++) {
			
			vec<Lit> clause;
			clause.growTo(state_count);
			
			for(unsigned int q=0; q<state_count; q++) {
				clause[q] = Lit(x[w][q]);
			}
			
			solver.addClause(clause);
			if(!solver.okay()) {
				(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT Bierman unary: created conflicting clause (all nodes are labeled).\n");
				return false;
			}
		}
		
		// Each node is labeled uniquely
		for(unsigned int w=0; w<this->tree.size(); w++) {
			for(unsigned int p=0; p<state_count; p++) {
				for(unsigned int q=p+1; q<state_count; q++) {
			
					solver.addBinary(~Lit(x[w][p]), ~Lit(x[w][q]));
					if(!solver.okay()) {
						(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT Bierman unary: created conflicting clause (each node is labeled uniquely).\n");
						return false;
					}
				}
			}
		}
			
		// Correct accepting
		for(unsigned int w1=0; w1<this->tree.size(); w1++) {
			for(unsigned int w2=w1+1; w2<this->tree.size(); w2++) {
		
				if((this->accept[w1] && this->reject[w2]) || (this->reject[w1] && this->accept[w2])) {

					for(unsigned int q=0; q<state_count; q++) {
						solver.addBinary(~Lit(x[w1][q]), ~Lit(x[w2][q]));
						if(!solver.okay()) {
							(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT Bierman unary: created conflicting clause (correct accepting).\n");
							return false;
						}
					}
				}
			}
		}
		
		
		
		/*
		 * Redundant clauses.
		 * They are not needed to find a solution, but add further knowledge
		 * the SAT solver may find useful. This can speed up the solving.
		 */
		if(!this->use_redundant) {
			return true;
		}
		
		return true;
	}
	
	finite_automaton * compute_automaton(unsigned int state_count) {
	
		// Check parameter
		if(state_count == 0) {
			return NULL;
		}
	
		// Prepare solver and variables
		Solver s;
		std::vector<std::vector<Var> > x;
		
		/*
		 * Create the SAT instance.
		 * This may result in conflicting clauses. If so, then there is no
		 * automaton with that many states consistent with the data.
		 */
		if(create_sat(s, state_count, x)) {

			// Simplify the problem
			s.simplifyDB();
		
			// Force 0 as initial state.

			// Solve it!
			if(!s.solve()) {
				(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT Bierman unary: There exists no automaton with %d states consistent with the data.\n", state_count);
				return NULL;
			}

			// Create automaton
			return sat2automaton(s, state_count, x);
		
		} else {
			(*this->my_logger)(LOGGER_ALGORITHM, "DFAminiSAT Bierman unary: There exists no automaton with %d states consistent with the data.\n", state_count);
			return NULL;
		}
	}

	finite_automaton * sat2automaton(Solver & s, unsigned int state_count, std::vector<std::vector<Var> > & x) {
	
		// Check SAT solver
		if(!s.model) {
			return NULL;
		}
	
		
		// Create automaton
		finite_automaton * result = new finite_automaton();
		#if 0
		result->input_alphabet_size = this->alphabet_size;
		result->state_count = state_count;
		result->initial_states.insert(initial);
		result->set_final_states(final);
		result->transitions = trans;
		
		// Final stuff
		result->valid = true;
		result->calc_determinism();
		#endif
	
		return result;
	}
	
};

}; // end namespace libalf

#endif // __libalf_algorithm_DFAminiSAT_h__

