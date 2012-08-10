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
 * This implementation allows several ways to model the same encoding in Z3. The
 * following two flags (given to the constructor or changed afterwards using the
 * corresponding getter and setters) control which variant is used. Thereby, all
 * combinations are possible.
 *
 * - use_variables: controls whether the states the inferred automaton assumes
 *                  after reading a word are modeled as an undefined function
 *                  (false) or as variables (true).
 *
 * - use_enum: controls whether the state of the automaton and the alphabet are
 *             modeled as integers (false) or a an enum (true), i.e., a finite
 *             domain.
 *
 * You might want to try several combinations and check which works best.
 *
 * This implementation supports Boolean values as the only <answer> type.
 */

#ifndef __ALGORITHM_DFA_INFERRING_CSP_Z3__
#define __ALGORITHM_DFA_INFERRING_CSP_Z3__

// Standard includes 
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <assert.h>

// libALF includes
#include "libalf/knowledgebase.h"
#include "libalf/conjecture.h"
#include "libalf/prefix_tree.h"
#include "libalf/algorithm_automata_inferring.h"

// Z3 includes
#include "z3.h"
#include "z3++.h"

namespace libalf {

class dfa_inferring_csp_Z3 : public automata_inferring<bool> {

	private:
	
	/**
	 * Indicates whether we use variables of an undefined function to model the states reached on reading words.
	 */
	bool use_variables;

	/**
	 * Indicates whether we use integers or enums to model the states and the alphabet.
	 */
	bool use_enum;
	
	/**
	 * Indicates whether the computed model should be logged (to the algorithm loglevel)
	 */
	bool log_model;
	
	public:

	/**
	 * Creates a new learning algorithm.
	 */
	dfa_inferring_csp_Z3(knowledgebase<bool> * base, logger * log, int alphabet_size, bool use_variables = false, bool use_enum = false) {

		this->set_alphabet_size(alphabet_size);
		this->set_logger(log);
		this->set_knowledge_source(base);
		
		this->use_variables = use_variables;
		this->use_enum = use_enum;
		
		this->log_model = false;
		
	}

	bool is_using_variables() {
		return use_variables;
	}
	
	void set_using_variables(bool use_variables) {
		this->use_variables = use_variables;
	}
	
	bool is_using_enum() {
		return use_enum;
	}
	
	void set_using_enum(bool use_enum) {
		this->use_enum = use_enum;
	}
	
	bool is_logging_model() {
		return log_model;
	}
	
	void set_log_model(bool log_model) {
		this->log_model = log_model;
	}
	
	void print(std::ostream & os) const {
	
		os << "(Minimal) DFA inferring algorithm via Biermann's CSP using Microsoft's Z3 SMT Solver. ";
		os << "Alphabet size is " << this->alphabet_size << ", using " << (use_variables ? "variables" : "undefined functions") << " to encode the states reached after reading samples, ";
		os << "using " << (use_enum ? "enums" : "integers") << " to model states.";
		
	}

	private:

	virtual conjecture * __infer(const prefix_tree<bool> & t, unsigned int n) const {
	
		// Check value for n
		if(n == 0) {
			(*this->my_logger)(LOGGER_ERROR, "The number 'n' of states has to be greater than 0.\n");
			return NULL;
		}

		// Decide which encoding of the formula we want to use
		if(!use_variables && !use_enum) {
			return infer_CSP_Z3(t, n);
		} else if(use_variables && !use_enum) {
			return infer_CSP_variables_Z3(t, n);
		} else if(!use_variables && use_enum) {
			return infer_CSP_enum_Z3(t, n);
		} else {
			return infer_CSP_variables_enum_Z3(t, n);
		}

	}
	
