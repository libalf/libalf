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

#include <libalf/statistics.h>

namespace libalf {

template <class answer>
class teacher {
	protected:
		statistics * stats;

	public:
		teacher()
		{{{
			  stats = NULL;
		}}}
		virtual ~teacher() { };

		virtual answer membership_query(list<int> &word) = 0;

//		virtual void membership_query(BDD<int> &tree) = 0;

		virtual void set_statistics_counter(statistics * stats)
		{{{
			this->stats = stats;
		}}}

};

template <class answer>
class teacher_cacheable : public teacher<answer> {
	public:
		teacher_cacheable();
		virtual ~teacher_cacheable();

//		virtual void membership_query(BDD<int> &tree) = 0;
};

}; // end namespace libalf

#endif

