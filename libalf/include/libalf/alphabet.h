/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_alphabet_h__
# define __libalf_alphabet_h__

#include <algorithm>
#include <list>
#include <string>

#ifdef _WIN32
# include <stdint.h>
#endif

namespace libalf {

using namespace std;

#define BOTTOM_CHAR -42

// alphabet is just <int> and the first letter is `0'.
// epsilon is denoted by an empty list<int>

// return ptr to new list with first∙second
list<int>* concat(list<int> &first, list<int> &second);

bool is_prefix_of(list<int> &prefix, list<int> &word);

bool is_suffix_of(list<int> &postfix, list<int> &word);

void print_word(ostream &os, list<int> &word);

void print_word(list<int> &word);

string word2string(list<int> &word, char separator);

inline string word2string(list<int> &word)
{ return word2string(word, '.'); };

basic_string<int32_t> serialize_word(list<int> &word);

bool deserialize_word(list<int32_t> &into, basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

// lexicographic compare:
// a < b iff a[i] == b[i] \forall i < j and a[j] < b[j]
bool is_lex_smaller(list<int> &a, list<int> &b);

// graded lexicographic compare:
// a < b iff |a|<|b| or ( |a|==|b| and lex. a < b )
bool is_graded_lex_smaller(list<int> &a, list<int> &b);

// get next bigger/smaller word w.r.t. graded lexicographic order
void inc_graded_lex(list<int> &word, int alphabet_size);
void dec_graded_lex(list<int> &word, int alphabet_size);

};

#endif