	libalf::finite_automaton * infer_CSP_Z3(const prefix_tree<bool> & t, unsigned int n) const {
	
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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running Z3 to find a solution for the CSP with n=%d and alphabet size=%d.\n", n, this->alphabet_size);
		
	
		/*========================================
		 *
		 * Create solver
		 *
		 *========================================*/
		z3::context c;
		z3::solver s(c);
		
		
		/*========================================
		 *
		 * Create function
		 *
		 *========================================*/
		z3::func_decl x = function("x", c.int_sort(), c.int_sort());
		
		
		/*========================================
		 *
		 * Create assertions
		 *
		 *========================================*/
		unsigned long long assertion_count = 0;
			
		// (1) Words are labeled correctly with states
		for(unsigned int u=0; u<t.node_count; u++) {

			s.add(x(u) >= 0);
			s.add(x(u) < c.int_val(n));

			assertion_count++;

		}
		
		// (2) Transition function is applied correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int v=0; v<t.node_count; v++) {
				for(int a=0; a<this->alphabet_size; a++) {
				
					if(t.edges[u][a] != prefix_tree<bool>::no_edge && t.edges[v][a] != prefix_tree<bool>::no_edge) {
				
						z3::expr assertion = implies(x(u) == x(v), x(t.edges[u][a]) == x(t.edges[v][a]));
						s.add(assertion);

						assertion_count++;

					}
					
				}
			}
		}
		
		// (3) Words are classified correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {
				for(unsigned int v=0; v<t.node_count; v++) {
					if(t.specified[v]) {
					
						if(t.output[u] != t.output[v]) {
						
							s.add(x(u) != x(v));
							assertion_count++;
						
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
		(*this->my_logger)(LOGGER_DEBUG, "Created %d assertions.\n", assertion_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ...\n");
		if(s.check() != z3::sat) {
			(*this->my_logger)(LOGGER_DEBUG, "Formula is unsatisfiable.\n");
			return NULL;
		}

		
		/*========================================
		 *
		 * Compute result
		 *
		 *========================================*/
		z3::model m = s.get_model();
		if(log_model) {
			std::stringstream out;
			out << m;
			(*this->my_logger)(LOGGER_ALGORITHM, "Model:\n%s\n", out.str().c_str());
		}
		
		// Transitions
		std::map<int, std::map<int, std::set<int> > > transitions;
		for(unsigned int u=0; u<t.node_count; u++) {

			// Get model value for source
			unsigned int source;
			Z3_bool conversion_ok = Z3_get_numeral_uint(c, m.eval(x(u)), &source);
			assert(conversion_ok == true);

			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<bool>::no_edge) {
				
					// Get model value for destination
					unsigned int dest;
					Z3_bool conversion_ok = Z3_get_numeral_uint(c, m.eval(x(t.edges[u][a])), &dest);
					assert(conversion_ok == true);
				
					// Check for nondeterminism
					assert(transitions[source][a].size() <= 1);
					assert(transitions[source][a].size() == 0 || *(transitions[source][a].begin()) == (int)dest);
				
					// Add transition
					transitions[source][a].insert(dest);
				
				}
			}

		}
		
		// Initial state
		unsigned int tmp_initial;
		Z3_bool conversion_ok = Z3_get_numeral_uint(c, m.eval(x(0)), &tmp_initial);
		assert(conversion_ok == true);
		std::set<int> initial;
		initial.insert(tmp_initial);
		
		// Final states
		std::set<int> final;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {

				// Get model value for state
				unsigned int state;
				conversion_ok = Z3_get_numeral_uint(c, m.eval(x(u)), &state);
				assert(conversion_ok == true);
				
				// Add
				if(t.output[u] == true) {
					final.insert(state);
				} else {
					assert(find(final.begin(), final.end(), state) == final.end());
				}
			
			}
		}
		
		// Construct and return automaton
		finite_automaton * dfa = new finite_automaton;
		dfa->input_alphabet_size = this->alphabet_size;
		dfa->state_count = n;
		dfa->initial_states = initial;
		dfa->set_final_states(final);
		dfa->transitions = transitions;
		dfa->valid = true;
		dfa->calc_determinism();

		assert(dfa->calc_validity());
		return dfa;
		
	}
	
