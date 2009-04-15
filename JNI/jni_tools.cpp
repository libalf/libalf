/* $Id$
 * vim: fdm=marker
 *
 * libalf JNI - Java Native Interface for Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *    and Daniel Neider, i7 Informatik RWTH-Aachen
 *        <neider@automata.rwth-aachen.de>
 *
 * see LICENSE file for licensing information.
 */

#include <string>

using namespace std;

int32_t *basic_string2intarray(basic_string<int32_t> str)
{{{
	int32_t *res = new int32_t[str.size()];
	basic_string<int32_t>::iterator si;
	int i;

	for(i = 0, si = str.begin(); si != str.end(); i++, si++)
		res[i] = *si;

	return res;
}}}

