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
		virtual int get_depending_reference_count()
		{ return get_reference_count(); };
};

#endif // __libalf_dispatcher_co_logger_h__

