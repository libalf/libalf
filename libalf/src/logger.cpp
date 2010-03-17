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

#include <string>
#include <ostream>

#include <stdarg.h>

#ifdef _WIN32
#include <stdio.h>
#endif

#include "libalf/logger.h"
#include "libalf/logger_terminalcolors.h"

namespace libalf {

using namespace std;

logger::logger()
{{{
	minimal_loglevel = LOGGER_INFO;
	log_algorithm = true;
}}}

void logger::operator()(enum logger_loglevel l, string &s)
{{{
	if( (l<=minimal_loglevel) || (log_algorithm && l==LOGGER_ALGORITHM)) {
		log(l, (char*)s.c_str());
	}
}}}

#define BUFSIZE 4096

void logger::operator()(enum logger_loglevel l, const char * format, ...)
{{{
	if( (l<=minimal_loglevel) || (log_algorithm && l==LOGGER_ALGORITHM)) {
		va_list ap;
		char buf[BUFSIZE];
		int a;

		va_start(ap, format);
		a = vsnprintf(buf, BUFSIZE, format, ap);
		va_end(ap);
		buf[BUFSIZE-1] = 0;

		log(l, buf);
		if(a >= BUFSIZE)
			log(LOGGER_ERROR, "NOTE: the last logging message was oversized and thus truncated.\n");
	}
}}}

void logger::set_minimal_loglevel(enum logger_loglevel minimal_loglevel)
{{{
	this->minimal_loglevel = minimal_loglevel;
}}}

void logger::set_log_algorithm(bool log_algorithm)
{{{
	this->log_algorithm = log_algorithm;
}}}



ignore_logger::~ignore_logger()
{ /* nothing */ }
void ignore_logger::log(enum logger_loglevel __attribute__ ((__unused__)) l, const char __attribute__ ((__unused__)) * s)
{ /* nothing */ }



ostream_logger::ostream_logger()
{{{
	use_color = true;
	out = NULL;
}}}

ostream_logger::ostream_logger(ostream *out, enum logger_loglevel minimal_loglevel, bool log_algorithm, bool use_color)
{{{
	this->out = out;
	this->minimal_loglevel = minimal_loglevel;
	this->log_algorithm = log_algorithm;
	this->use_color = use_color;
	log(LOGGER_INTERNAL, "started logger instance\n");
}}}

ostream_logger::~ostream_logger()
{{{
	log(LOGGER_INTERNAL, "stopped logger instance\n");
}}}

void ostream_logger::log(enum logger_loglevel l, const char* s)
{{{
	//				internal, error, warn,   info,   debug, algorithm
	static const char *colors[] = { C_OK,     C_ERR, C_WARN, C_BOLD, "",    C_ATT };

	if(out) {
		if(use_color)
			(*out) << colors[l];
		(*out) << s;
		if(use_color)
			(*out) << C_RESET;
		(*out) << flush;
	}
}}}




buffered_logger::buffered_logger()
{{{
	buffer = new string;
	log(LOGGER_INTERNAL, "started logger instance\n");
}}}

buffered_logger::buffered_logger(enum logger_loglevel minimal_loglevel, bool log_algorithm)
{{{
	this->minimal_loglevel = minimal_loglevel;
	this->log_algorithm = log_algorithm;

	buffer = new string;

	log(LOGGER_INTERNAL, "started logger instance\n");
}}}

buffered_logger::~buffered_logger()
{{{
	// no need to log this.
	delete buffer;
}}}

string * buffered_logger::receive_and_flush()
{{{
	string * tmp;

	tmp = buffer;
	buffer = new string;

	return tmp;
}}}

void buffered_logger::log(enum logger_loglevel __attribute__ ((__unused__)) l, const char* s)
{{{
	buffer->append(s);
}}}

} // end namespace libalf

