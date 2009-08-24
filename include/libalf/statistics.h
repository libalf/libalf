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
	public: // data
		struct {
			int bytes;	// bytes of algorithms data structure
			int members;	// number of membership data
			int words;	// number of words in table
			int upper_table;// size of upper table (if appropriate)
			int lower_table;// size of lower table (if appropriate)
			int columns;	// columns (if appropriate)
		} memory;

		struct {
			int membership;
			int uniq_membership;
			int equivalence;
		} queries;

		struct {
			int cpu_sec;
			int cpu_usec;
			int sys_sec;
			int sys_usec;
		} time;


	public: // methods
		statistics();
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

}; // end namespace libalf

#endif

