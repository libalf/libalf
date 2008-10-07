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

#include <algorithm>
#include <list>

namespace libalf {

using namespace std;

// alphabet is just <int>
// but there are some special definitions:

// special char for the empty word:
#define ALPHABET_EPSILON 0
// first normal character:
#define ALPHABET_FIRST 1

bool wordcmp(list<int> first, list<int> second)
{{{
	if(first.size() != second.size())
		return false;
	return equal(first.begin(), first.end(), second.begin());
}}}

}

