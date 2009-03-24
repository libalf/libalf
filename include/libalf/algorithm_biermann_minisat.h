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

#include <libalf/algorithm_biermann.h>

#include <libalf/minisat_Solver.h>

namespace libalf {


using namespace std;


// biermann using CSP2SAT + MiniSat, as described in
// "M. Leucker, O. Grinchtein, N. Piterman - Inferring Network Invariants Automatically"
template <class answer>
class MiniSat_biermann : public basic_biermann<answer> {
	protected:
		MiniSat::Solver solver;
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
		virtual bool solve_constraints()
		{
			// FIXME
			
			return false;
		}
};


}; // end namespace libalf

#endif // __libalf_algorithm_biermann_minisat_h__

