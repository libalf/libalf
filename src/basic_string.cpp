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
#include <fstream>
#include <string>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "libalf/basic_string.h"

namespace libalf {

using namespace std;

void print_basic_string(basic_string<int32_t> str, ostream &os)
{{{
	basic_string<int32_t>::iterator si;

	os << "_";

	for(si = str.begin(); si != str.end(); si++) {
		os << *si;
		os << "_";
	}
}}}

void print_basic_string_2hl(basic_string<int32_t> str, ostream &os)
{{{
	basic_string<int32_t>::iterator si;

	os << "_";

	for(si = str.begin(); si != str.end(); si++) {
		os << ntohl(*si);
		os << "_";
	}
}}}

bool basic_string_to_file(basic_string<int32_t> &str, const char* filename)
{{{
	int fd;
	basic_string<int32_t>::iterator si;

	fd = open(filename, O_WRONLY | O_CREAT);

	if(fd < 0)
		return false;

	for(si = str.begin(); si != str.end(); si++) {
		int32_t d = *si;
		if(write(fd, &d, sizeof(d)) != sizeof(d))
			return false;
	}

	return (close(fd) >= 0);
}}}

bool file_to_basic_string(const char* filename, basic_string<int32_t> &str)
{{{
	int fd;
	basic_string<int32_t>::iterator si;
	str.clear();

	fd = open(filename, O_RDONLY);

	if(fd < 0)
		return false;

	int32_t d;
	int s;
	while((s = read(fd, &d, sizeof(d))) == sizeof(d)) {
		str.push_back(d);
	}
	if(s > 0)
		return false;

	return (close(fd) >= 0);
}}}

}; // end of namespace libalf

