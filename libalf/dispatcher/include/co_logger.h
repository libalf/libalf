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

#ifndef __libalf_dispatcher_co_logger_h__
# define __libalf_dispatcher_co_logger_h__

#include <libalf/logger.h>

#include "client_object.h"

using namespace libalf;

class co_logger : public client_object {
	private:
		buffered_logger * o;

	public:
		co_logger();

		virtual ~co_logger();

		virtual enum object_type get_type()
		{ return OBJ_LOGGER; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data);
};

#endif // __libalf_dispatcher_co_logger_h__

