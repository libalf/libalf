/* $Id$
 * vim: fdm=marker
 *
 * amore++
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __amore_amore_h__
# define __amore_amore_h__

#include <amore++/finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>
#include <amore++/nondeterministic_finite_automaton.h>

// _init() and _fini() may not be exported.

namespace amore {

using namespace std;

const char* libamorepp_version();

}; // end of namespace amore

#endif // __amore_amore_h__

