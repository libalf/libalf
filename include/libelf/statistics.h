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

namespace libalf {

class statistics {

	struct {
		int bytes;
		int members;
		int words;
	} table_size;

	struct {
		int membership;
		int equality;
	} query_count;

};

}; // end namespace libalf

