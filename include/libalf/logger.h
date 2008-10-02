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
#include <functional>

namespace libalf {

enum logger_loglevel {
	LOGGER_NONE	 = 0,
	LOGGER_ERROR	 = 1,
	LOGGER_WARN	 = 2,
	LOGGER_INFO	 = 3,
	LOGGER_DEBUG	 = 4,

	LOGGER_ALGORITHM = 5
};

class logger : std::binary_function< enum logger_loglevel, string&, void > {

	public:
		virtual void operator()(enum logger_loglevel, string&) = 0;

};

}; // end namespace libalf

