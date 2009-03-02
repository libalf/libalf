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
	protected:
		enum logger_loglevel minimal_loglevel;
		bool log_algorithm;

	public:
		logger();
		virtual ~logger() { };

		virtual void operator()(enum logger_loglevel, string&);
		virtual void operator()(enum logger_loglevel, const char* format, ...);

		void set_minimal_loglevel(enum logger_loglevel minimal_loglevel);
		void set_log_algorithm(bool log_algorithm);

	protected:
		virtual void log(enum logger_loglevel l, char* s) = 0;

};

class ignore_logger : public logger {
	protected:
		virtual void log(enum logger_loglevel l, char* s) { };
};

class ostream_logger : public logger {
	private:
		ostream *out;
		bool use_color;
	public:
		ostream_logger();
		ostream_logger(ostream *out, enum logger_loglevel minimal_loglevel, bool log_algorithm = true, bool use_color = true);

		virtual ~ostream_logger();

	protected:
		virtual void log(enum logger_loglevel l, char* s);
};

class buffered_logger : public logger {
	private:
		string * buffer;
	public:
		buffered_logger();
		buffered_logger(enum logger_loglevel minimal_loglevel, bool log_algorithm = true);

		virtual ~buffered_logger();

		string * receive_and_flush();

	protected:
		virtual void log(enum logger_loglevel l, char* s);
};

}; // end namespace libalf

#endif

