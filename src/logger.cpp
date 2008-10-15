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

#include <functional>
#include <string>
#include <iostream>

#include "libalf/logger.h"

namespace libalf {

using namespace std;

stdout_logger::stdout_logger()
{{{
	minimal_loglevel = LOGGER_ERROR;
	log_algorithm = false;
	this->log(LOGGER_NONE, "started logger instance");
}}}

stdout_logger::stdout_logger(enum logger_loglevel minimal_loglevel, bool log_algorithm)
{{{
	this->minimal_loglevel = minimal_loglevel;
	this->log_algorithm = log_algorithm;
	log(LOGGER_NONE, "started logger instance");
}}}

stdout_logger::~stdout_logger()
{{{
	log(LOGGER_NONE, "stopped logger instance");
}}}

void stdout_logger::operator()(enum logger_loglevel l, string &s)
{{{
	log(l, (char*)s.c_str());
}}}

void stdout_logger::log(enum logger_loglevel l, char* s)
{{{
	if( (l<=minimal_loglevel) || (log_algorithm && l==LOGGER_ALGORITHM) || (l==LOGGER_NONE))
		cout << s;
}}}


} // end namespace libalf
