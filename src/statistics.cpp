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

#include <libalf/statistics.h>

namespace libalf {

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

}; // end namespace libalf
