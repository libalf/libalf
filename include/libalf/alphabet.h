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

// alphabet is just <int> and the first letter is `0'.
// epsilon is denoted by an empty list<int>

// return ptr to new list with first∙second
list<int>* concat(list<int> &first, list<int> &second);

bool is_prefix_of(list<int> &prefix, list<int> &word);

bool is_suffix_of(list<int> &postfix, list<int> &word);

void print_word(ostream &os, list<int> &word);

void print_word(list<int> &word);

}

#endif

