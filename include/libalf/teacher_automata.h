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

#ifndef __libalf_teacher_automata_h__
# define __libalf_teacher_automata_h__

#include <list>

#include "libalf/teacher.h"

namespace libalf {

// teacher automata
//
// simple teacher that uses an automata as backend for all its tests
//
template <class answer>
class teacher_automata : public teacher<answer> {
	private:
		automata *atm;

	public:
		teacher_automata()
		{{{
			atm = NULL;
		}}}

		teacher_automata(automata &a)
		{{{
			set_automata(a);
		}}}

		virtual ~teacher_automata()
		{{{
			  if(atm)
				  delete atm;
		}}}

		virtual void set_automata(automata &a)
		{{{
			  if(atm)
				  delete atm;
			  atm = a.clone();
		}}}

		virtual automata *get_automata()
		{{{
			  return atm;
		}}}

		virtual answer membership_query(list<int> &word)
		// FIXME: problem: answer vs. bool
		// this implementation requires that a cast from bool to <answer> is possible
		{{{
			return atm->contains(word);
		}}}

};

}; // end namespace libalf

#endif

