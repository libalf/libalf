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

	ret += htonl(query_count.membership);
	ret += htonl(query_count.equivalence);

	ret[0] = htonl( ret.length() - 1 );
	return ret;
}}}

bool statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator final)
{
	// FIXME: deserialize is not implemented
	return false;
}

}; // end namespace libalf
