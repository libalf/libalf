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

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <string>
#include <iterator>

#include "libalf/statistics.h"

namespace libalf {

using namespace std;

statistics::statistics()
{{{
	reset();
}}}

void statistics::reset()
{{{
	memory.bytes = 0;
	memory.members = 0;
	memory.words = 0;
	memory.upper_table = 0;
	memory.lower_table = 0;
	memory.columns = 0;

	queries.membership = 0;
	queries.uniq_membership = 0;
	queries.equivalence = 0;

	time.cpu_sec = 0;
	time.cpu_usec = 0;
	time.sys_sec = 0;
	time.sys_usec = 0;
}}}

basic_string<int32_t> statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += 0; // length field, filled in later.

	ret += htonl(memory.bytes);
	ret += htonl(memory.members);
	ret += htonl(memory.words);
	ret += htonl(memory.upper_table);
	ret += htonl(memory.lower_table);
	ret += htonl(memory.columns);

	ret += htonl(queries.membership);
	ret += htonl(queries.uniq_membership);
	ret += htonl(queries.equivalence);

	ret += htonl(time.cpu_sec);
	ret += htonl(time.cpu_usec);
	ret += htonl(time.sys_sec);
	ret += htonl(time.sys_usec);

	ret[0] = htonl( ret.length() - 1 );
	return ret;
}}}

bool statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit)
{{{
	int size;

	if(it == limit)
		goto deserialization_failed;

	// data size
	size = ntohl(*it);

	// memory.bytes
	it++; if(size <= 0 || limit == it) goto deserialization_failed;
	memory.bytes = ntohl(*it);
	// memory.members
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	memory.members = ntohl(*it);
	// memory.words
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	memory.words = ntohl(*it);
	// memory.upper_table
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	memory.upper_table = ntohl(*it);
	// memory.lower_table
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	memory.lower_table = ntohl(*it);
	// memory.columns
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	memory.columns = ntohl(*it);

	// queries.membership
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	queries.membership = ntohl(*it);
	// queries.uniq_membership
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	queries.uniq_membership = ntohl(*it);
	// queries.equivalence
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	queries.equivalence = ntohl(*it);

	// time.cpu_sec
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	time.cpu_sec = ntohl(*it);
	// time.cpu_usec
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	time.cpu_usec = ntohl(*it);
	// time.sys_sec
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	time.sys_sec = ntohl(*it);
	// time.sys_usec
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	time.sys_usec = ntohl(*it);

	it++; size--;

	if(size == 0)
		return true;

deserialization_failed:
	reset();
	return false;
}}}

}; // end namespace libalf
