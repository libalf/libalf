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

#ifndef __libalf_teacher_network_h__
# define __libalf_teacher_network_h__

#include <list>

#include <libalf/teacher.h>

namespace libalf {

using namespace std;

template <class answer>
class teacher_network : public teacher<answer>, public network_connection {
	public:
		teacher_network();
		virtual ~teacher_network();

		virtual answer membership_query(list<int> &word);
};

}; // end namespace libalf

#endif // __libalf_teacher_network_h__

