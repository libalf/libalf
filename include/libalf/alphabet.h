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

// special char for the empty word (should always be alone in a list<int>
#define ALPHABET_EPSILON 0
// first normal character:
#define ALPHABET_FIRST 1

// part of STL:
//bool operator==(list<int> first, list<int> second)
//{{{
//	return (
//		first.size() == second.size()
//		&&
//		equal(first.begin(), first.end(), second.begin());
//	       );
//}}}

list<int> operator+ (list<int> first, list<int> second)
{{{
	list<int> *l = new list<int>;
	list<int>::iterator li;

	l.assign(first.begin(), first.end());
	for(li = second.begin(); li != second.end(); li++)
		l.pushback(*li);

	return l;
}}}

// FIXME: check below; optimize
bool is_prefix_of(list<int> prefix, list<int> word)
{{{
	if(prefix.size() > word.size())
		return false;
	list<int>::iterator pi, wi;
	for(pi = prefix.begin(), li = word.begin(); pi != pi.end(); pi++, li++)
		if(*pi != *li)
			return false;
	return true;
}}}

bool is_suffix_of(list<int> postfix, list<int> word)
{{{
	if(postfix.size() > word.size())
		return false;
	list<int>::iterator pi, wi;

	li = word.begin();
	li += li.size() - postfix.size();
	for(pi = postfix.begin(); pi != pi.end(); pi++)
		if(*pi != *li)
			return false;
	return true;
}}}

}

