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

#include <arpa/inet.h>

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
	table_size.bytes = 0;
	table_size.members = 0;
	table_size.words = 0;

	table_size.upper_table = 0;
	table_size.lower_table = 0;
	table_size.columns = 0;

	query_count.membership = 0;
	query_count.equivalence = 0;
}}}

basic_string<int32_t> statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += 0; // length field, filled in later.

	ret += htonl(table_size.bytes);
	ret += htonl(table_size.members);
	ret += htonl(table_size.words);
	ret += htonl(table_size.upper_table);
	ret += htonl(table_size.lower_table);
	ret += htonl(table_size.columns);

	ret += htonl(query_count.membership);
	ret += htonl(query_count.equivalence);

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

	// table_size.bytes
	it++; if(size <= 0 || limit == it) goto deserialization_failed;
	table_size.bytes = ntohl(*it);
	// table_size.members
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	table_size.members = ntohl(*it);
	// table_size.words
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	table_size.words = ntohl(*it);
	// table_size.upper_table
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	table_size.upper_table = ntohl(*it);
	// table_size.lower_table
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	table_size.lower_table = ntohl(*it);
	// table_size.columns
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	table_size.columns = ntohl(*it);

	// query_count.membership
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	query_count.membership = ntohl(*it);
	// query_count.equivalence
	it++; size--; if(size <= 0 || limit == it) goto deserialization_failed;
	query_count.equivalence = ntohl(*it);

	it++; size--;

	if(size == 0)
		return true;

deserialization_failed:
	reset();
	return false;
}}}

}; // end namespace libalf
