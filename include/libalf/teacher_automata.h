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

namespace libalf {

// teacher automata
//
// simple teacher that uses an automata as backend for all its tests
//
template <class answer>
class teacher_automata : public teacher {
	private:
		automata *automata;

	public:
		teacher_automata()
		{{{
			automata = NULL;
		}}}

		teacher_automata(automata &a)
		{{{
			set_automata(a);
		}}}

		virtual ~teacher_automata()
		{{{
			  if(automata)
				  delete automata;
		}}}

		virtual void set_automata(automata &a)
		{{{
			  if(automata)
				  delete automata;
			  automata = a.clone;
		}}}

		virtual automata *get_automata()
		{{{
			  return automata;
		}}}

		virtual answer membership_query(list<int> &word)
		// FIXME: problem: answer vs. bool
		// this implementation requires that a cast from bool to <answer> is possible
		{{{
			/*
			if(automata->contains(word))
				return answer::true;
			else
				return answer::false;
			*/
			return automata->contains(word);
		}}}

};

}; // end namespace libalf

#endif

