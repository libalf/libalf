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

#ifndef __ALGORITHM_NFA_INFERRING_MINISAT__
#define __ALGORITHM_NFA_INFERRING_MINISAT__

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
#include "libalf/minisat/Solver.h"

namespace libalf {

class nfa_inferring_MiniSat : public automata_inferring<bool> {

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
	nfa_inferring_MiniSat(knowledgebase<bool> * base, logger * log, int alphabet_size, bool symmetry_breaking = true) {

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
	
		os << "(Minimal) NFA inferring algorithm via transitions encoding using the MiniSat SAT Solver. ";
		os << "Alphabet size is " << this->alphabet_size;
		os << (symmetry_breaking ? " using symmetry breaking" : "") << ".";
		
	}

	private:

	virtual conjecture * __infer(const prefix_tree<bool> & t, unsigned int n) const {
	
		// Check value for n
		if(n == 0) {
			(*this->my_logger)(LOGGER_ERROR, "The number 'n' of states has to be greater than 0.\n");
			return NULL;
		}

		return infer_MiniSat(t, n);

	}
	
	libalf::finite_automaton * infer_MiniSat(const prefix_tree<bool> & t, unsigned int n) const {

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
		(*this->my_logger)(LOGGER_ALGORITHM, "Running MiniSat using the 'transition encoding' to find an NFA with %d states and alphabet size %d.\n", n, this->alphabet_size);
		
		
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

		// Final states
		std::map<int, MiniSat::Var> f; // p \in F
		for(unsigned int q=0; q<n; q++) {
			f[q] = solver.newVar();
			var_count++;
		}

		// y variables (needed for correct transitions)
		std::map<int, std::map<int, std::map<int, MiniSat::Var> > > y; // y_{ua, p, q}		
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<bool>::no_edge) {

					for(unsigned int p=0; p<n; p++) {
						for(unsigned int q=0; q<n; q++) {

							y[t.edges[u][a]][p][q] = solver.newVar();
							var_count++;

						}
					}


				}
			}
		}

		// z variables (needed for final states)
		std::map<int, std::map<int, MiniSat::Var> > z; // z_{u, q}
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int q=0; q<n; q++) {

				z[u][q] = solver.newVar();
				var_count++;

			}
		}
		
		
		/*========================================
		 *
		 * Create clauses
		 *
		 *========================================*/
		unsigned long long clause_count = 0;
		
		// (0) At least one initial state exists (otherwise we might get automata, e.g., for the empty language) without any states)
		solver.addUnit(MiniSat::Lit(x[prefix_tree<bool>::root][0]));
		clause_count++;

		// (1) Transitions are applied correctly (forward): (x[u][p] && d[p][a][q]) => x[ua][q]
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<bool>::no_edge) {

					for(unsigned int p=0; p<n; p++) {
						for(unsigned int q=0; q<n; q++) {

							solver.addTernary(~MiniSat::Lit(x[u][p]), ~MiniSat::Lit(d[p][a][q]), MiniSat::Lit(x[t.edges[u][a]][q]));

						}
					}

				}
			}
		}
		
		// (2a) Variables y_{u, p, q} are set correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<bool>::no_edge) {

					for(unsigned int p=0; p<n; p++) {
						for(unsigned int q=0; q<n; q++) {

							solver.addBinary(~MiniSat::Lit(y[t.edges[u][a]][p][q]), MiniSat::Lit(x[u][p]));
							solver.addBinary(~MiniSat::Lit(y[t.edges[u][a]][p][q]), MiniSat::Lit(d[p][a][q]));

							clause_count += 2;

						}
					}

				}
			}
		}

		// (3) If a state is assigned at some point, then there has to be at least one transition from a predecessor
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<bool>::no_edge) {

					for(unsigned int q=0; q<n; q++) {

						MiniSat::vec<MiniSat::Lit> clause;
						clause.growTo(n+1);

						for(unsigned int p=0; p<n; p++) {
							clause[p] = MiniSat::Lit(y[t.edges[u][a]][p][q]);
						}
						clause[n] = ~MiniSat::Lit(x[t.edges[u][a]][q]);
						
						solver.addClause(clause);
						clause_count++;

					}
					
				}
			}
		}

		// (4a) Variables z_{u, q} are set correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int q=0; q<n; q++) {
		
				solver.addBinary(~MiniSat::Lit(z[u][q]), MiniSat::Lit(x[u][q]));
				solver.addBinary(~MiniSat::Lit(z[u][q]), MiniSat::Lit(f[q]));
				clause_count += 2;
		
			}
		}
		
		// (5a) Positively classified words lead to accepting states
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u] && t.output[u] == true) {
				
				MiniSat::vec<MiniSat::Lit> clause;
				clause.growTo(n);

				for(unsigned int q=0; q<n; q++) {
					clause[q] = MiniSat::Lit(z[u][q]);
				}
					
				solver.addClause(clause);
				clause_count++;

			}
		}
		
		// (5b) Negatively classified words do not lead to accepting states
		for(unsigned int u=0; u<t.node_count; u++) {
			if(t.specified[u] && t.output[u] == false) {
				for(unsigned int q=0; q<n; q++) {

					solver.addBinary(~MiniSat::Lit(x[u][q]), ~MiniSat::Lit(f[q]));
					clause_count++;
					
				}
			}
		}
		

		// Add redundant clauses if wanted
		if(symmetry_breaking) {

		// (2b) Variables y_{u, p, q} are set correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(int a=0; a<this->alphabet_size; a++) {
				if(t.edges[u][a] != prefix_tree<bool>::no_edge) {

					for(unsigned int p=0; p<n; p++) {
						for(unsigned int q=0; q<n; q++) {

							solver.addTernary(~MiniSat::Lit(x[u][p]), ~MiniSat::Lit(d[p][a][q]), MiniSat::Lit(y[t.edges[u][a]][p][q]));

							clause_count++;

						}
					}

				}
			}
		}

		// (4b) Variables z_{u, q} are set correctly
		for(unsigned int u=0; u<t.node_count; u++) {
			for(unsigned int q=0; q<n; q++) {
		
				solver.addTernary(~MiniSat::Lit(x[u][q]), ~MiniSat::Lit(f[q]), MiniSat::Lit(z[u][q]));
				clause_count ++;
		
			}
		}
		
		} // End adding redundant clauses

		
		/*========================================
		 *
		 * Solve
		 *
		 *========================================*/
		(*this->my_logger)(LOGGER_ALGORITHM, "Created %d variables and %d clauses.\n", var_count, clause_count);
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
						(*this->my_logger)(LOGGER_ALGORITHM, "d[%d][%d][%d] = %s\n", p, a, q, (solver.model[d[p][a][q]] == MiniSat::l_True ? "1" : (solver.model[d[p][a][q]] == MiniSat::l_False ? "0" : "?")));
					}
				}
			}
			// Final states
			for(unsigned int q=0; q<n; q++) {
				(*this->my_logger)(LOGGER_ALGORITHM, "f[%d] = %s\n", q, (solver.model[f[q]] == MiniSat::l_True ? "1" : (solver.model[f[q]] == MiniSat::l_False ? "0" : "?")));
			}
			// States
			for(unsigned int u=0; u<t.node_count; u++) {
				for(unsigned int q=0; q<n; q++) {
					(*this->my_logger)(LOGGER_ALGORITHM, "x[%d][%d] = %s\n", u, q, (solver.model[x[u][q]] == MiniSat::l_True ? "1" : (solver.model[x[u][q]] == MiniSat::l_False ? "0" : "?")));
				}
			}
			// z variables
			for(unsigned int u=0; u<t.node_count; u++) {
				for(unsigned int q=0; q<n; q++) {
					(*this->my_logger)(LOGGER_ALGORITHM, "z[%d][%d] = %s\n", u, q, (solver.model[z[u][q]] == MiniSat::l_True ? "1" : (solver.model[z[u][q]] == MiniSat::l_False ? "0" : "?")));
				}
			}
			
		}
		
		// Transitions
		std::map<int, std::map<int, std::set<int> > > transitions;
		for(unsigned int p=0; p<n; p++) {
			for(int a=0; a<this->alphabet_size; a++) {

				// Find q such that d[p][a][q] is true
				for(unsigned int q=0; q<n; q++) {

					assert(solver.model[d[p][a][q]] != MiniSat::l_Undef);
				
					if(solver.model[d[p][a][q]] == MiniSat::l_True) {
						transitions[p][a].insert(q);						
					} 

				}

			}
		}
		
		// Initial state
		std::set<int> initial;
		for(unsigned int q=0; q<n; q++) {

			assert(solver.model[x[prefix_tree<bool>::root][q]] != MiniSat::l_Undef);
		
			if(solver.model[x[prefix_tree<bool>::root][q]] == MiniSat::l_True) {
				initial.insert(q);
			}

		}
		assert(initial.size() > 0);
		
		// Final states
		std::set<int> final;
		for(unsigned int q=0; q<n; q++) {

			assert(solver.model[x[0][q]] != MiniSat::l_Undef);
		
			if(solver.model[f[q]] == MiniSat::l_True) {
				final.insert(q);
			}

		}
		
		// Construct and return automaton
		finite_automaton * dfa = new finite_automaton;
		dfa->input_alphabet_size = alphabet_size;
		dfa->state_count = n;
		dfa->initial_states = initial;
		dfa->set_final_states(final);
		dfa->transitions = transitions;
		dfa->valid = true;
		dfa->calc_determinism();

		assert(dfa->calc_validity());
		return dfa;

	}

	public:

	virtual enum learning_algorithm_type get_type() const
	{ return ALG_INFERRING_MINISAT; };

	virtual enum learning_algorithm_type get_basic_compatible_type() const
	{ return ALG_INFERRING_MINISAT; };

	virtual std::basic_string<int32_t> serialize() const
	{
		std::basic_string<int32_t> ret;

		// length (filled in later)
		ret += 0;

		// implementation type
		ret += ::serialize(ALG_INFERRING_CSP_MINISAT);

		// alphabet size
		ret += ::serialize(this->get_alphabet_size());

		// symmetry_breaking
		ret += ::serialize(symmetry_breaking);

		// log model
		ret += ::serialize(log_model);

		// Set length
		ret[0] = htonl(ret.length() - 1);

		return ret;
	}


	virtual bool deserialize(serial_stretch & serial)
	{
		int s;

		// Parse size
		if(!::deserialize(s, serial)) return false;

		// Implementation type
		if(!::deserialize(s, serial)) return false;
		if(s != ALG_INFERRING_CSP_MINISAT) return false;

		// alphabet size
		if(!::deserialize(s, serial)) return false;
		if(s <= 0) return false;
		this->set_alphabet_size(s);

		// symmetry_breaking
		if(!::deserialize(symmetry_breaking, serial)) return false;

		// log model
		if(!::deserialize(log_model, serial)) return false;

		return true;
	}


};

}; // End libalf namespace

#endif
