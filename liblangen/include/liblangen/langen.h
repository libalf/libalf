/* $Id$
 * vim: fdm=marker
 *
 * liblangen (LANguageGENerator)
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __liblangen_langen_h__
# define __liblangen_langen_h__

#include <liblangen/prng.h>
#include <liblangen/dfa_enumerator.h>
#include <liblangen/dfa_randomgenerator.h>
#include <liblangen/nfa_randomgenerator.h>
#include <liblangen/regex_randomgenerator.h>

namespace liblangen {

using namespace std;

const char* liblangen_version();

}; // end of namespace liblangen

#endif // __liblangen_langen_h__

