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

using namespace libalf;

class statistics {
	public:
		struct {
			int bytes;
			int members;
			int words;
		} table_size;

		struct {
			int membership;
			int equivalence;
		} query_count;

		// rechenzeit
		// ...

		// speicherverbrauch
		// ...


		statistics();
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator final);

};

}; // end namespace libalf

#endif

