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

#ifndef __libalf_statistics_h__
# define __libalf_statistics_h__

#include <stdint.h>

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

