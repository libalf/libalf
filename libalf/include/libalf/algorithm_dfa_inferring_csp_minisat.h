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
 *
 * This implementation supports Boolean values as the only <answer> type.
 */

#ifndef __ALGORITHM_DETERMINISTIC_INFERRING_CSP_MINISAT__
#define __ALGORITHM_DETERMINISTIC_INFERRING_CSP_MINISAT__

// Standard includes 
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <vector>

#include <math.h>
#include <assert.h>

// libALF includes
#include "libalf/knowledgebase.h"
#include "libalf/conjecture.h"
#include "libalf/prefix_tree.h"
#include "libalf/algorithm_automata_inferring.h"

// MiniSat includes
#include "libalf/minisat_Solver.h"

namespace libalf {

template <class answer>
class deterministic_inferring_csp_MiniSat : public automata_inferring<answer> {

	private:

	/**
	 * Indicates whether we use the unary encoding of the CSP (true) or the
	 * the binary encoding (false).
	 */
	bool unary_encoding;

	/**
	 * Indicates whether the computed model should be logged (to the algorithm loglevel)
	 */
	bool log_model;
	
	public:

	/**
	 * Creates a new learning algorithm.
	 */
	deterministic_inferring_csp_MiniSat(knowledgebase<answer> * base, logger * log, int alphabet_size, bool unary_encoding = true) {

		this->set_alphabet_size(alphabet_size);
		this->set_logger(log);
		this->set_knowledge_source(base);
		
		this->unary_encoding = unary_encoding;
		this->log_model = false;

	}

	/**
	 * Returns whether this algorithm uses the unary or the binary encoding
	 * of the CSP.
	 *
	 * @return Returns whether this algorithm uses the unary or the binary encoding
	 *         of the CSP.
	 */
	bool uses_unary_encoding() {
		return unary_encoding;
	}

	/**
	 * Sets whether this algorithm should use the unary or binary encoding
	 * of the CSP.
	 *
	 * @param unary_encoding Indicates whether to use unary or binary
	 *                       encoding
	 */
	void set_unary_encoding(bool unary_encoding) {
		this->unary_encoding = unary_encoding;
	}

	bool is_logging_model() {
		return log_model;
	}
	
	void set_log_model(bool log_model) {
		this->log_model = log_model;
	}
	
	void print(std::ostream & os) const {
	
		os << "(Minimal) DFA inferring algorithm via Biermann's CSP using the MiniSat SAT Solver. ";
		os << "Alphabet size is " << this->alphabet_size;
		os << ", using the " << (unary_encoding ? "unary" : "binary") << " encoding of the CSP.";
		
	}

	private:

	virtual conjecture * __infer(const prefix_tree<answer> & t, unsigned int n) const {
	
		// Check value for n
		if(n == 0) {
			(*this->my_logger)(LOGGER_ERROR, "The number 'n' of states has to be greater than 0.\n");
			return NULL;
		}

		// Decide which encoding to use
		if(unary_encoding) {
			return infer_unary_MiniSat(t, n);
		} else {
			return infer_binary_MiniSat(t, n);
		}

	}
	
	libalf::moore_machine<answer> * infer_unary_MiniSat(const prefix_tree<answer> & t, unsigned int n) const {

		/*========================================
		 *
		 * Check parameter
		 *
		 *========================================*/
		if(n==0 || this->alphabet_size==0) {
			(*this->my_logger)(LOGGER_ERROR, "Alphabet size or size of automaton is zero.\n");
			return NULL;
		}
		assert(n > 0);
		assert(this->alphabet_size > 0);
		(*this->my_logger)(LOGGER_ALGORITHM, "Running MiniSat using the 'unary encoding' of the CSP to find a ");
		if(typeid(answer) == typeid(bool)) {
			(*this->my_logger)(LOGGER_ALGORITHM, "DFA");
		} else {
			(*this->my_logger)(LOGGER_ALGORITHM, "deterministic Moore machine");
		}
		(*this->my_logger)(LOGGER_ALGORITHM, " with %u states and alphabet size %d.\n", n, this->alphabet_size);
		
		
		/*========================================
		 *
		 * Create solver
		 *
		 *========================================*/
		MiniSat::Solver solver;
		
		
		/*========================================
		 *
		 * Create variables
		 *
		 *========================================*/
		unsigned long var_count = 0;
		std::map<int, std::map<int, MiniSat::Var> > x; // x_{u,q}

		// Labeling of the  prefixes
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int q=0; q<n; q++) {
				x[u][q] = solver.newVar();
				var_count++;
			}
		}
		
		
		/*========================================
		 *
		 * Create clauses
		 *
		 *========================================*/
		unsigned long long clause_count = 0;
		
