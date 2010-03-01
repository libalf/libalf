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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <algorithm>
#include <list>
#include <ostream>
#include <iterator>

#ifdef _WIN32
# include <winsock.h>
# include <stdio.h>
#else
# include <arpa/inet.h>
#endif

#include "libalf/alphabet.h"
#include "libalf/alf.h"

namespace libalf {

using namespace std;

// return ptr to new list with first∙second
list<int>* concat(list<int> &first, list<int> &second)
{{{
	list<int> *l = new list<int>;
	list<int>::iterator li;

	if(first.size() != 0)
		l->assign(first.begin(), first.end());
	if(second.size() != 0)
		for(li = second.begin(); li != second.end(); ++li)
			l->push_back(*li);

	return l;
}}}

list<int> operator+(const list<int> & prefix, const list<int> & suffix)
{{{
	list<int> ret;
	list<int>::const_iterator li;

	ret = prefix;
	for(li = suffix.begin(); li != suffix.end(); ++li)
		ret.push_back(*li);

	return ret;
}}}

bool is_prefix_of(list<int> &prefix, list<int> &word)
{{{
	unsigned int prs, ws;
	prs = prefix.size();
	if(prs == 0)
		return true;
	ws = word.size();
	if(prs > ws)
		return false;
	list<int>::iterator pi, li;
	for(pi = prefix.begin(), li = word.begin(); pi != prefix.end(); pi++, li++)
		if(*pi != *li)
			return false;
	return true;
}}}

bool is_suffix_of(list<int> &suffix, list<int> &word)
{{{
	unsigned int sus,ws;
	sus = suffix.size();
	if(sus == 0)
		return true;
	ws = word.size();
	if(sus > ws)
		return false;
	list<int>::iterator pi, li;

	li = word.begin();
	for(unsigned int i = 0; i < ws - sus; i++)
		li++;
	for(pi = suffix.begin(); pi != suffix.end(); pi++)
		if(*pi != *li)
			return false;
	return true;
}}}

void print_word(ostream &os, list<int> &word)
{{{
	ostream_iterator<int> out(os, ".");
	os << ".";
	copy(word.begin(), word.end(), out);
}}}

void print_word(list<int> &word)
{{{
	printf(".");
	for(list<int>::iterator l = word.begin(); l != word.end(); l++)
		printf("%d.", *l);
}}}

string word2string(list<int> &word, char separator = '.')
{{{
	string ret;
	char buf[32];

	ret += separator;

	for(list<int>::iterator wi = word.begin(); wi != word.end(); wi++) {
		snprintf(buf, 32, "%d%c", *wi, separator);
		buf[31] = 0;
		ret += buf;
	}

	return ret;
}}}

basic_string<int32_t> serialize_word(list<int> &word)
{{{
	basic_string<int32_t> ret;

	ret += htonl(word.size());

	for(list<int>::iterator l = word.begin(); l != word.end(); l++)
		ret += htonl(*l);

	return ret;
}}}

bool deserialize_word(list<int32_t> &into, basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{{{
	int length;

	into.clear();

	if(it == limit) return false;

	length = ntohl(*it);
	it++;

	if(it == limit) return (length == 0);

	for(/* -- */; it != limit && length > 0; length--, it++)
		into.push_back(ntohl(*it));

	if(length) {
		into.clear();
		return false;
	}

	return true;
}}}

bool is_lex_smaller(list<int> &a, list<int> &b)
{{{
	list<int>::iterator w1i;
	list<int>::iterator w2i;

	for(w1i = a.begin(), w2i = b.begin(); w1i != a.end() && w2i != b.end(); w1i++, w2i++)
		if(*w1i != *w2i)
			return (*w1i < *w2i);

	return w1i == a.end() && w2i != b.end();
}}}

bool is_graded_lex_smaller(list<int> &a, list<int> &b)
{{{
	list<int>::iterator w1i;
	list<int>::iterator w2i;
	int cmp = 0;

	for(w1i = a.begin(), w2i = b.begin(); w1i != a.end() && w2i != b.end(); w1i++, w2i++)
		if(cmp == 0) {
			if(*w1i < *w2i)
				cmp = -1;
			else
				if(*w1i > *w2i)
					cmp = 1;
		}

	if(w1i == a.end() && w2i == b.end())
		return (cmp == -1);
	else
		return (w1i == a.end());
}}}

void inc_graded_lex(list<int> &word, int alphabet_size)
{{{
	list<int>::iterator wi;
	wi = word.end();
	wi--;

	while(wi != word.end()) {
		(*wi)++;
		if(*wi >= alphabet_size)
			*wi = 0;
		else
			break;
		wi--;
	}
	if(wi == word.end()) {
		int s = word.size() + 1;
		word.clear();
		for(int i = 0; i < s; i++)
			word.push_back(0);
	}
}}}

void dec_graded_lex(list<int> &word, int alphabet_size)
{{{
	list<int>::iterator wi;
	wi = word.end();
	wi--;

	while(wi != word.end()) {
		(*wi)--;
		if(*wi < 0)
			*wi = alphabet_size - 1;
		else
			break;
		wi--;
	}
	if(wi == word.end()) {
		int s = word.size() - 1;
		word.clear();
		for(int i = 0; i < s; i++)
			word.push_back(alphabet_size - 1);
	}
}}}

}

