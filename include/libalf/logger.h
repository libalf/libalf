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

#ifndef __libalf_logger_h__
# define __libalf_logger_h__

#include <string>

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
		stdout_logger();

		stdout_logger(enum logger_loglevel minimal_loglevel, bool log_algorithm);

		~stdout_logger();

		virtual void operator()(enum logger_loglevel l, string &s);

	protected:
		virtual void log(enum logger_loglevel l, char* s);
};

} // end namespace libalf

#endif

