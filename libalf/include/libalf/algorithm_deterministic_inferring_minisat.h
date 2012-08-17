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

#ifndef __ALGORITHM_DETERMINISTIC_INFERRING_MINISAT__
#define __ALGORITHM_DETERMINISTIC_INFERRING_MINISAT__

// Standard includes 
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <vector>

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
class deterministic_inferring_MiniSat : public automata_inferring<answer> {

	private:

	/**
	 * Indicates whether we add additional constraints to perform symmetry
	 * breaking.
	 */
	bool symmetry_breaking;
	
	/**
	 * Indicates whether the computed model should be logged (to the algorithm loglevel)
	 */
	bool log_model;
	
	public:

	/**
	 * Creates a new learning algorithm.
	 */
	deterministic_inferring_MiniSat(knowledgebase<answer> * base, logger * log, int alphabet_size, bool symmetry_breaking = true) : automata_inferring<answer>() {

		this->set_alphabet_size(alphabet_size);
		this->set_logger(log);
		this->set_knowledge_source(base);
		
		this->symmetry_breaking = symmetry_breaking;

		this->log_model = false;
	}

	/**
	 * Returns whether we add additional constraints to perform symmetry
	 * breaking.
	 *
	 * @return Returns whether we add additional constraints to perform symmetry
	 *         breaking.
	 */
	bool uses_symmetry_breaking() {
		return symmetry_breaking;
	}

	/**
	 * Sets whether we add additional constraints to perform symmetry
	 * breaking.
	 *
	 * @param symmetry_breaking Indicates whether to use add additional
	 *                          constraints
	 */
	void set_symmetry_breaking(bool symmetry_breaking) {
		this->symmetry_breaking = symmetry_breaking;
	}

	bool is_logging_model() {
		return log_model;
	}
	
	void set_log_model(bool log_model) {
		this->log_model = log_model;
	}
	
	void print(std::ostream & os) const {
	
		os << "(Minimal) Moore machine inferring algorithm via transitions encoding using the MiniSat SAT Solver. ";
		os << "Alphabet size is " << this->alphabet_size;
		os << (symmetry_breaking ? " using symmetry breaking" : "") << ".";
		
	}

	private:

	virtual conjecture * __infer(const prefix_tree<answer> & t, unsigned int n) const {
	
		// Check value for n
		if(n == 0) {
			(*this->my_logger)(LOGGER_ERROR, "The number 'n' of states has to be greater than 0.\n");
			return NULL;
		}

		return infer_MiniSat(t, n);

	}
	
