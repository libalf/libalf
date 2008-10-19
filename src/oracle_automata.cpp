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

#include <libalf/automata.h>
#include <libalf/oracle.h>
#include <libalf/oracle_automata.h>

namespace libalf {

using namespace std;

oracle_automata::oracle_automata()
{{{
	backend_automata = NULL;
}}}

oracle_automata:: oracle_automata(automata &a)
{{{
	  set_automata(a);
}}}

oracle_automata::~oracle_automata()
{{{
	  if(backend_automata)
		  delete backend_automata;
}}}

void oracle_automata::set_automata(automata &a)
{{{
	  if(backend_automata)
		  delete backend_automata;
	  backend_automata = a.clone();
}}}

automata * oracle_automata::get_automata()
{{{
	  return backend_automata;
}}}

// returns <true, (void)> if automata is ok,
// returns <false, list of counter-examples> if automata is not ok
pair<bool, list< list<int> > > oracle_automata::equality_query(automata & hypothesis)
{{{
	pair<bool, list< list< int> > > ret;

	if(backend_automata) {
		automata *difference;
		difference = backend_automata->lang_difference(hypothesis);
		if(difference->is_empty()) {
			ret.first = true;
			return ret;
		} else {
			ret.first = false;
			// FIXME: find more than one counter-example and push it back?
			ret.second.push_back(difference->get_sample_word());

			return ret;
		}
	} else {
		// FIXME: handle as error
		ret.first = false;
		return ret;
	}
}}}


}; // end of namespace libalf

