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

#ifndef __libalf_oracle_automata_h__
# define __libalf_oracle_automata_h__

#include <list>

#include <libalf/automata.h>
#include <libalf/oracle.h>

namespace libalf {

using namespace std;

// oracle automata
//
// simple oracle that uses an automata as backend for all its tests
//
class oracle_automata : public oracle {
	private:
		automata *automata;
	public:
		oracle_automata()
		{{{
			automata = NULL;
		}}}

		oracle_automata(automata &a)
		{{{
			  set_automata(a);
		}}}

		virtual ~oracle_automata()
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

		// returns <true, (void)> if automata is ok,
		// returns <false, list of counter-examples> if automata is not ok
		virtual pair<bool, list< list<int> > > equality_query(automata&)
		// FIXME
		{{{
		}}}
};

}; // end namespace libalf

#endif

