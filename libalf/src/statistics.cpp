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
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#ifdef _WIN32
#include <winsock.h>
#include <stdint.h>
#else
#include <arpa/inet.h>
#endif

#include <string>
#include <iterator>

#include "libalf/statistics.h"

namespace libalf {

using namespace std;


query_statistics::query_statistics()
{{{
	reset();
}}}
void query_statistics::reset()
{{{
	membership = 0;
	uniq_membership = 0;
	equivalence = 0;
}}}
basic_string<int32_t> query_statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += htonl(3);
	ret += htonl(membership);
	ret += htonl(uniq_membership);
	ret += htonl(equivalence);

	return ret;
}}}
bool query_statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit)
{{{
	int size;

	if(it == limit) goto deserialization_failed;

	// data size
	size = ntohl(*it);
	if(size != 3) goto deserialization_failed;

	// membership
	it++; if(limit == it) goto deserialization_failed;
	membership = ntohl(*it);
	// uniq_membership
	it++; if(limit == it) goto deserialization_failed;
	uniq_membership = ntohl(*it);
	// equivalence
	it++; if(limit == it) goto deserialization_failed;
	equivalence = ntohl(*it);

	return true;

deserialization_failed:
	reset();
	return false;
}}}


memory_statistics::memory_statistics()
{{{
	reset();
}}}
void memory_statistics::reset()
{{{
	bytes = 0;
	members = 0;
	words = 0;
	upper_table = 0;
	lower_table = 0;
	columns = 0;
}}}
basic_string<int32_t> memory_statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += htonl(6);
	ret += htonl(bytes);
	ret += htonl(members);
	ret += htonl(words);
	ret += htonl(upper_table);
	ret += htonl(lower_table);
	ret += htonl(columns);

	return ret;
}}}
bool memory_statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit)
{{{
	int size;

	if(it == limit) goto deserialization_failed;

	// data size
	size = ntohl(*it);
	if(size != 6) goto deserialization_failed;

	// bytes
	it++; if(limit == it) goto deserialization_failed;
	bytes = ntohl(*it);
	// members
	it++; if(limit == it) goto deserialization_failed;
	members = ntohl(*it);
	// words
	it++; if(limit == it) goto deserialization_failed;
	words = ntohl(*it);
	// upper_table
	it++; if(limit == it) goto deserialization_failed;
	upper_table = ntohl(*it);
	// lower_table
	it++; if(limit == it) goto deserialization_failed;
	lower_table = ntohl(*it);
	// columns
	it++; if(limit == it) goto deserialization_failed;
	columns = ntohl(*it);

	return true;

deserialization_failed:
	reset();
	return false;
}}}


timing_statistics::timing_statistics()
{{{
	reset();
}}}
void timing_statistics::reset()
{{{
	user_sec = 0;
	user_usec = 0;
	sys_sec = 0;
	sys_usec = 0;
}}}
basic_string<int32_t> timing_statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += htonl(4);
	ret += htonl(user_sec);
	ret += htonl(user_usec);
	ret += htonl(sys_sec);
	ret += htonl(sys_usec);

	return ret;
}}}
bool timing_statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit)
{{{
	int size;

	if(it == limit) goto deserialization_failed;

	// data size
	size = ntohl(*it);
	if(size != 4) goto deserialization_failed;

	// user_sec
	it++; if(limit == it) goto deserialization_failed;
	user_sec = ntohl(*it);
	// user_usec
	it++; if(limit == it) goto deserialization_failed;
	user_usec = ntohl(*it);
	// sys_sec
	it++; if(limit == it) goto deserialization_failed;
	sys_sec = ntohl(*it);
	// sys_usec
	it++; if(limit == it) goto deserialization_failed;
	sys_usec = ntohl(*it);

	return true;

deserialization_failed:
	reset();
	return false;
}}}


void statistics::reset()
{{{
	queries.reset();
	memory.reset();
	time.reset();
}}}
basic_string<int32_t> statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += 0; // length field, filled in later.

	ret += queries.serialize();
	ret += memory.serialize();
	ret += time.serialize();

	ret[0] = htonl( ret.length() - 1 );
	return ret;
}}}
bool statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit)
{{{
	int size;
	basic_string<int32_t>::iterator end;

	if(it == limit)
		goto deserialization_failed;

	// data size
	size = ntohl(*it);
	it++;
	end = it;
	end += size;

	if(!queries.deserialize(it, limit)) goto deserialization_failed;
	if(!memory.deserialize(it, limit)) goto deserialization_failed;
	if(!time.deserialize(it, limit)) goto deserialization_failed;

	if(it != end) goto deserialization_failed;

	return true;

deserialization_failed:
	reset();
	return false;
}}}

}; // end namespace libalf
