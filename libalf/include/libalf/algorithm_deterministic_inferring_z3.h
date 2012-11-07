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

#ifndef __ALGORITHM_DETERMINISTIC_INFERRING_Z3_H__
#define __ALGORITHM_DETERMINISTIC_INFERRING_Z3_H__

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

template <class answer>
class deterministic_inferring_Z3 : public automata_inferring<answer> {

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
	deterministic_inferring_Z3(knowledgebase<answer> * base, logger * log, int alphabet_size, bool use_variables = false, bool use_enum = false) : automata_inferring<answer>() {

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
	
		os << "(Minimal) deterministic Moore machine inferring algorithm using Microsoft's Z3 SMT Solver. ";
		os << "Alphabet size is " << this->alphabet_size << ", using " << (use_variables ? "variables" : "undefined functions") << " to encode the states reached after reading samples, ";
		os << "using " << (use_enum ? "enums" : "integers") << " to model states, alphabet symbols, and output symbols.";
		
	}

	private:

	virtual conjecture * __infer(const prefix_tree<answer> & t, unsigned int n) const {
	
		// Check value for n
		if(n == 0) {
			(*this->my_logger)(LOGGER_ERROR, "The number 'n' of states has to be greater than 0.\n");
			return NULL;
		}

		// Decide which encoding of the formula we want to use
		if(!use_variables && !use_enum) {
			return infer_Z3(t, n);
		} else if(use_variables && !use_enum) {
			return infer_Z3_variables(t, n);
		} else if(!use_variables && use_enum) {
			return infer_Z3_enum(t, n);
		} else {
			return infer_Z3_variables_enum(t, n);
		}

	}
	
	virtual conjecture * infer_simple_conjecture(prefix_tree<answer> const & t) const {
	
		return infer_simple_moore_machine(t);
		
	}
	