	libalf::moore_machine<answer> * infer_MiniSat(const prefix_tree<answer> & t, unsigned int n) const {

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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running MiniSat using the 'transition encoding' to find a solution with %u states and alphabet size %d.\n", n, this->alphabet_size);
		

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
		MiniSat::Solver solver;
		
		
		/*========================================
		 *
		 * Create variables
		 *
		 *========================================*/
		unsigned long var_count = 0;
			
		// Labeling of the prefixes
		std::map<int, std::map<int, MiniSat::Var> > x; // x_{u, q}
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int q=0; q<n; q++) {
				x[u][q] = solver.newVar();
				var_count++;
			}
		}
		
		// Transitions
		std::map<int, std::map<int, std::map<int, MiniSat::Var> > > d; // d(p, a) = q
		for(unsigned int p=0; p<n; p++) {
			for(int a=0; a<this->alphabet_size; a++) {
				for(unsigned int q=0; q<n; q++) {
					d[p][a][q] = solver.newVar();
					var_count++;
				}
			}
		}

		// Output
		std::map<int, std::map<int, MiniSat::Var> > f; // f_{q, i}
		for(unsigned int q=0; q<n; q++) {
			for(unsigned int i=0; i<output_count; i++) {
				f[q][i] = solver.newVar();
			}
			var_count += output_count;
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
		
		// (2) The variables d encode a function (not a relation)
		for(unsigned int p=0; p<n; p++) {
			for(int a=0; a<this->alphabet_size; a++) {

				for(unsigned int q1=0; q1<n; q1++) {
					for(unsigned int q2=0; q2<q1; q2++) {

						solver.addBinary(~MiniSat::Lit(d[p][a][q1]), ~MiniSat::Lit(d[p][a][q2]));

						clause_count++;

					}
				}

			}
		}
		
		// (3) Transitions are applied correctly: (x[u][p] && x[ua][q]) => d[p][a][q]
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<answer>::no_edge) {

					for(unsigned int p=0; p<n; p++) {
						for(unsigned int q=0; q<n; q++) {

							solver.addTernary(~MiniSat::Lit(x[u][p]), MiniSat::Lit(d[p][a][q]), ~MiniSat::Lit(x[t.edges[u][a]][q]));

							clause_count++;

						}
					}

				}
			}
		}
		
		// (4a) At leat one output is defined
		for(unsigned int q=0; q<n; q++) {

			MiniSat::vec<MiniSat::Lit> clause;
			clause.growTo(output_count);

			for(unsigned int i=0; i<output_count; i++) {
				clause[i] = MiniSat::Lit(f[q][i]);
			}

			solver.addClause(clause);
			
			clause_count++;

		}

		// (4b) At most one output is defined
		for(unsigned int q=0; q<n; q++) {
			for(unsigned int i=0; i<output_count; i++) {
				for(unsigned int j=0; j<i; j++) {

					solver.addBinary(~MiniSat::Lit(f[q][i]), ~MiniSat::Lit(f[q][j]));

					clause_count++;

				}
			}
		}

		// (5) Output is correct
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u]) {
				for(unsigned int q=0; q<n; q++) {

					solver.addBinary(~MiniSat::Lit(x[u][q]), MiniSat::Lit(f[q][output_id[t.output[u]]]));

					clause_count++;

				}
			}
		}


		// Add redundant clauses if wanted
		if(symmetry_breaking) {

		// (6) For each prefix, at most one state is assumed
		for(unsigned int u=0; u<t.node_count; u++) {	
			for(unsigned int p=0; p<n; p++) {
				for(unsigned int q=0; q<p; q++) {

					solver.addBinary(~MiniSat::Lit(x[u][p]), ~MiniSat::Lit(x[u][q]));

					clause_count++;

				}
			}
		}
		
		// (7) The transition function is completely defined
		for(unsigned int p=0; p<n; p++) {
			for(int a=0; a<this->alphabet_size; a++) {

				MiniSat::vec<MiniSat::Lit> clause;
				clause.growTo(n);

				for(unsigned int q=0; q<n; q++) {
					clause[q] = MiniSat::Lit(d[p][a][q]);
				}

				solver.addClause(clause);

				clause_count++;

			}
		}
		
		// (8) Transitions are applied correctly: (x[u][p] && d[p][a][q]) => x[ua][q]
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<answer>::no_edge) {

					for(unsigned int p=0; p<n; p++) {
						for(unsigned int q=0; q<n; q++) {

							solver.addTernary(~MiniSat::Lit(x[u][p]), ~MiniSat::Lit(d[p][a][q]), MiniSat::Lit(x[t.edges[u][a]][q]));

							clause_count++;

						}
					}

				}
			}
		}
		
		} // End adding redundant clauses
		
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
			// Transitions
			for(unsigned int p=0; p<n; p++) {
				for(int a=0; a<this->alphabet_size; a++) {
					for(unsigned int q=0; q<n; q++) {
						(*this->my_logger)(LOGGER_ALGORITHM, "d[%u][%u][%u] = %s\n", p, a, q, (solver.model[d[p][a][q]] == MiniSat::l_True ? "1" : (solver.model[d[p][a][q]] == MiniSat::l_False ? "0" : "?")));
					}
				}
			}
			// Output
			for(unsigned int q=0; q<n; q++) {
				for(unsigned int i=0; i<output_count; i++) {
					(*this->my_logger)(LOGGER_ALGORITHM, "f[%u][%u] = %s\n", q, i, (solver.model[f[q][i]] == MiniSat::l_True ? "1" : (solver.model[f[q][i]] == MiniSat::l_False ? "0" : "?")));
				}
			}
			// States
			for(unsigned int u=0; u<t.node_count; u++) {
				for(unsigned int q=0; q<n; q++) {
					(*this->my_logger)(LOGGER_ALGORITHM, "x[%u][%u] = %s\n", u, q, (solver.model[x[u][q]] == MiniSat::l_True ? "1" : (solver.model[x[u][q]] == MiniSat::l_False ? "0" : "?")));
				}
			}
			
		}
		
		// Transitions
		std::map<int, std::map<int, std::set<int> > > transitions;
		for(unsigned int p=0; p<n; p++) {
			for(int a=0; a<this->alphabet_size; a++) {

				// Find (unique) q such that d[p][a][q] is true
				bool found_dest = false;
				unsigned int dest = 0;
				for(unsigned int q=0; q<n; q++) {

					assert(solver.model[d[p][a][q]] != MiniSat::l_Undef);
				
					if(solver.model[d[p][a][q]] == MiniSat::l_True) {
		
						assert(!found_dest);
						found_dest = true;
						dest = q;
						
					} 

				}
				
				// If no symmetry breaking, then there might be undefined transitions
				if(symmetry_breaking) {
					assert(found_dest && dest < n);
				}
				
				if(found_dest) {
					transitions[p][a].insert(dest);
				}

			}
		}
		
		// Initial state
		bool initial_found = false;
		unsigned int tmp_initial = 0;
		for(unsigned int q=0; q<n; q++) {

			assert(solver.model[x[0][q]] != MiniSat::l_Undef);
		
			if(solver.model[x[0][q]] == MiniSat::l_True) {
			
				assert(!initial_found);
				initial_found = true;
				tmp_initial = q;
			
			}

		}
		assert(initial_found && tmp_initial < (int)n);
		std::set<int> initial;
		initial.insert(tmp_initial);
		
		// Output
		std::map<int, answer> output_mapping;
		for(unsigned int q=0; q<n; q++) {

			bool found_output = false;
			unsigned int output = 0;
			for(unsigned int i=0; i<output_count; i++) {

				assert(solver.model[f[q][i]] != MiniSat::l_Undef);
			
				if(solver.model[f[q][i]] == MiniSat::l_True) {
					
					assert(!found_output);
					found_output = true;
					output = i;

				}

			}
			assert(found_output && output < output_count);

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

	private:

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
