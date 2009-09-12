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

#include <set>

#include <libalf/logger.h>

#include "client_object.h"

using namespace libalf;

class co_logger : public client_object {
	public:
		buffered_logger * o;

		set<int> referring_learning_algorithms;
	public:
		co_logger();

		virtual ~co_logger();

		virtual enum object_type get_type()
		{ return OBJ_LOGGER; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data);

		virtual void ref_learning_algorithm(int oid);
		virtual void deref_learning_algorithm(int oid);

		virtual int get_reference_count()
		{
			int refs = 0;
			refs += referring_learning_algorithms.size();
			return refs;
		}
};

#endif // __libalf_dispatcher_co_logger_h__

