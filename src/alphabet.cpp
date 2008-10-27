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

#include "libalf/alphabet.h"
#include "libalf/alf.h"

namespace libalf {

using namespace std;

// part of STL:
//bool operator==(list<int> first, list<int> second)
//{{{
//	return (
//		first.size() == second.size()
//		&&
//		equal(first.begin(), first.end(), second.begin());
//	       );
//}}}

// return ptr to new list with first∙second
list<int>* operator+ (list<int> first, list<int> second)
{{{
	list<int> *l = new list<int>;
	list<int>::iterator li;

	l->assign(first.begin(), first.end());
	for(li = second.begin(); li != second.end(); li++)
		l->push_back(*li);

	return l;
}}}

// FIXME: check below; optimize
bool is_prefix_of(list<int> prefix, list<int> word)
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

bool is_suffix_of(list<int> suffix, list<int> word)
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

}

