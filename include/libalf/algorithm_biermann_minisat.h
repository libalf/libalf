/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
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

// biermann using CSP2SAT + MiniSat, as described in
// "M. Leucker, O. Grinchtein, N. Piterman - Inferring Network Invariants Automatically"
template <class answer>
class MiniSat_biermann : public basic_biermann<answer> {
	public:
		MiniSat_biermann(knowledgebase<answer> * base, logger * log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(NULL, base);
		}}}
		virtual ~MiniSat_biermann()
		{{{
			// nothing.
			return;
		}}}

	protected:
		virtual bool csp2sat(MiniSat::Solver & solver, map<typename basic_biermann<answer>::knowledgebase_node_ptr, vector<MiniSat::Var>, typename basic_biermann<answer>::node_comparator > & vars)
		{{{
			typename set< typename basic_biermann<answer>::knowledgebase_node_ptr >::iterator si;
			int clausecount = 0;

			// create vars
			// and on the fly add clauses enforcing unary encoding
			for(si = this->sources.begin(); si != this->sources.end(); si++) {
				vector<MiniSat::Var> state_vars;
				vector<MiniSat::Var>::iterator svi;
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
					MiniSat::Var v;

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
				while(!carry) {
					// NOTE: any boolean function may be represented by
					// conjunction of minterms of its indexes evaluation to 0.

					if(set_count != 1) {
						typename MiniSat::vec<MiniSat::Lit> clause;
						clause.growTo(this->mdfa_size);
						// create minterm for this index
printf("\tclause");
						for(svi = state_vars.begin(), i = 0; svi != state_vars.end(); svi++, i++) {
							MiniSat::Lit l(*svi, !varcounter[i]);
							clause[i] = l;
printf(" %c%2d", MiniSat::sign(l) ? '!' : ' ', MiniSat::var(l));
						}
						clausecount++;
printf("\n");
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
printf("constraints\n");

			// add clauses for each constraint:
			typename list<typename basic_biermann<answer>::constraint>::iterator csi;
			for(csi = this->constraints.begin(); csi != this->constraints.end(); csi++) {
				vector<MiniSat::Var>::iterator svi1, svi2, svi1e, svi2e;

				svi1 = vars[csi->l1].begin();
				svi1e = vars[csi->l1].end();
				svi2 = vars[csi->l2].begin();
				svi2e = vars[csi->l2].end();

				while(svi1 != svi1e && svi2 != svi2e) {
					MiniSat::Lit l1(*svi1, true);
					MiniSat::Lit l2(*svi2, true);

					if(csi->has_second) {
						MiniSat::vec<MiniSat::Lit> clause;
						vector<MiniSat::Var>::iterator svi3, svi4, svi3e, svi4e;

						svi3 = vars[csi->l3].begin();
						svi3e = vars[csi->l3].end();
						svi4 = vars[csi->l4].begin();
						svi4e = vars[csi->l4].end();

						while(svi3 != svi3e && svi4 != svi4e) {
							MiniSat::vec<MiniSat::Lit> clause;
							MiniSat::Lit l3(*svi3, false);
							MiniSat::Lit l4(*svi4, true);
							clause.growTo(4);
							clause[0] = l1;
							clause[1] = l2;
							clause[2] = l3;
							clause[3] = l4;
printf("\tclause %c%2d %c%2d %c%2d %c%2d\n", MiniSat::sign(l1) ? '!' : ' ', MiniSat::var(l1), MiniSat::sign(l2) ? '!' : ' ', MiniSat::var(l2), MiniSat::sign(l3) ? '!' : ' ', MiniSat::var(l3), MiniSat::sign(l4) ? '!' : ' ', MiniSat::var(l4));
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
						MiniSat::vec<MiniSat::Lit> clause;
						clause.growTo(2);
						clause[0] = l1;
						clause[1] = l2;

printf("\tclause %c%2d %c%2d\n", MiniSat::sign(l1) ? '!' : ' ', MiniSat::var(l1), MiniSat::sign(l2) ? '!' : ' ', MiniSat::var(l2));
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
			(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: translated CSP to %d clauses of %d vars.\n", clausecount, solver.nVars());
			return true;
		}}}

		virtual bool solve_constraints()
		{
			MiniSat::Solver solver;
			map<typename basic_biermann<answer>::knowledgebase_node_ptr, vector<MiniSat::Var>, typename basic_biermann<answer>::node_comparator > vars;

//			solver.verbosity = 1;

			if(!csp2sat(solver, vars))
				return false;

			solver.simplifyDB();
			if(!solver.okay()) {
				(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: SAT: early conflict found.\n");
				return false;
			}

			(*this->my_logger)(LOGGER_ALGORITHM, "biermann+MiniSat: trying to solve %d clauses.\n", solver.nClauses());
			if(solver.solve()) {
				//solver.model
printf("satisfiable ");
for (int i = 0; i < solver.nVars(); i++)
	if (solver.model[i] != MiniSat::l_Undef)
		printf(" %c%d", (solver.model[i]==MiniSat::l_True)?' ':'!', i);
printf("\n");

				
				return true;
			} else {
				return false;
			}
		}
};


}; // end namespace libalf

#endif // __libalf_algorithm_biermann_minisat_h__

