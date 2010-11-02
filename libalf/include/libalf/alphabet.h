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
 * (c) 2008,2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *                and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *	   Daniel Neider <neider@automata.rwth-aachen.de
 *
 */

#ifndef __libalf_alphabet_h__
# define __libalf_alphabet_h__

#include <list>
#include <string>
#include <iostream>

#ifdef _WIN32
# include <stdint.h>
#endif

namespace libalf {

#define BOTTOM_CHAR -42

// alphabet is just <int> and the first letter is `0'.
// epsilon is denoted by an empty std::list<int>

// return ptr to new std::list with first∙second
std::list<int>* concat(const std::list<int> &first, const std::list<int> &second);

std::list<int> operator+(const std::list<int> & prefix, const std::list<int> & suffix);

// simple constructors for (const-len) words.
// call:
//	my_word = word(4,  0,1,2,3);
//                     ^ number of letters
std::list<int> word(const int num_letters, ...);

bool is_prefix_of(const std::list<int> &prefix, const std::list<int> &word);

bool is_suffix_of(const std::list<int> &postfix, const std::list<int> &word);

void print_word(std::ostream &os, const std::list<int> &word);

void print_word(const std::list<int> &word);

std::string word2string(const std::list<int> &word, char separator);

inline std::string word2string(const std::list<int> &word)
{ return word2string(word, '.'); };

std::basic_string<int32_t> serialize_word(const std::list<int> &word);

bool deserialize_word(std::list<int32_t> &into, std::basic_string<int32_t>::const_iterator &it, std::basic_string<int32_t>::const_iterator limit);

// lexicographic compare:
// a < b iff a[i] == b[i] \forall i < j and a[j] < b[j]
bool is_lex_smaller(const std::list<int> &a, const std::list<int> &b);

// graded lexicographic compare:
// a < b iff |a|<|b| or ( |a|==|b| and lex. a < b )
bool is_graded_lex_smaller(const std::list<int> &a, const std::list<int> &b);

// get next bigger/smaller word w.r.t. graded lexicographic order
void inc_graded_lex(std::list<int> &word, int alphabet_size);
void dec_graded_lex(std::list<int> &word, int alphabet_size);

};

#endif

