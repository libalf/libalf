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

#include <string>
#include <ostream>

#include <stdarg.h>

#include "libalf/logger.h"
#include "libalf/logger_terminalcolors.h"

namespace libalf {

using namespace std;

logger::logger()
{{{
	minimal_loglevel = LOGGER_ERROR;
	log_algorithm = true;
}}}

void logger::operator()(enum logger_loglevel l, string &s)
{{{
	if( (l<=minimal_loglevel) || (log_algorithm && l==LOGGER_ALGORITHM)) {
		log(l, (char*)s.c_str());
	}
}}}

void logger::operator()(enum logger_loglevel l, char * format, ...)
{{{
	if( (l<=minimal_loglevel) || (log_algorithm && l==LOGGER_ALGORITHM)) {
		va_list ap;
		char buf[1024];

		va_start(ap, format);
		vsnprintf(buf, 1024, format, ap);
		va_end(ap);

		log(l, buf);
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



ostream_logger::ostream_logger()
{{{
	minimal_loglevel = LOGGER_ERROR;
	log_algorithm = true;
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

void ostream_logger::log(enum logger_loglevel l, char* s)
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
	minimal_loglevel = LOGGER_ERROR;
	log_algorithm = true;

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

	return buffer;
}}}

void buffered_logger::log(enum logger_loglevel l, char* s)
{{{
printf("log '%s'\n", s);
	buffer->append(s);
printf("now in log: '%s'\n", buffer->c_str());
}}}

} // end namespace libalf

