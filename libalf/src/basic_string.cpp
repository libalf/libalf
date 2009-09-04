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

#ifdef _WIN32
#include <winsock.h>
#include <stdint.h>
#include <stdio.h>
#else
#include <arpa/inet.h>
#endif

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

bool basic_string_to_fd(basic_string<int32_t> &str, int fd)
{{{
	basic_string<int32_t>::iterator si;

	if(fd < 0)
		return false;

	for(si = str.begin(); si != str.end(); si++) {
		if(write(fd, &*si, sizeof(int32_t)) != sizeof(int32_t))
			return false;
	}

	return true;
}}}

bool fd_to_basic_string(const int fd, basic_string<int32_t> & str)
{{{
	if(fd < 0)
		return false;

	int32_t d;
	int s;
	while((s = read(fd, &d, sizeof(d))) == sizeof(d)) {
		str.push_back(d);
	}
	if(s > 0)
		return false;

	return true;
}}}

bool basic_string_to_file(basic_string<int32_t> &str, const char* filename)
{{{
	int fd;

	fd = open(filename, O_WRONLY | O_CREAT, 0644);

	bool ok = basic_string_to_fd(str, fd);

	if(close(fd) >= 0)
		return ok;
	else
		return false;
}}}

bool file_to_basic_string(const char* filename, basic_string<int32_t> &str)
{{{
	int fd;
	str.clear();

	fd = open(filename, O_RDONLY);

	bool ok = fd_to_basic_string(fd, str);

	if(close(fd) >= 0)
		return ok;
	else
		return false;
}}}

}; // end of namespace libalf

