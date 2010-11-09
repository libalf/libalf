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

#ifndef __libalf_basic_string_h__
#define __libalf_basic_string_h__

#include <ostream>
#include <string>

#include <sys/types.h>
#ifdef _WIN32
# include <stdint.h>
#endif

namespace libalf {

void print_basic_string(std::basic_string<int32_t> str, std::ostream &os);

void print_basic_string_2hl(std::basic_string<int32_t> str, std::ostream &os);

bool basic_string_to_fd(std::basic_string<int32_t> &str, int fd);

bool fd_to_basic_string(const int fd, std::basic_string<int32_t> & str);

bool basic_string_to_file(std::basic_string<int32_t> &str, const char* filename);

bool file_to_basic_string(const char* filename, std::basic_string<int32_t> &str);

// str[0] has to be the size of the string (otherwise operator>> won't work)
// the content of str has to be in network byte order.
std::ostream & operator<<(std::ostream& os, const std::basic_string<int32_t> &str);
// the string in <is> has to be in network byte order, and the first element
// has to be the size of the full string.
std::istream & operator>>(std::istream& is, std::basic_string<int32_t> &str);

}; // end of namespace libalf

#endif // __libalf_basic_string_h__

