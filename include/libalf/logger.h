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

namespace libalf {

using namespace std;

enum logger_loglevel {
	LOGGER_ERROR	 = 0,
	LOGGER_WARN	 = 1,
	LOGGER_INFO	 = 2,
	LOGGER_DEBUG	 = 3,

	LOGGER_ALGORITHM = 10,

	LOGGER_NONE	 = 99
};

class logger : public binary_function< enum logger_loglevel, string&, void > {

	public:
		virtual void operator()(enum logger_loglevel, string&) = 0;

};

class stdout_logger : public logger {
	private:
		enum logger_loglevel minimal_loglevel;
		bool log_algorithm;
	public:
		stdout_logger()
		{{{
			minimal_loglevel = LOGGER_ERROR;
			log_algorithm = false;
			this->log(LOGGER_NONE, "started logger instance");
		}}}

		stdout_logger(enum logger_loglevel minimal_loglevel, bool log_algorithm)
		{{{
			this->minimal_loglevel = minimal_loglevel;
			this->log_algorithm = log_algorithm;
			log(LOGGER_NONE, "started logger instance");
		}}}

		virtual void operator()(enum logger_loglevel l, string &s)
		{{{
			log(l, (char*)s.c_str());
		}}}

	protected:
		virtual void log(enum logger_loglevel l, char* s)
		{{{
			if( (l<=minimal_loglevel) || (log_algorithm && l==LOGGER_ALGORITHM) || (l==LOGGER_NONE))
				cout << s;
		}}}
};

} // end namespace libalf

