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

#include <ostream>
#include <string>
#include <arpa/inet.h>

#include "libalf/basic_string.h"

namespace libalf {

using namespace std;

void print_basic_string(basic_string<int32_t> str, ostream &os)
{
	basic_string<int32_t>::iterator si;

	os << "_";

	for(si = str.begin(); si != str.end(); si++) {
		os << *si;
		os << "_";
	}
}

void print_basic_string_2hl(basic_string<int32_t> str, ostream &os)
{
	basic_string<int32_t>::iterator si;

	os << "_";

	for(si = str.begin(); si != str.end(); si++) {
		os << ntohl(*si);
		os << "_";
	}
}

}; // end of namespace libalf

