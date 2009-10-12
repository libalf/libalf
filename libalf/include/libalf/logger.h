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
 * (c) 2008,2009 by David R. Piegdon, Chair of Computer Science 2 and 7, RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_logger_h__
# define __libalf_logger_h__

#include <string>
#include <istream>

namespace libalf {

using namespace std;

enum logger_loglevel {
	LOGGER_INTERNAL	 = 0,		// only internal, do not use

	LOGGER_ERROR	 = 1,		// all log messages that describe a non-recoverable error are marked with LOGGER_ERROR.
	LOGGER_WARN	 = 2,		// messages describing a state or command that is erroneous but may be ignored under most conditions
	LOGGER_INFO	 = 3,		// any information not describing an erroneous condition are marked with LOGGER_INFO.
	LOGGER_DEBUG	 = 4,		// messages that may help debugging of libalf. most likely removed before release version.

	LOGGER_ALGORITHM = 5		// do not use this as minimal loglevel
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
		virtual void log(enum logger_loglevel l, const char* s) = 0;

};

class ignore_logger : public logger {
	public:
		virtual ~ignore_logger();

	protected:
		virtual void log(enum logger_loglevel l, const char* s);
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
		virtual void log(enum logger_loglevel l, const char* s);
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
		virtual void log(enum logger_loglevel l, const char* s);
};

}; // end namespace libalf

#endif

