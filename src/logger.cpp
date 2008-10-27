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

#include "libalf/logger.h"

namespace libalf {

using namespace std;

ostream_logger::ostream_logger()
{{{
	minimal_loglevel = LOGGER_ERROR;
	log_algorithm = false;
	this->log(LOGGER_NONE, "started logger instance");
}}}

ostream_logger::ostream_logger(ostream *out, enum logger_loglevel minimal_loglevel, bool log_algorithm)
{{{
	this->out = out;
	this->minimal_loglevel = minimal_loglevel;
	this->log_algorithm = log_algorithm;
	log(LOGGER_NONE, "started logger instance");
}}}

ostream_logger::~ostream_logger()
{{{
	log(LOGGER_NONE, "stopped logger instance");
}}}

void ostream_logger::operator()(enum logger_loglevel l, string &s)
{{{
	log(l, (char*)s.c_str());
}}}

void ostream_logger::operator()(enum logger_loglevel l, char*s)
{{{
	log(l, s);
}}}

void ostream_logger::log(enum logger_loglevel l, char* s)
{{{
	if( (l<=minimal_loglevel) || (log_algorithm && l==LOGGER_ALGORITHM) || (l==LOGGER_NONE))
		(*out) << s;
}}}


} // end namespace libalf
