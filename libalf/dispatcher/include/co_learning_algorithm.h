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
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_dispatcher_co_algorithm_h__
# define __libalf_dispatcher_co_algorithm_h__

#include <libalf/learning_algorithm.h>

#include "client_object.h"

using namespace libalf;

class co_learning_algorithm : public client_object {
	public:
		learning_algorithm<extended_bool> * o;

		int referenced_knowledgebase;
		int referenced_logger;
		int referenced_normalizer;
	public:
		// alg MUST be valid.
		co_learning_algorithm(enum libalf::learning_algorithm<extended_bool>::algorithm alg, int alphabet_size);

		virtual ~co_learning_algorithm();

		virtual enum object_type get_type()
		{ return OBJ_LEARNING_ALGORITHM; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data);

		virtual void ref_knowledgebase(int oid);
		virtual void deref_knowledgebase(int oid);

		virtual void ref_logger(int oid);
		virtual void deref_logger(int oid);

		virtual void ref_normalizer(int oid);
		virtual void deref_normalizer(int oid);

		virtual int get_reference_count()
		{
			int refs = 0;
			if(referenced_knowledgebase != -1)
				refs++;
			if(referenced_logger != -1)
				refs++;
			if(referenced_normalizer != -1)
				refs++;
			return refs;
		}
		virtual int get_depending_reference_count()
		{
			// we don't care  for references in a learning_algorithm.
			// none of these depend on the learning_algorithm,
			// but it depends on them.
			return 0;
		};
};

#endif // __libalf_dispatcher_co_algorithm_h__

