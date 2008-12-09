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
#include <istream>

namespace libalf {

using namespace std;

enum logger_loglevel {
	LOGGER_INTERNAL	 = 0,

	LOGGER_ERROR	 = 1,
	LOGGER_WARN	 = 2,
	LOGGER_INFO	 = 3,
	LOGGER_DEBUG	 = 4,

	LOGGER_ALGORITHM = 5
};

class logger : public binary_function< enum logger_loglevel, string&, void > {

	public:
		virtual ~logger() { };

		virtual void operator()(enum logger_loglevel, string&);
		virtual void operator()(enum logger_loglevel, char* format, ...);

	protected:
		virtual void log(enum logger_loglevel l, char* s) = 0;

};

class ostream_logger : public logger {
	private:
		ostream *out;
		enum logger_loglevel minimal_loglevel;
		bool log_algorithm;
		bool use_color;
	public:
		ostream_logger();
		ostream_logger(ostream *out, enum logger_loglevel minimal_loglevel, bool log_algorithm = true, bool use_color = true);

		virtual ~ostream_logger();

	protected:
		virtual void log(enum logger_loglevel l, char* s);
};

}; // end namespace libalf

#endif

