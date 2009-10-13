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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */



#ifndef __libalf_algorithm_biermann_minisat_h__
# define __libalf_algorithm_biermann_minisat_h__

#include <list>
#include <vector>
#include <set>

#include <libalf/algorithm_biermann.h>

#include <libalf/minisat_Solver.h>

namespace libalf {


using namespace std;
using namespace libalf::MiniSat;

// biermann using CSP2SAT + MiniSat, as described in
//	* M. Leucker, O. Grinchtein, N. Piterman: "Inferring Network Invariants Automatically"
template <class answer>
class MiniSat_biermann : public basic_biermann<answer> {
	public: // types
		typedef typename knowledgebase<answer>::node node;
	public:
		MiniSat_biermann(knowledgebase<answer> * base, logger * log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(base);
		}}}
		virtual ~MiniSat_biermann()
		{{{
			// nothing.
			return;
		}}}

	protected:
		virtual bool csp2sat(Solver & solver, map<node*, vector<Var>, typename knowledgebase<answer>::node_comparator > & vars)
		{{{
			typename set< node* >::iterator si;
			int clausecount = 0;

			// create vars
			// and on the fly add clauses enforcing unary encoding
			for(si = this->sources.begin(); si != this->sources.end(); si++) {
				vector<Var> state_vars;
				vector<Var>::iterator svi;
				bool varcounter[this->mdfa_size];
				bool carry;
				int set_count;
				int i;

//list<int> word;
//string s;
//word = (*si)->get_word();
//s = word2string(word, '.');
//printf("\n\t%s new vars", s.c_str());
				for(int i = 0; i < this->mdfa_size; i++) {
					Var v;

					v = solver.newVar();
//printf(" %d", v);
					state_vars.push_back(v);
				}
//printf("\n");

				// remember vars v_{x,0} .. v_{x,mdfa_size-1}
				vars[*si] = state_vars;

				// and add clauses that enforces a valid number with only one digit set.
				for(i = 0; i < this->mdfa_size; i++)
					varcounter[i] = false;
				carry = false;
				set_count = 0;
				while(!carry) {
					// NOTE: any boolean function may be represented by
					// conjunction of minterms of its indexes evaluation to 0.

					if(set_count != 1) {
						vec<Lit> clause;
						clause.growTo(this->mdfa_size);
						// create minterm for this index
//printf("\tclause");
						for(svi = state_vars.begin(), i = 0; svi != state_vars.end(); svi++, i++) {
							clause[i] = varcounter[i] ? ~Lit(*svi) : Lit(*svi);
//printf(" %c%2d", sign(clause[i]) ? '!' : ' ', var(clause[i]));
						}
						clausecount++;
//printf("\n");
						solver.addClause(clause);
						if(!solver.okay()) {
							(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: SAT: found conflict during CSP2SAT.\n", clausecount);
							return false;
						}
					}

					// increment: try next clause
					// FIXME: this is far too inefficient
					carry = true;
					set_count = 0;
					for(i = this->mdfa_size - 1; i >= 0; i--) {
						if(carry) {
							if(varcounter[i]) {
								varcounter[i] = false;
							} else {
								varcounter[i] = true;
								set_count++;
								carry = false;
							}
						} else {
							if(varcounter[i]) {
								set_count++;
								if(set_count > 1)
									break;
							}
						}
					}
					// if carry is still true, we've got an overflow and are done.
				}
			}
//printf("\n");

			// add clauses for each constraint:
			typename list<typename basic_biermann<answer>::constraint>::iterator csi;
			for(csi = this->constraints.begin(); csi != this->constraints.end(); csi++) {
				vector<Var>::iterator svi1, svi2, svi1e, svi2e;

				svi1 = vars[csi->l1].begin();
				svi1e = vars[csi->l1].end();
				svi2 = vars[csi->l2].begin();
				svi2e = vars[csi->l2].end();

				while(svi1 != svi1e && svi2 != svi2e) {
					if(csi->has_second) {
						vec<Lit> clause;
						vector<Var>::iterator svi3, svi4, svi3e, svi4e;

						svi3 = vars[csi->l3].begin();
						svi3e = vars[csi->l3].end();
						svi4 = vars[csi->l4].begin();
						svi4e = vars[csi->l4].end();

						while(svi3 != svi3e && svi4 != svi4e) {
							vec<Lit> clause;
							clause.growTo(4);
							clause[0] = ~Lit(*svi1);
							clause[1] = ~Lit(*svi2);
							clause[2] = Lit(*svi3);
							clause[3] = ~Lit(*svi4);
//printf("\tclause %c%2d %c%2d %c%2d %c%2d\n", sign(clause[0]) ? '!' : ' ', var(clause[0]), sign(clause[1]) ? '!' : ' ', var(clause[1]), sign(clause[2]) ? '!' : ' ', var(clause[2]), sign(clause[3]) ? '!' : ' ', var(clause[3]));
							clausecount++;
							solver.addClause(clause);
							if(!solver.okay()) {
								(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: SAT: found conflict during CSP2SAT.\n", clausecount);
								return false;
							}

							svi3++;
							svi4++;
						}
					} else {
						vec<Lit> clause;
						clause.growTo(2);
						clause[0] = ~Lit(*svi1);
						clause[1] = ~Lit(*svi2);

//printf("\tclause %c%2d %c%2d\n", sign(clause[0]) ? '!' : ' ', var(clause[0]), sign(clause[1]) ? '!' : ' ', var(clause[1]));
						clausecount++;
						solver.addClause(clause);
						if(!solver.okay()) {
							(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: SAT: found conflict during CSP2SAT.\n", clausecount);
							return false;
						}
					}

					svi1++;
					svi2++;
				}
			}
			(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: translated CSP to %d CNF clauses of %d vars.\n", clausecount, solver.nVars());
			return true;
		}}}

		virtual bool solve_constraints()
		{{{
			Solver solver;
			map<node*, vector<Var>, typename knowledgebase<answer>::node_comparator > vars;

			//solver.verbosity = 1;

			if(!csp2sat(solver, vars))
				return false;

			solver.simplifyDB();
			if(!solver.okay()) {
				(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: SAT: early conflict found.\n");
				return false;
			}

			if(solver.nClauses() <= 0) {
				(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: resolved to 0 clauses. aborting this run.\n", solver.nClauses());
				return false;
			}

			(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: trying to solve %d clauses.\n", solver.nClauses());

			// enforce 0 as initial state.
			vec<Lit> assumptions;
			Lit force_initial(0,false);
			assumptions.push(force_initial);

			if(solver.solve(assumptions)) {
				/*
				printf("satisfiable ");
				for (int i = 0; i < solver.nVars(); i++) {
					if(solver.model[i] != l_Undef)
						printf(" %c%d", (solver.model[i]==l_True)?' ':'!', i);
				}
				printf("\n");
				*/

				typename map<node*, vector<Var>, typename knowledgebase<answer>::node_comparator>::iterator vsi;
				int vindex = 0;

				for(vsi = vars.begin(); vsi != vars.end(); vsi++) {
					vector<Var>::iterator vi;
					bool assigned = false;
					int stateid;
					int i;
					for(i = 0, vi = vsi->second.begin(); vi != vsi->second.end(); i++, vi++, vindex++) {
						if(solver.model[vindex] == l_True) {
							if(assigned) {
								(*this->my_logger)(LOGGER_ERROR, "biermann+MiniSat: received bad var assignment from MiniSat (non-unary encoding: %d and %d).\n", stateid, i);
								return false;
							} else {
								assigned = true;
								stateid = i;
							}
						} else {
							if(solver.model[vindex] != l_False) {
								(*this->my_logger)(LOGGER_ERROR, "biermann+MiniSat: received unassigned var from MiniSat. trying to ignore.\n");
							}
						}
					}

					if(!assigned) {
						(*this->my_logger)(LOGGER_ERROR, "biermann+MiniSat: received bad var assignment from MiniSat (none set at all).\n");
						return false;
					}

					this->solution[vsi->first] = stateid;
				}

				return true;
			} else {
				return false;
			}
		}}}
};


}; // end namespace libalf

#endif // __libalf_algorithm_biermann_minisat_h__