	libalf::finite_automaton * infer_CSP_variables_Z3(const prefix_tree<bool> & t, unsigned int n) const {
		
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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running Z3 using variables to find a solution for the CSP with n=%d and alphabet size=%d.\n", n, this->alphabet_size);
		
		
		/*========================================
		 *
		 * Create solver
		 *
		 *========================================*/
		z3::context c;
		z3::solver s(c);
		
		
		/*========================================
		 *
		 * Create variables
		 *
		 *========================================*/
		std::vector<z3::expr> state_vars;
		for(unsigned int u=0; u<t.node_count; u++) {
			
			std::stringstream s;
			s << "x" << u;
			state_vars.push_back(c.int_const(s.str().c_str()));
			
		}
		
		
		/*========================================
		 *
		 * Create assertions
		 *
		 *========================================*/
		unsigned long long assertion_count = 0;
			
		// (1) Words are labeled correctly with states
		for(unsigned int u=0; u<t.node_count; u++) {

			s.add(state_vars[u] >= 0);
			s.add(state_vars[u] < c.int_val(n));

			assertion_count++;

		}
		
		// Initial state?
		
		// (2) Transition function is applied correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int v=0; v<t.node_count; v++) {
				for(int a=0; a<this->alphabet_size; a++) {
				
					if(t.edges[u][a] != prefix_tree<bool>::no_edge && t.edges[v][a] != prefix_tree<bool>::no_edge) {
				
						z3::expr assertion = implies(state_vars[u] == state_vars[v], state_vars[t.edges[u][a]] == state_vars[t.edges[v][a]]);
						s.add(assertion);

						assertion_count++;

					}
					
				}
			}
		}
		
		// (3) Words are classified correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {
				for(unsigned int v=0; v<t.node_count; v++) {
					if(t.specified[v]) {
					
						if(t.output[u] != t.output[v]) {
						
							s.add(state_vars[u] != state_vars[v]);
							assertion_count++;
						
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
		(*this->my_logger)(LOGGER_DEBUG, "Created %d assertions.\n", assertion_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ...\n");
		if(s.check() != z3::sat) {
			(*this->my_logger)(LOGGER_DEBUG, "Formula is unsatisfiable.\n");
			return NULL;
		}

		
		/*========================================
		 *
		 * Compute result
		 *
		 *========================================*/
		z3::model m = s.get_model();
		if(log_model) {
			std::stringstream out;
			out << m;
			(*this->my_logger)(LOGGER_ALGORITHM, "Model:\n%s\n", out.str().c_str());
		}
		
		// Transitions
		std::map<int, std::map<int, std::set<int> > > transitions;
		for(unsigned int u=0; u<t.node_count; u++) {

			// Get model value for source
			unsigned int source;
			Z3_bool conversion_ok = Z3_get_numeral_uint(c, m.eval(state_vars[u]), &source);
			assert(conversion_ok == true);

			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<bool>::no_edge) {
				
					// Get model value for destination
					unsigned int dest;
					Z3_bool conversion_ok = Z3_get_numeral_uint(c, m.eval(state_vars[t.edges[u][a]]), &dest);
					assert(conversion_ok == true);
				
					// Check for nondeterminism
					assert(transitions[source][a].size() <= 1);
					assert(transitions[source][a].size() == 0 || *(transitions[source][a].begin()) == (int)dest);
				
					// Add transition
					transitions[source][a].insert(dest);
				
				}
			}

		}
		
		// Initial state
		unsigned int tmp_initial;
		Z3_bool conversion_ok = Z3_get_numeral_uint(c, m.eval(state_vars[0]), &tmp_initial);
		assert(conversion_ok == true && tmp_initial < n);
		std::set<int> initial;
		initial.insert(tmp_initial);
		
		// Final states
		std::set<int> final;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {

				// Get model value for state
				unsigned int state;
				conversion_ok = Z3_get_numeral_uint(c, m.eval(state_vars[u]), &state);
				assert(conversion_ok == true);
				
				// Add
				if(t.output[u] == true) {
					final.insert(state);
				} else {
					assert(find(final.begin(), final.end(), state) == final.end());
				}
			
			}
		}
		
		// Construct and return automaton
		finite_automaton * dfa = new finite_automaton;
		dfa->input_alphabet_size = this->alphabet_size;
		dfa->state_count = n;
		dfa->initial_states = initial;
		dfa->set_final_states(final);
		dfa->transitions = transitions;
		dfa->valid = true;
		dfa->calc_determinism();

		assert(dfa->calc_validity());
		return dfa;
		
	}
	
	libalf::finite_automaton * infer_CSP_enum_Z3(const prefix_tree<bool> & t, unsigned int n) const {

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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running Z3 using enums to find a solution for the CSP with n=%d and alphabet size=%d.\n", n, this->alphabet_size);


		/*========================================
		 *
		 * Create solver
		 *
		 *========================================*/
		z3::context c;
		z3::solver s(c);
	
	
		/*========================================
		 *
		 * Create enum and functions
		 *
		 *========================================*/
		// Create state enum
		Z3_symbol s_name = Z3_mk_string_symbol(c, "state");
		Z3_symbol s_names[n];
		Z3_func_decl s_consts[n];
		Z3_func_decl s_testers[n];
		for(unsigned int i=0; i<n; i++) {
			std::stringstream tmp;
			tmp << "q" << i;
			s_names[i] = Z3_mk_string_symbol(c, tmp.str().c_str());
		}
		z3::sort state_sort(c, Z3_mk_enumeration_sort(c, s_name, n, s_names, s_consts, s_testers));
		std::vector<z3::func_decl> state_consts;
		std::vector<z3::func_decl> state_testers;
		for(unsigned int i=0; i<n; i++) {
			state_consts.push_back(z3::func_decl(c, s_consts[i]));
			state_testers.push_back(z3::func_decl(c, s_testers[i]));
		}

		// Functions
		z3::func_decl x = function("x", c.int_sort(), state_sort);
	
		/*========================================
		 *
		 * Create assertions
		 *
		 *========================================*/
		unsigned long long assertion_count = 0;
	
		// (1) The initial state is 0
		s.add(state_testers[0](x(prefix_tree<bool>::root)));
		assertion_count++;
	
		// (2) Transition function is applied correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int v=0; v<t.node_count; v++) {
				for(int a=0; a<this->alphabet_size; a++) {
			
					if(t.edges[u][a] != prefix_tree<bool>::no_edge && t.edges[v][a] != prefix_tree<bool>::no_edge) {
			
						z3::expr assertion = implies(x(u) == x(v), x(t.edges[u][a]) == x(t.edges[v][a]));
						s.add(assertion);

						assertion_count++;

					}
				
				}
			}
		}
	
		// (3) Words are classified correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {
				for(unsigned int v=0; v<t.node_count; v++) {
					if(t.specified[v]) {
				
						if(t.output[u] != t.output[v]) {
					
							s.add(x(u) != x(v));
					
							assertion_count++;
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
		(*this->my_logger)(LOGGER_DEBUG, "Created %d assertions.\n", assertion_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ...\n");
		if(s.check() != z3::sat) {
			(*this->my_logger)(LOGGER_DEBUG, "Formula is unsatisfiable.\n");
			return NULL;
		}

		
		/*========================================
		 *
		 * Compute result
		 *
		 *========================================*/
		z3::model m = s.get_model();
		if(log_model) {
			std::stringstream out;
			out << m;
			(*this->my_logger)(LOGGER_ALGORITHM, "Model:\n%s\n", out.str().c_str());
		}

		// Transitions
		std::map<int, std::map<int, std::set<int> > > transitions;
		for(unsigned int u=0; u<t.node_count; u++) {

			// Get model value for source
			int source = -1;
			for(unsigned int p=0; p<n; p++) {
		
				Z3_bool v = Z3_get_bool_value(c, m.eval(state_testers[p](x(u))));
				assert(v != Z3_L_UNDEF);
		
				if(v == Z3_L_TRUE) {
			
					if(source == -1) {
						source = p;
					} else {
						(*this->my_logger)(LOGGER_ERROR, "Detected error in model!\n");
						return NULL;
					}
			
				}
			
			}
			assert(source >=0 && source < (int)n);
		
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<bool>::no_edge) {
			
					// Get model value for destination
					int dest = -1;
					for(unsigned int p=0; p<n; p++) {
				
						Z3_bool v = Z3_get_bool_value(c, m.eval(state_testers[p](x(t.edges[u][a]))));
						assert(v != Z3_L_UNDEF);
				
						if(v == Z3_L_TRUE) {
					
							if(dest == -1) {
								dest = p;
							} else {
								(*this->my_logger)(LOGGER_ERROR, "Detected error in model!\n");
								return NULL;
							}
					
						} 

					}
					assert(dest >=0 && dest < (int)n);
			
					// Check for nondeterminism
					assert(transitions[source][a].size() <= 1);
					assert(transitions[source][a].size() == 0 || *(transitions[source][a].begin()) == dest);
			
					// Add transition
					transitions[source][a].insert(dest);
			
				}
			}

		}
	
		// Initial state
		int tmp_initial = -1;
		{
			for(unsigned int p=0; p<n; p++) {
					
				Z3_bool v = Z3_get_bool_value(c, m.eval(state_testers[p](x(prefix_tree<bool>::root))));
				assert(v != Z3_L_UNDEF);
		
				if(v == Z3_L_TRUE) {
			
					if(tmp_initial == -1) {
						tmp_initial = p;
					} else {
						(*this->my_logger)(LOGGER_ERROR, "Detected error in model!\n");
						return NULL;
					}
			
				} 

			}
		}
		assert(tmp_initial >= 0 && tmp_initial < (int)n);
		std::set<int> initial;
		initial.insert(tmp_initial);

		// Final states
		std::set<int> final;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {

				// Get model value for state
				int state = -1;
				for(unsigned int p=0; p<n; p++) {
				
					// Identify which enum is assigned to this variable
					Z3_bool v = Z3_get_bool_value(c, m.eval(state_testers[p](x(u))));
					assert(v != Z3_L_UNDEF);
			
					if(v == Z3_L_TRUE) {
				
						if(state == -1) {
							state = p;
						} else {
							(*this->my_logger)(LOGGER_ERROR, "Detected error in final states!\n");
							return NULL;
						}
				
					}
			
				}
				assert(state >= 0 && state < (int)n);
			
				// Add
				if(t.output[u] == true) {
					final.insert(state);
				} else {
					assert(find(final.begin(), final.end(), state) == final.end());
				}
		
			}
		}

		// Construct and return automaton
		finite_automaton * dfa = new finite_automaton;
		dfa->input_alphabet_size = this->alphabet_size;
		dfa->state_count = n;
		dfa->initial_states = initial;
		dfa->set_final_states(final);
		dfa->transitions = transitions;
		dfa->valid = true;
		dfa->calc_determinism();

		assert(dfa->calc_validity());
		return dfa;

	}
	
	libalf::finite_automaton * infer_CSP_variables_enum_Z3(const prefix_tree<bool> & t, unsigned int n) const {
		
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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running Z3 using variables and enums to find a solution for the CSP with n=%d and alphabet size=%d.\n", n, this->alphabet_size);
		
		
		/*========================================
		 *
		 * Create solver
		 *
		 *========================================*/
		z3::context c;
		z3::solver s(c);
		
		
		/*========================================
		 *
		 * Create enum and variables
		 *
		 *========================================*/
		// Create state enum
		Z3_symbol s_name = Z3_mk_string_symbol(c, "state");
		Z3_symbol s_names[n];
		Z3_func_decl s_consts[n];
		Z3_func_decl s_testers[n];
		for(unsigned int i=0; i<n; i++) {
			std::stringstream tmp;
			tmp << "q" << i;
			s_names[i] = Z3_mk_string_symbol(c, tmp.str().c_str());
		}
		z3::sort state_sort(c, Z3_mk_enumeration_sort(c, s_name, n, s_names, s_consts, s_testers));
		std::vector<z3::func_decl> state_consts;
		std::vector<z3::func_decl> state_testers;
		for(unsigned int i=0; i<n; i++) {
			state_consts.push_back(z3::func_decl(c, s_consts[i]));
			state_testers.push_back(z3::func_decl(c, s_testers[i]));
		}

		// Variables
		std::vector<z3::expr> state_vars;
		for(unsigned int u=0; u<t.node_count; u++) {
			
			std::stringstream s;
			s << "x" << u;
			state_vars.push_back(c.constant(s.str().c_str(), state_sort));
			
		}
		
		
		/*========================================
		 *
		 * Create assertions
		 *
		 *========================================*/
		unsigned long long assertion_count = 0;
		
		// (1) The initial state is 0
		s.add(state_testers[0](state_vars[prefix_tree<bool>::root]));
		assertion_count++;
		
		// (2) Transition function is applied correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int v=0; v<t.node_count; v++) {
				for(int a=0; a<this->alphabet_size; a++) {
				
					if(t.edges[u][a] != prefix_tree<bool>::no_edge && t.edges[v][a] != prefix_tree<bool>::no_edge) {
				
						z3::expr assertion = implies(state_vars[u] == state_vars[v], state_vars[t.edges[u][a]] == state_vars[t.edges[v][a]]);
						s.add(assertion);

						assertion_count++;

					}
					
				}
			}
		}
		
		// (3) Words are classified correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {
				for(unsigned int v=0; v<t.node_count; v++) {
					if(t.specified[v]) {
					
						if(t.output[u] != t.output[v]) {
						
							s.add(state_vars[u] != state_vars[v]);
						
							assertion_count++;
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
		(*this->my_logger)(LOGGER_DEBUG, "Created %d assertions.\n", assertion_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ...\n");
		if(s.check() != z3::sat) {
			(*this->my_logger)(LOGGER_DEBUG, "Formula is unsatisfiable.\n");
			return NULL;
		}

		
		/*========================================
		 *
		 * Compute result
		 *
		 *========================================*/
		z3::model m = s.get_model();
		if(log_model) {
			std::stringstream out;
			out << m;
			(*this->my_logger)(LOGGER_ALGORITHM, "Model:\n%s\n", out.str().c_str());
		}
		
		// Transitions
		std::map<int, std::map<int, std::set<int> > > transitions;
		for(unsigned int u=0; u<t.node_count; u++) {

			// Get model value for source
			int source = -1;
			for(unsigned int p=0; p<n; p++) {
			
				Z3_bool v = Z3_get_bool_value(c, m.eval(state_testers[p](state_vars[u])));
				assert(v != Z3_L_UNDEF);
			
				if(v == Z3_L_TRUE) {
				
					if(source == -1) {
						source = p;
					} else {
						(*this->my_logger)(LOGGER_ERROR, "Detected error in model.\n");
						return NULL;
					}
				
				}
				
			}
			assert(source >=0 && source < (int)n);
			
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<bool>::no_edge) {
				
					// Get model value for destination
					int dest = -1;
					for(unsigned int p=0; p<n; p++) {
					
						Z3_bool v = Z3_get_bool_value(c, m.eval(state_testers[p](state_vars[t.edges[u][a]])));
						assert(v != Z3_L_UNDEF);
					
						if(v == Z3_L_TRUE) {
						
							if(dest == -1) {
								dest = p;
							} else {
								(*this->my_logger)(LOGGER_ERROR, "Detected error in model.\n");
								return NULL;
							}
						
						} 

					}
					assert(dest >=0 && dest < (int)n);
				
					// Check for nondeterminism
					assert(transitions[source][a].size() <= 1);
					assert(transitions[source][a].size() == 0 || *(transitions[source][a].begin()) == dest);
				
					// Add transition
					transitions[source][a].insert(dest);
				
				}
			}

		}
		
		// Initial state
		int tmp_initial = -1;
		{
			for(unsigned int p=0; p<n; p++) {
						
				Z3_bool v = Z3_get_bool_value(c, m.eval(state_testers[p](state_vars[prefix_tree<bool>::root])));
				assert(v != Z3_L_UNDEF);
			
				if(v == Z3_L_TRUE) {
				
					if(tmp_initial == -1) {
						tmp_initial = p;
					} else {
						(*this->my_logger)(LOGGER_ERROR, "Detected error in model.\n");
						return NULL;
					}
				
				} 

			}
		}
		assert(tmp_initial >= 0 && tmp_initial < (int)n);
		std::set<int> initial;
		initial.insert(tmp_initial);
		
		// Final states
		std::set<int> final;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {

				// Get model value for state
				int state = -1;
				for(unsigned int p=0; p<n; p++) {
					
					// Identify which enum is assigned to this variable
					Z3_bool v = Z3_get_bool_value(c, m.eval(state_testers[p](state_vars[u])));
					assert(v != Z3_L_UNDEF);
				
					if(v == Z3_L_TRUE) {
					
						if(state == -1) {
							state = p;
						} else {
							(*this->my_logger)(LOGGER_ERROR, "Detected error in model.\n");
							return NULL;
						}
					
					}
				
				}
				assert(state >= 0 && state < (int)n);
				
				// Add
				if(t.output[u] == true) {
					final.insert(state);
				} else {
					assert(find(final.begin(), final.end(), state) == final.end());
				}
			
			}
		}
		
		// Construct and return automaton
		finite_automaton * dfa = new finite_automaton;
		dfa->input_alphabet_size = this->alphabet_size;
		dfa->state_count = n;
		dfa->initial_states = initial;
		dfa->set_final_states(final);
		dfa->transitions = transitions;
		dfa->valid = true;
		dfa->calc_determinism();

		assert(dfa->calc_validity());
		return dfa;
		
	}
	
};

}; // End libalf namespace

#endif
