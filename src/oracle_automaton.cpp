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
#include <libalf/oracle_automaton.h>

namespace libalf {

using namespace std;

oracle_automaton::oracle_automaton()
{{{
	backend_automaton = NULL;
}}}

oracle_automaton::oracle_automaton(finite_language_automaton &a)
{{{
	backend_automaton = NULL;
	set_automaton(a);
}}}

oracle_automaton::~oracle_automaton()
{{{
	if(backend_automaton)
		delete backend_automaton;
}}}

void oracle_automaton::set_automaton(finite_language_automaton &a)
{{{
	if(backend_automaton)
		delete backend_automaton;
	backend_automaton = a.clone();
}}}

finite_language_automaton * oracle_automaton::get_automaton()
{{{
	return backend_automaton;
}}}

// returns <true, (void)> if hypothesis is true,
// returns <false, list of counter-examples> if hypothesis is not ok
pair<bool, list< list<int> > > oracle_automaton::equality_query(finite_language_automaton & hypothesis)
{{{
	pair<bool, list< list< int> > > ret;

	if(backend_automaton) {
		finite_language_automaton *difference;
		difference = backend_automaton->lang_difference(hypothesis);
		if(difference->is_empty()) {
			ret.first = true;
			delete difference;
			return ret;
		} else {
			ret.first = false;
			// FIXME: find more than one counter-example and push it back?
			ret.second.push_back(difference->get_sample_word());

			delete difference;
			return ret;
		}
	} else {
		// FIXME: handle as error
		ret.first = false;
		return ret;
	}
}}}


}; // end of namespace libalf

