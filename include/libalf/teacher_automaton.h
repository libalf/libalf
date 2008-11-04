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

#ifndef __libalf_teacher_automaton_h__
# define __libalf_teacher_automaton_h__

#include <list>

#include "libalf/teacher.h"

namespace libalf {

// teacher automaton
//
// simple teacher that uses an automaton as backend for all its tests
//
template <class answer>
class teacher_automaton : public teacher<answer> {
	private:
		finite_language_automaton *atm;

	public:
		teacher_automaton()
		{{{
			atm = NULL;
		}}}

		teacher_automaton(finite_language_automaton *a)
		{{{
			atm = NULL;
			set_automaton(a);
		}}}

		virtual ~teacher_automaton()
		{{{
			if(atm)
				delete atm;
		}}}

		virtual void set_automaton(finite_language_automaton *a)
		{{{
			if(atm)
				delete atm;
			atm = a->clone();
		}}}

		virtual finite_language_automaton *get_automaton()
		{{{
			return atm;
		}}}

		virtual answer membership_query(list<int> &word)
		{{{
			if(this->stats)
				this->stats->query_count.membership++;

			answer r;
			r = atm->contains(word);
			return r;
		}}}

};

}; // end namespace libalf

#endif

