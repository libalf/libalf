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

#ifndef __libalf_teacher_h__
# define __libalf_teacher_h__

#include <list>

namespace libalf {

template <class answer>
class teacher {

	public:
		virtual answer membership_query(list<int> &word) = 0;

//		virtual void membership_query(BDD<int> &tree) = 0;

};

}; // end namespace libalf

#endif

