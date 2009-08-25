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

#ifndef __libalf_statistics_h__
# define __libalf_statistics_h__

#include <string>
#include <iterator>

namespace libalf {

using namespace std;

class statistics {
	public: // data									// changed by:
		struct {
			int membership;							// reserved for user, e.g. from knowledgebase::count_resolved_queries()
			int uniq_membership;						// reserved for user while resolving a query
			int equivalence;						// reserved for user
		} queries;

		struct {
			int bytes;	// bytes of algorithms data structure		// algorithm
			int members;	// number of membership data			// algorithm
			int words;	// number of words in table			// algorithm
			int upper_table;// size of upper table (if appropriate)		// algorithm
			int lower_table;// size of lower table (if appropriate)		// algorithm
			int columns;	// columns (if appropriate)			// algorithm
		} memory;

		struct {
			int cpu_sec;							// algorithm
			int cpu_usec;							// algorithm
			int sys_sec;							// algorithm
			int sys_usec;							// algorithm
		} time;


	public: // methods
		statistics();
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

}; // end namespace libalf

#endif

