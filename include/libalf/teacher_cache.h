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

#ifndef __libalf_teacher_cache_h__
# define __libalf_teacher_cache_h__

#include <list>

#include <libalf/teacher.h>

namespace libalf {

template <class answer>
class teacher_cache : public teacher_cacheable {
	protected:
		teacher_cacheable *teach;

	public:
		teacher_cache()
		{{{
			teach = NULL;
		}}}

		virtual ~teacher_cache() { };

		virtual void set_cached_teacher(teacher_cacheable * teach)
		{{{
			this->teach = teach;
		}}}

		virtual answer membership_query(list<int> &word)
		{
			answer r;

			// check cache

			r = teach->membership_query(word);
			// store result in cache
		}

}

}; // end namespace libalf

#endif