	libalf::moore_machine<answer> * infer_Z3(const prefix_tree<answer> & t, unsigned int n) const {

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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running Z3 to find a solution with %u states and alphabet size %d.\n", n, this->alphabet_size);


		/*========================================
		 *
		 * Create mapping that maps all possible
		 * outputs to some internal ID
		 *
		 *========================================*/
		std::map<answer, unsigned int> output_id;
		unsigned int output_count = 0;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u] && output_id.count(t.output[u]) == 0) {
				output_id[t.output[u]] = output_count++;
			}
		}

		
		/*========================================
		 *
		 * Create solver
		 *
		 *========================================*/
		z3::context c;
		z3::solver s(c);
		
		
		/*========================================
		 *
		 * Create undefined functions
		 *
		 *========================================*/
		z3::func_decl d = function("d", c.int_sort(), c.int_sort(), c.int_sort());
		z3::func_decl f = function("f", c.int_sort(), c.int_sort());
		z3::func_decl x = function("x", c.int_sort(), c.int_sort());
		
		
		/*========================================
		 *
		 * Create assertions
		 *
		 *========================================*/
		unsigned long long assertion_count = 0;
			
		// (1) The initial state is 0
		s.add(x(0) == prefix_tree<answer>::root);
		assertion_count++;

		// (2) Transition function is valid, i.e.,  0 <= d(q, a) < n
		for(unsigned int q=0; q<n; q++) {
			for(int a=0; a<this->alphabet_size; a++) {

				z3::expr args[] = {c.int_val(q), c.int_val(a)};
				s.add(d(2, args) >= 0);
				s.add(d(2, args) < n);
		
				assertion_count = assertion_count + 2;
			}
		}
		
		// (3) Transitions are applied correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
			
				if(t.edges[u][a] != prefix_tree<answer>::no_edge) {
				
					z3::expr args[] = {x(u), c.int_val(a)};
					s.add(x(t.edges[u][a]) == d(2, args));
		
					assertion_count++;
					
				}
			
			}
		}
		
		// (4) Output is valid, i.e., 0 <= f(q) < output_count
		for(unsigned int q=0; q<n; q++) {

			s.add(f(q) >= 0);
			s.add(f(q) < output_count);

			assertion_count = assertion_count + 2;

		}

		// (5) Words are classified correctly
		for(unsigned int u=0; u<t.node_count; u++) {
				
				if(t.specified[u]) {
				
					s.add(f(x(u)) == c.int_val(output_id[t.output[u]]));

					assertion_count++;
					
				}

		}

		/*========================================
		 *
		 * Solve
		 *
		 *========================================*/
		(*this->my_logger)(LOGGER_ALGORITHM, "Created %u assertions.\n", assertion_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ...\n");
		if(s.check() != z3::sat) {
			(*this->my_logger)(LOGGER_ALGORITHM, "Formula is unsatisfiable.\n");
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
		for(unsigned int q=0; q<n; q++) {
			for(int a=0; a<this->alphabet_size; a++) {
				
				// Get info from model
				z3::expr args[] = {c.int_val(q), c.int_val(a)};
				z3::expr result = m.eval(d(2, args));
				unsigned int dest;
				Z3_bool conversion_ok = Z3_get_numeral_uint(c, result, &dest);
				assert(conversion_ok == true);
				
				// Add transition
				transitions[q][a].insert(dest);

			}
		}
		
		// Initial state
		std::set<int> initial;
		initial.insert(prefix_tree<answer>::root);
		
		// Output
		std::map<int, answer> output_mapping;
		for(unsigned int q=0; q<n; q++) {
			
			// Retrive value from model
			unsigned int output;
			Z3_bool conversion_ok = Z3_get_numeral_uint(c, m.eval(f(q)), &output);
			assert(conversion_ok == true);
			
			// Add output
			assert(output < output_count);
			output_mapping[q] = find_key(output_id, output);
			
		}
		assert(output_mapping.size() == n);
		
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
	
	libalf::moore_machine<answer> * infer_Z3_variables(const prefix_tree<answer> & t, unsigned int n) const {

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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running Z3 using undefined functions and variables to find a solution with %u states and alphabet size %d.\n", n, this->alphabet_size);
		

		/*========================================
		 *
		 * Create mapping that maps all possible
		 * outputs to some internal ID
		 *
		 *========================================*/
		std::map<answer, unsigned int> output_id;
		unsigned int output_count = 0;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u] && output_id.count(t.output[u]) == 0) {
				output_id[t.output[u]] = output_count++;
			}
		}

		
		/*========================================
		 *
		 * Create solver
		 *
		 *========================================*/
		z3::context c;
		z3::solver s(c);
		

		/*========================================
		 *
		 * Create undefined functions and variables
		 *
		 *========================================*/
		z3::func_decl d = function("d", c.int_sort(), c.int_sort(), c.int_sort());
		z3::func_decl f = function("f", c.int_sort(), c.int_sort());
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

		// (1) The initial state is 0
		s.add(state_vars[0] == prefix_tree<answer>::root);
		assertion_count++;
			
		// (2) Transition function is valid, i.e.,  0 <= d(q, a) < n
		for(unsigned int q=0; q<n; q++) {
			for(int a=0; a<this->alphabet_size; a++) {

				z3::expr args[] = {c.int_val(q), c.int_val(a)};
				s.add(d(2, args) >= 0);
				s.add(d(2, args) < n);
		
				assertion_count = assertion_count + 2;
			}
		}
		
		// (3) Transitions are applied correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
			
				if(t.edges[u][a] != prefix_tree<answer>::no_edge) {
				
					z3::expr args[] = {state_vars[u], c.int_val(a)};
					s.add(state_vars[t.edges[u][a]] == d(2, args));
		
					assertion_count++;
					
				}
			
			}
		}
		
		// (4) Output is valid, i.e.,  0 <= f(q) < output_count
		for(unsigned int q=0; q<n; q++) {
			for(int a=0; a<this->alphabet_size; a++) {

				s.add(f(q) >= 0);
				s.add(f(q) < output_count);
		
				assertion_count = assertion_count + 2;
			}
		}

		// (5) Words are classified correctly
		for(unsigned int u=0; u<t.node_count; u++) {
				
				if(t.specified[u]) {

					s.add(f(state_vars[u]) == c.int_val(output_id[t.output[u]]));
					
					assertion_count++;
					
				}

		}

		/*========================================
		 *
		 * Solve
		 *
		 *========================================*/
		(*this->my_logger)(LOGGER_ALGORITHM, "Created %u assertions.\n", assertion_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ...\n");
		if(s.check() != z3::sat) {
			(*this->my_logger)(LOGGER_ALGORITHM, "Formula is unsatisfiable.\n");
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
		for(unsigned int q=0; q<n; q++) {
			for(int a=0; a<this->alphabet_size; a++) {
				
				// Get info from model
				z3::expr args[] = {c.int_val(q), c.int_val(a)};
				z3::expr result = m.eval(d(2, args));
				unsigned int dest;
				Z3_bool conversion_ok = Z3_get_numeral_uint(c, result, &dest);
				assert(conversion_ok == true);
				
				// Add transition
				transitions[q][a].insert(dest);

			}
		}
		
		// Initial state
		std::set<int> initial;
		initial.insert(prefix_tree<answer>::root);
		
		// Output
		std::map<int, answer> output_mapping;
		for(unsigned int q=0; q<n; q++) {
			
			// Retrive value from model
			unsigned int output;
			Z3_bool conversion_ok = Z3_get_numeral_uint(c, m.eval(f(q)), &output);
			assert(conversion_ok != Z3_L_UNDEF);
			
			// Add output
			assert(output < output_count);
			output_mapping[q] = find_key(output_id, output);
			
		}
		assert(output_mapping.size() == n);
		
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
	
	libalf::moore_machine<answer> * infer_Z3_enum(const prefix_tree<answer> & t, unsigned int n) const {

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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running Z3 using undefined functions and enums to find a solution with %u states and alphabet size %d.\n", n, this->alphabet_size);


		/*========================================
		 *
		 * Create mapping that maps all possible
		 * outputs to some internal ID
		 *
		 *========================================*/
		std::map<answer, unsigned int> output_id;
		unsigned int output_count = 0;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u] && output_id.count(t.output[u]) == 0) {
				output_id[t.output[u]] = output_count++;
			}
		}


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
		
		// Create alphabet enum
		Z3_symbol a_name = Z3_mk_string_symbol(c, "alphabet");
		Z3_symbol a_names[this->alphabet_size];
		Z3_func_decl a_consts[this->alphabet_size];
		Z3_func_decl a_testers[this->alphabet_size];
		for(int i=0; i<this->alphabet_size; i++) {
		
			std::stringstream tmp;
			tmp << "a" << i;
			a_names[i] = Z3_mk_string_symbol(c, tmp.str().c_str());
		
		}
		z3::sort alphabet_sort(c, Z3_mk_enumeration_sort(c, a_name, this->alphabet_size, a_names, a_consts, a_testers));
		std::vector<z3::func_decl> alphabet_consts;
		std::vector<z3::func_decl> alphabet_testers;
		for(int i=0; i<this->alphabet_size; i++) {
			alphabet_consts.push_back(z3::func_decl(c, a_consts[i]));
			alphabet_testers.push_back(z3::func_decl(c, a_testers[i]));
		}
		
		// Create output enum
		Z3_symbol o_name = Z3_mk_string_symbol(c, "output");
		Z3_symbol o_names[output_count];
		Z3_func_decl o_consts[output_count];
		Z3_func_decl o_testers[output_count];
		for(unsigned int i=0; i<output_count; i++) {
		
			std::stringstream tmp;
			tmp << "out" << i;
			o_names[i] = Z3_mk_string_symbol(c, tmp.str().c_str());
		
		}
		z3::sort output_sort(c, Z3_mk_enumeration_sort(c, o_name, output_count, o_names, o_consts, o_testers));
		//std::vector<z3::func_decl> output_consts;
		std::vector<z3::func_decl> output_testers;
		for(unsigned int i=0; i<output_count; i++) {
			//output_consts.push_back(z3::func_decl(c, o_consts[i]));
			output_testers.push_back(z3::func_decl(c, o_testers[i]));
		}

		// Functions
		z3::func_decl d = function("d", state_sort, alphabet_sort, state_sort);
		z3::func_decl f = function("f", state_sort, output_sort);
		z3::func_decl x = function("x", c.int_sort(), state_sort);

		
		/*========================================
		 *
		 * Create assertions
		 *
		 *========================================*/
		unsigned long long assertion_count = 0;
			
		// (1) The initial state is 0
		s.add(state_testers[prefix_tree<answer>::root](x(0)));
		assertion_count++;
		
		// (2) Transitions are applied correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
			
				if(t.edges[u][a] != prefix_tree<answer>::no_edge) {
				
					z3::ast tmp (c, Z3_mk_app(c, a_consts[a], 0, NULL));
					z3::expr args[] = {x(u), to_expr(c, tmp)};
					s.add(x(t.edges[u][a]) == d(2, args));
		
					assertion_count++;
					
				}
			
			}
		}
		
		// (3) Words are classified correctly
		for(unsigned int u=0; u<t.node_count; u++) {
				
			if(t.specified[u]) {
			
				s.add(output_testers[output_id[t.output[u]]](f(x(u))));

				assertion_count++;
				
			}

		}
		
		
		/*========================================
		 *
		 * Solve
		 *
		 *========================================*/
		(*this->my_logger)(LOGGER_ALGORITHM, "Created %u assertions.\n", assertion_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ...\n");
		if(s.check() != z3::sat) {
			(*this->my_logger)(LOGGER_ALGORITHM, "Formula is unsatisfiable.\n");
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
		for(unsigned int q=0; q<n; q++) {
			for(int a=0; a<this->alphabet_size; a++) {
				
				// Check which of the finite many states is defined by the model
				bool dest_found = false;
				unsigned int dest = 0;
				for(unsigned int p=0; p<n; p++) {
				
					// Get info from model
					z3::ast tmp1 (c, Z3_mk_app(c, s_consts[q], 0, NULL));
					z3::ast tmp2 (c, Z3_mk_app(c, a_consts[a], 0, NULL));
					z3::expr args[] = {to_expr(c, tmp1), to_expr(c, tmp2)};
					z3::expr result = m.eval(state_testers[p](d(2, args)));

					Z3_bool conversion_ok = Z3_get_bool_value(c, result);
					assert(conversion_ok != Z3_L_UNDEF);

					// Check destination
					if(conversion_ok == Z3_L_TRUE) {
					
						assert(!dest_found);
						dest_found = true;
						dest = p;
					
					}
					
				}
				assert(dest_found && dest < n);
				
				// Add transition
				transitions[q][a].insert(dest);

			}
		}
		
		// Initial state
		std::set<int> initial;
		initial.insert(prefix_tree<answer>::root);
		
		// Output
		std::map<int, answer> output_mapping;
		for(unsigned int q=0; q<n; q++) {
			
			// Retrive value from model
			bool output_found = false;
			unsigned int output = 0;
			for(unsigned int i=0; i<output_count; i++) {

				z3::ast tmp (c, Z3_mk_app(c, s_consts[q], 0, NULL));
				Z3_lbool result = Z3_get_bool_value(c, m.eval(output_testers[i](f(to_expr(c, tmp)))));
				assert(result != Z3_L_UNDEF);
			
				// Add final state
				if(result == Z3_L_TRUE) {

					assert(!output_found);
					output_found = true;
					output = i;

				}

			}
			assert(output_found && output < output_count);

			output_mapping[q] = find_key(output_id, output);
			
		}
		assert(output_mapping.size() == n);
		
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
	
	libalf::moore_machine<answer> * infer_Z3_variables_enum(const prefix_tree<answer> & t, unsigned int n) const {

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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running Z3 using undefined functions, variables, and enums to find a solution with %u states and alphabet size %d.\n", n, this->alphabet_size);
	

		/*========================================
		 *
		 * Create mapping that maps all possible
		 * outputs to some internal ID
		 *
		 *========================================*/
		std::map<answer, unsigned int> output_id;
		unsigned int output_count = 0;
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u] && output_id.count(t.output[u]) == 0) {
				output_id[t.output[u]] = output_count++;
			}
		}

	
		/*========================================
		 *
		 * Create solver
		 *
		 *========================================*/
		z3::context c;
		z3::solver s(c);
		
		
		/*========================================
		 *
		 * Create enum, functions, and variables
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
		
		// Create alphabet enum
		Z3_symbol a_name = Z3_mk_string_symbol(c, "alphabet");
		Z3_symbol a_names[this->alphabet_size];
		Z3_func_decl a_consts[this->alphabet_size];
		Z3_func_decl a_testers[this->alphabet_size];
		for(int i=0; i<this->alphabet_size; i++) {
		
			std::stringstream tmp;
			tmp << "a" << i;
			a_names[i] = Z3_mk_string_symbol(c, tmp.str().c_str());
		
		}
		z3::sort alphabet_sort(c, Z3_mk_enumeration_sort(c, a_name, this->alphabet_size, a_names, a_consts, a_testers));
		std::vector<z3::func_decl> alphabet_consts;
		std::vector<z3::func_decl> alphabet_testers;
		for(int i=0; i<this->alphabet_size; i++) {
			alphabet_consts.push_back(z3::func_decl(c, a_consts[i]));
			alphabet_testers.push_back(z3::func_decl(c, a_testers[i]));
		}
		
		// Create output enum
		Z3_symbol o_name = Z3_mk_string_symbol(c, "output");
		Z3_symbol o_names[output_count];
		Z3_func_decl o_consts[output_count];
		Z3_func_decl o_testers[output_count];
		for(unsigned int i=0; i<output_count; i++) {
		
			std::stringstream tmp;
			tmp << "out" << i;
			o_names[i] = Z3_mk_string_symbol(c, tmp.str().c_str());
		
		}
		z3::sort output_sort(c, Z3_mk_enumeration_sort(c, o_name, output_count, o_names, o_consts, o_testers));
		//std::vector<z3::func_decl> output_consts;
		std::vector<z3::func_decl> output_testers;
		for(unsigned int i=0; i<output_count; i++) {
			//output_consts.push_back(z3::func_decl(c, o_consts[i]));
			output_testers.push_back(z3::func_decl(c, o_testers[i]));
		}

		// Functions
		z3::func_decl d = function("d", state_sort, alphabet_sort, state_sort);
		z3::func_decl f = function("f", state_sort, output_sort);
		
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
		s.add(state_testers[prefix_tree<answer>::root](state_vars[0]));
		assertion_count++;
		
		// (2) Transitions are applied correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
			
				if(t.edges[u][a] != prefix_tree<answer>::no_edge) {
				
					z3::ast tmp (c, Z3_mk_app(c, a_consts[a], 0, NULL));
					z3::expr args[] = {state_vars[u], to_expr(c, tmp)};
					s.add(state_vars[t.edges[u][a]] == d(2, args));
		
					assertion_count++;
					
				}
			
			}
		}
		
		// (3) Words are classified correctly
		for(unsigned int u=0; u<t.node_count; u++) {
				
			if(t.specified[u]) {
			
				s.add(output_testers[output_id[t.output[u]]](f(state_vars[u])));

				assertion_count++;
			
			}
		}
		
		
		/*========================================
		 *
		 * Solve
		 *
		 *========================================*/
		(*this->my_logger)(LOGGER_ALGORITHM, "Created %u assertions.\n", assertion_count);
		(*this->my_logger)(LOGGER_ALGORITHM, "Solving ...\n");
		if(s.check() != z3::sat) {
			(*this->my_logger)(LOGGER_ALGORITHM, "Formula is unsatisfiable.\n");
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
		for(unsigned int q=0; q<n; q++) {
			for(int a=0; a<this->alphabet_size; a++) {
				
				// Check which of the finite many states is defined by the model
				bool dest_found = false;
				unsigned int dest = 0;
				for(unsigned int p=0; p<n; p++) {
				
					// Get info from model
					z3::ast tmp1 (c, Z3_mk_app(c, s_consts[q], 0, NULL));
					z3::ast tmp2 (c, Z3_mk_app(c, a_consts[a], 0, NULL));
					z3::expr args[] = {to_expr(c, tmp1), to_expr(c, tmp2)};
					z3::expr result = m.eval(state_testers[p](d(2, args)));

					Z3_bool conversion_ok = Z3_get_bool_value(c, result);
					assert(conversion_ok != Z3_L_UNDEF);

					// Check destination
					if(conversion_ok == Z3_L_TRUE) {
					
						assert(!dest_found);
						dest_found = true;
						dest = p;

					}
					
				}
				assert(dest_found && dest < n);
				
				// Add transition
				transitions[q][a].insert(dest);

			}
		}
		
		// Initial state
		std::set<int> initial;
		initial.insert(prefix_tree<answer>::root);
		
		// Final states
		std::map<int, answer> output_mapping;
		for(unsigned int q=0; q<n; q++) {
			
			// Retrive value from model
			bool output_found = false;
			unsigned int output = 0;
			for(unsigned int i=0; i<output_count; i++) {

				z3::ast tmp (c, Z3_mk_app(c, s_consts[q], 0, NULL));
				Z3_lbool result = Z3_get_bool_value(c, m.eval(output_testers[i](f(to_expr(c, tmp)))));
				assert(result != Z3_L_UNDEF);
			
				// Add final state
				if(result == Z3_L_TRUE) {

					assert(!output_found);
					output_found = true;
					output = i;

				}

			}
			assert(output_found && output < output_count);

			output_mapping[q] = find_key(output_id, output);
			
		}
		assert(output_mapping.size() == n);
		
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


	protected:

	/**
	 * Searches a map for a key with a given value.
	 *
	 * NOTE: This methid requires the given value to be present in the map!
	 *
	 */
	answer find_key(const std::map<answer, unsigned int> & m, unsigned value) const {
	
		for(typename std::map<answer, unsigned int>::const_iterator it=m.begin(); it!=m.end(); it++) {
			if(it->second == value) {
				return it->first;
			}
		}

		(*this->my_logger)(LOGGER_ERROR, "Could not find key.\n");
		assert(false);
		return answer();

	}
	
};

}; // End libalf namespace

#endif
