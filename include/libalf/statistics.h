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

class query_statistics {
	public:	// data
		int32_t membership;
		int32_t uniq_membership;
		int32_t equivalence;
	public:	// methods
		query_statistics();
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

class memory_statistics {
	public:	// data
		int32_t bytes;		// bytes of algorithms data structure
		int32_t members;	// number of membership data
		int32_t words;		// number of words in table
		int32_t upper_table;	// size of upper table (if appropriate)
		int32_t lower_table;	// size of lower table (if appropriate)
		int32_t columns;	// columns (if appropriate)
	public:	// methods
		memory_statistics();
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

class timing_statistics {
	public:	// data
		int32_t user_sec;
		int32_t user_usec;
		int32_t sys_sec;
		int32_t sys_usec;
	public: // methods
		timing_statistics();
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

class statistics {
	public: // data
		query_statistics queries;
		memory_statistics memory;
		timing_statistics time;

	public: // methods
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

}; // end namespace libalf

#endif

