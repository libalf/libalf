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

#ifndef __libalf_basic_string_h__
#define __libalf_basic_string_h__

#include <ostream>
#include <string>

namespace libalf {

using namespace std;

void print_basic_string(basic_string<int32_t> str, ostream &os);

void print_basic_string_2hl(basic_string<int32_t> str, ostream &os);

bool basic_string_to_file(basic_string<int32_t> &str, const char* filename);

bool file_to_basic_string(const char* filename, basic_string<int32_t> &str);

}; // end of namespace libalf

#endif // __libalf_basic_string_h__

