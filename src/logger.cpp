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

#include <libalf/logger.h>
#include <libalf/logger_terminalcolors.h>

namespace libalf {

using namespace std;

void logger::operator()(enum logger_loglevel l, string &s)
{{{
	log(l, (char*)s.c_str());
}}}

void logger::operator()(enum logger_loglevel l, char * format, ...)
{{{
	va_list ap;
	char buf[1024];

	va_start(ap, format);
	vsnprintf(buf, 1024, format, ap);
	va_end(ap);

	log(l, buf);
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
	log(LOGGER_INTERNAL, "\nstarted logger instance\n");
}}}

ostream_logger::~ostream_logger()
{{{
	log(LOGGER_INTERNAL, "stopped logger instance\n\n");
}}}

void ostream_logger::log(enum logger_loglevel l, char* s)
{{{
	//				internal, error, warn,   info,   debug, algorithm
	static const char *colors[] = { C_OK,     C_ERR, C_WARN, C_BOLD, "",    C_ATT };

	if(out)
		if( (l<=minimal_loglevel) || (log_algorithm && l==LOGGER_ALGORITHM)) {
			if(use_color)
				(*out) << colors[l];
			(*out) << s;
			if(use_color)
				(*out) << C_RESET;
			(*out) << flush;
		}
}}}


} // end namespace libalf