		// (1) For each prefix, at least one state is assumed
		for(unsigned int u=0; u<t.node_count; u++) {
			
			MiniSat::vec<MiniSat::Lit> clause;
			clause.growTo(n);

			for(unsigned int q=0; q<n; q++) {
				clause[q] = MiniSat::Lit(x[u][q]);
			}
			
			solver.addClause(clause);
			clause_count++;

		}
		
		// (2) For each prefix, at most one state is assumed
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int p=0; p<n; p++) {
				for(unsigned int q=0; q<p; q++) {

					solver.addBinary(~MiniSat::Lit(x[u][p]), ~MiniSat::Lit(x[u][q]));
					clause_count++;

				}
			}
		}

		// (3) Transition function is applied correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int v=0; v<u; v++) {
				for(int a=0; a<this->alphabet_size; a++) {
					if(t.edges[u][a] != prefix_tree<answer>::no_edge && t.edges[v][a] != prefix_tree<answer>::no_edge) {

						for(unsigned int p=0; p<n; p++) {
							for(unsigned int q=0; q<n; q++) {
					
								{

									MiniSat::vec<MiniSat::Lit> clause;
									clause.growTo(4);

									clause[0] = ~MiniSat::Lit(x[u][q]);
									clause[1] = ~MiniSat::Lit(x[v][q]);
									clause[2] = MiniSat::Lit(x[t.edges[u][a]][p]);
									clause[3] = ~MiniSat::Lit(x[t.edges[v][a]][p]);
										
									solver.addClause(clause);
									clause_count++;

								}
						
								{

									MiniSat::vec<MiniSat::Lit> clause;
									clause.growTo(4);

									clause[0] = ~MiniSat::Lit(x[u][q]);
									clause[1] = ~MiniSat::Lit(x[v][q]);
									clause[2] = ~MiniSat::Lit(x[t.edges[u][a]][p]);
									clause[3] = MiniSat::Lit(x[t.edges[v][a]][p]);
										
									solver.addClause(clause);
									clause_count++;

								}
						
							}
						}
					}
				}
			}
		}
		
		// (4) Words with different classification cannot lead to the same state
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int v=0; v<u; v++) {
				if(t.specified[u] && t.specified[v] && (t.output[u] != t.output[v])) {

					for(unsigned int q=0; q<n; q++) {
						
						solver.addBinary(~MiniSat::Lit(x[u][q]), ~MiniSat::Lit(x[v][q]));
						clause_count++;
						
					}
				
				}
			}
		}
		
		
		/*========================================
		 *
		 * Solve
		 *
		 *========================================*/
		(*this->my_logger)(LOGGER_ALGORITHM, "Created %u variables and %u clauses.\n", var_count, clause_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ... ");
		if(!solver.solve()) {
			(*this->my_logger)(LOGGER_ALGORITHM, "Formula is unsatisfiable.\n");
			return NULL;
		} else {
			(*this->my_logger)(LOGGER_ALGORITHM, "Formula is satisfiable.\n");
		}

		
		/*========================================
		 *
		 * Compute result
		 *
		 *========================================*/
		// Write model
		if(log_model) {

			(*this->my_logger)(LOGGER_ALGORITHM, "Model:\n");
			for(unsigned int u=0; u<t.node_count; u++) {
				for(unsigned int q=0; q<n; q++) {
					(*this->my_logger)(LOGGER_ALGORITHM, "x[%u][%u] = %s\n", u, q, (solver.model[x[u][q]] == MiniSat::l_True ? "1" : (solver.model[x[u][q]] == MiniSat::l_False ? "0" : "?")));
				}
			}
			
		}
		
		// Transitions
		std::map<int, std::map<int, std::set<int> > > transitions;
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<answer>::no_edge) {
				
					// Search for source state
					int source = -1;
					for(unsigned int q=0; q<n; q++) {

						assert(solver.model[x[u][q]] != MiniSat::l_Undef);

						if(solver.model[x[u][q]] == MiniSat::l_True) {

							assert(source == -1);
							source = q;

						}

					}
					assert(source >= 0 && source <= (int)n);

					// Search for destination state
					int dest = -1;
					for(unsigned int q=0; q<n; q++) {
					
						assert(solver.model[x[t.edges[u][a]][q]] != MiniSat::l_Undef);
					
						if(solver.model[x[t.edges[u][a]][q]] == MiniSat::l_True) {

							assert(dest == -1);
							dest = q;

						}

					}
					assert(dest >= 0 && dest <= (int)n);
					
					// Check if a transition is defined and if so whether it is the same
					if(transitions.count(source) > 0) {
					
						assert(transitions[source].count(a) <= 1);
					
						if(transitions[source].count(a) > 0) {
							assert(*(transitions[source][a].begin()) == dest);
						} else {
							transitions[source][a].insert(dest);
						}
					
					} else {
						transitions[source][a].insert(dest);
					}
				
				}
			}
		}
		
		// Initial state
		int tmp_initial = -1;
		for(unsigned int q=0; q<n; q++) {
			
			assert(solver.model[x[0][q]] != MiniSat::l_Undef);
			
			if(solver.model[x[0][q]] == MiniSat::l_True) {
				
				assert(tmp_initial == -1);
				tmp_initial = q;

				
			}

		}
		assert(tmp_initial >= 0 && tmp_initial < (int)n);
		std::set<int> initial;
		initial.insert(tmp_initial);
		
		// Output
		std::map<int, answer> output_mapping;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {
			
				int state = -1;
				for(unsigned int q=0; q<n; q++) {
				
					assert(solver.model[x[u][q]] != MiniSat::l_Undef);
				
					if(solver.model[x[u][q]] == MiniSat::l_True) {
						
						assert(state == -1);
						state = q;

					}

				}
		
				assert(state >= 0 && state < (int)n);
				if(output_mapping.count(state) > 0) {
					assert(output_mapping[state] == t.output[u]);
				} else {
					output_mapping[state] = t.output[u];
				}

			}
		}
		//Add missing outputs
		for(unsigned int q=0; q<n; q++) {
			if(output_mapping.count(q) == 0) {
				output_mapping[q] = false;
			}
		}
		
		// Construct and return automaton
		moore_machine<answer> * automaton;
		if(typeid(answer) == typeid(bool)) {
			automaton = dynamic_cast<moore_machine<answer> * >(new finite_automaton);
		} else {
			automaton = new moore_machine<answer>;
		}
		automaton->input_alphabet_size = this->alphabet_size;
		automaton->state_count = n;
		automaton->initial_states = initial;
		automaton->output_mapping = output_mapping;
		automaton->transitions = transitions;
		automaton->valid = true;
		automaton->calc_determinism();

		assert(automaton->calc_validity());
		return automaton;
		
	}
	
	libalf::moore_machine<answer> * infer_binary_MiniSat(const prefix_tree<answer> & t, unsigned int n) const {

		/*========================================
		 *
		 * Check parameter
		 *
		 *========================================*/
		if(n==0 || this->alphabet_size==0) {
			(*this->my_logger)(LOGGER_ERROR, "Alphabet size or size of automaton is zero.\n");
			return NULL;
		}
		assert(n > 0);
		assert(this->alphabet_size > 0);
		(*this->my_logger)(LOGGER_ALGORITHM, "Running MiniSat using the 'binary encoding' of the CSP to find a ");
		if(typeid(answer) == typeid(bool)) {
			(*this->my_logger)(LOGGER_ALGORITHM, "DFA");
		} else {
			(*this->my_logger)(LOGGER_ALGORITHM, "deterministic Moore machine");
		}
		(*this->my_logger)(LOGGER_ALGORITHM, " with %u states and alphabet size %d.\n", n, this->alphabet_size);


		/*========================================
		 *
		 * Compute log(n)
		 *
		 *========================================*/
		unsigned int log_n = (unsigned int)(log(n) / log(2));
		if(pow(2, log_n) < n) {
			log_n++;
		}
		assert(pow(2, log_n) >= n);


		/*========================================
		 *
		 * Create solver
		 *
		 *========================================*/
		MiniSat::Solver solver;
	
	
		/*========================================
		 *
		 * Create variables
		 *
		 *========================================*/
		unsigned long var_count = 0;
		std::map<int, std::map<int, MiniSat::Var> > x; // x_{u,m} in binary encoding, least bit left
		
		// Labeling of the prefixes
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int m=0; m<log_n; m++) {
				x[u][m] = solver.newVar();
				var_count++;
			}
		}
	
	
		/*========================================
		 *
		 * Create clauses
		 *
		 *========================================*/
		unsigned long long clause_count = 0;
		
		// (1) For each prefix, a valid state (i.e. less than n) is assumed
		// We do this by not allowing binary values greater of equal to n.
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int q=n; q<pow(2, log_n); q++) {	

				// Get binary representation of q with log_n bits
				std::vector<bool> binary = to_binary(q, log_n);

				// Create clause that does not allow a value of q
				MiniSat::vec<MiniSat::Lit> clause;
				clause.growTo(log_n);
				for(unsigned int m=0; m<binary.size(); m++) {

					clause[m] = binary[m] ? ~MiniSat::Lit(x[u][m]) : MiniSat::Lit(x[u][m]);


				}

				solver.addClause(clause);
				clause_count++;

			}
		}

		// (2) Differently classified words lead to different states
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int v=0; v<u; v++) {
				if(t.specified[u] && t.specified[v] && (t.output[u] != t.output[v])) {

					for(unsigned int q=0; q<n; q++) {

						// Get binary representation of state
						std::vector<bool> binary = to_binary(q, log_n);

						// Create clause
						MiniSat::vec<MiniSat::Lit> clause;
						clause.growTo(2 * log_n);
						for(unsigned int m=0; m<binary.size(); m++) {

							clause[2*m] = binary[m] ? ~MiniSat::Lit(x[u][m]) : MiniSat::Lit(x[u][m]);
							clause[2*m+1] = binary[m] ? ~MiniSat::Lit(x[v][m]) : MiniSat::Lit(x[v][m]);

						}

						// Add clause
						solver.addClause(clause);
						clause_count++;

					}
				}
			}
		}

		// (3) A deterministic transition function is applied
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int v=0; v<u; v++) {
				for(int a=0; a<this->alphabet_size; a++) {
					if(t.edges[u][a] != prefix_tree<answer>::no_edge && t.edges[v][a] != prefix_tree<answer>::no_edge) {

						for(unsigned int q=0; q<n; q++) {

							// Get binary representation of state
							std::vector<bool> binary = to_binary(q, log_n);

							for(unsigned int m1=0; m1<log_n; m1++) {

								// Create clause 1
								MiniSat::vec<MiniSat::Lit> clause1;
								clause1.growTo(2 * log_n + 2);
								for(unsigned int m=0; m<binary.size(); m++) {
			
									clause1[2*m] = binary[m] ? ~MiniSat::Lit(x[u][m]) : MiniSat::Lit(x[u][m]);
									clause1[2*m+1] = binary[m] ? ~MiniSat::Lit(x[v][m]) : MiniSat::Lit(x[v][m]);

								}
								clause1[2*log_n] = MiniSat::Lit(x[t.edges[u][a]][m1]);
								clause1[2*log_n+1] = ~MiniSat::Lit(x[t.edges[v][a]][m1]);

								// Add clause 1
								solver.addClause(clause1);
								clause_count++;


								// Create clause 2
								MiniSat::vec<MiniSat::Lit> clause2;
								clause2.growTo(2 * log_n + 2);
								for(unsigned int m=0; m<binary.size(); m++) {
			
									clause2[2*m] = binary[m] ? ~MiniSat::Lit(x[u][m]) : MiniSat::Lit(x[u][m]);
									clause2[2*m+1] = binary[m] ? ~MiniSat::Lit(x[v][m]) : MiniSat::Lit(x[v][m]);

								}
								clause2[2*log_n] = ~MiniSat::Lit(x[t.edges[u][a]][m1]);
								clause2[2*log_n+1] = MiniSat::Lit(x[t.edges[v][a]][m1]);

								// Add clause 2
								solver.addClause(clause2);
								clause_count++;

							}
						}

					}
				}
			}
		}
	
	
		/*========================================
		 *
		 * Solve
		 *
		 *========================================*/
		(*this->my_logger)(LOGGER_ALGORITHM, "Created %u variables and %u clauses.\n", var_count, clause_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ... ");
		if(!solver.solve()) {
			(*this->my_logger)(LOGGER_ALGORITHM, "Formula is unsatisfiable.\n");
			return NULL;
		} else {
			(*this->my_logger)(LOGGER_ALGORITHM, "Formula is satisfiable.\n");
		}

		
		/*========================================
		 *
		 * Compute result
		 *
		 *========================================*/
		// Write model
		if(log_model) {

			(*this->my_logger)(LOGGER_ALGORITHM, "Model:\n");
			for(unsigned int u=0; u<t.node_count; u++) {
				for(unsigned int m=0; m<log_n; m++) {
					(*this->my_logger)(LOGGER_ALGORITHM, "x[%u][%u] = %s\n", u, m, (solver.model[x[u][m]] == MiniSat::l_True ? "1" : (solver.model[x[u][m]] == MiniSat::l_False ? "0" : "?")));
				}
			}
			
		}
		
		// Transitions
		std::map<int, std::map<int, std::set<int> > > transitions;
		// Process each prefix and look for applied transition
		for(unsigned int u=0; u<t.node_count; u++) {

			// Decode source state
			unsigned int source = 0;
			for(unsigned int m=0; m<log_n; m++) {
				
				assert(solver.model[x[u][m]] != MiniSat::l_Undef);
				
				if(solver.model[x[u][m]] == MiniSat::l_True) {
					source += pow(2, m);
				} 
				
			}

			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<answer>::no_edge) {

					// Decode destination state
					unsigned int dest = 0;
					for(unsigned int m=0; m<log_n; m++) {
					
						assert(solver.model[x[u][m]] != MiniSat::l_Undef);
					
						if(solver.model[x[t.edges[u][a]][m]] == MiniSat::l_True) {
							dest += pow(2, m);
						}

					}

					// Add transition
					assert(transitions[source][a].size() <= 1);
					assert(transitions[source][a].size() == 0 || *(transitions[source][a].begin()) == (int)dest);
					transitions[source][a].insert(dest);

				}
			}
		}
		 
		// Initial states
		unsigned int tmp_initial = 0;
		for(unsigned int m=0; m<log_n; m++) {
			
			assert(solver.model[x[0][m]] != MiniSat::l_Undef);
			
			if(solver.model[x[0][m]] == MiniSat::l_True) {
				tmp_initial += pow(2, m);
			}
			
		}
		std::set<int> initial;
		initial.insert(tmp_initial);
	
		// Output
		std::map<int, answer> output_mapping;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {

				// Decode state
				unsigned int state = 0;
				for(unsigned int m=0; m<log_n; m++) {
					
					assert(solver.model[x[u][m]] != MiniSat::l_Undef);
					
					if(solver.model[x[u][m]] == MiniSat::l_True) {
						state += pow(2, m);
					}
					
				}

				assert(state < (int)n);
				if(output_mapping.count(state) > 0) {
					assert(output_mapping[state] == t.output[u]);
				} else {
					output_mapping[state] = t.output[u];
				}
				
			}
		}

		// Construct and return automaton
		moore_machine<answer> * automaton;
		if(typeid(answer) == typeid(bool)) {
			automaton = dynamic_cast<moore_machine<answer> * >(new finite_automaton);
		} else {
			automaton = new moore_machine<answer>;
		}
		automaton->input_alphabet_size = this->alphabet_size;
		automaton->state_count = n;
		automaton->initial_states = initial;
		automaton->output_mapping = output_mapping;
		automaton->transitions = transitions;
		automaton->valid = true;
		automaton->calc_determinism();

		assert(automaton->calc_validity());
		return automaton;

	}

	/**
	 * Given an natural number, computes the binary representation (least bit left)
	 *
	 * @param number The number to convert into binary representation
	 *
	 * @return Returns a bit representation of the given number.
	 */
	std::vector<bool> to_binary(unsigned int number) const {

		std::vector<bool> binary;

		while (number) {
			binary.push_back((number & 1));
			number >>= 1;
		}

		return binary;
	}

	/**
	 * Given an natural number, computes the binary representation (least bit left)
	 * using at most n bits. Make sure that <tt>2^n >= number</tt>! This is not
	 * checked automatically!
	 *
	 * @param number The number to convert into binary representation
	 * @param n The number of bits to represent the number
	 *
	 * @return Returns a bit representation with <tt>n</tt> bits of the given number.
	 */

	std::vector<bool> to_binary(unsigned int number, unsigned int n) const {
	
		std::vector<bool> binary = to_binary(number);
		assert(binary.size() <= n);

		for(unsigned int i = n - binary.size(); i > 0; i--) {
			binary.push_back(0);
		}

		return binary;
	}

};

}; // End libalf namespace

#endif
