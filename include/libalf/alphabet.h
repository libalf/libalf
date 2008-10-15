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

#ifndef __libalf_alphabet_h__
# define __libalf_alphabet_h__

#include <algorithm>
#include <list>

namespace libalf {

using namespace std;

// alphabet is just <int>
// but there are some special definitions:

// special char for the empty word (should always be alone in a list<int>
#define ALPHABET_EPSILON 0
// first normal character:
#define ALPHABET_FIRST 1

// return ptr to new list with first∙second
list<int>* operator+ (list<int> first, list<int> second);

bool is_prefix_of(list<int> prefix, list<int> word);

bool is_suffix_of(list<int> postfix, list<int> word);

}

#endif

