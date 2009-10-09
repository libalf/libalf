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
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __libalf_dispatcher_co_normalizer_h__
# define __libalf_dispatcher_co_normalizer_h__

#include <set>

#include <libalf/normalizer.h>

#include "client_object.h"

using namespace libalf;

class co_normalizer : public client_object {
	public:
		normalizer * o;

		set<int> referring_learning_algorithms;
	public:
		// type MUST be valid.
		co_normalizer(enum libalf::normalizer::type type);

		virtual ~co_normalizer();

		virtual enum object_type get_type()
		{ return OBJ_NORMALIZER; };

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

#endif // __libalf_dispatcher_co_normalizer_h__

