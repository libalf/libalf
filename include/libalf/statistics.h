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

namespace libalf {

class statistics {
	public:
		struct {
			int bytes;
			int members;
			int words;
		} table_size;

		struct {
			int membership;
			int equality;
		} query_count;

		// rechenzeit

		// speicherverbrauch
};

}; // end namespace libalf

#endif

