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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_dispatcher_co_knowledgebase_iterator_h__
# define __libalf_dispatcher_co_knowledgebase_iterator_h__

#include <libalf/knowledgebase.h>
#include <libalf/answer.h>

#include "client_object.h"

using namespace libalf;

class co_knowledgebase_iterator : public client_object {
	public:
		knowledgebase<bool>::iterator * o;

		int referenced_knowledgebase;
	public:
		co_knowledgebase_iterator();
		co_knowledgebase_iterator(knowledgebase<bool>::iterator * o);

		virtual ~co_knowledgebase_iterator();

		virtual enum object_type get_type()
		{ return OBJ_KNOWLEDGEBASE_ITERATOR; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data);

		virtual void ref_knowledgebase(int oid);
		virtual void deref_knowledgebase(int oid);

		virtual int get_reference_count()
		{
			int refs = 0;
			if(referenced_knowledgebase != -1)
				refs++;
			return refs;
		}
		virtual int get_depending_reference_count()
		{
			// we don't care  for references in a knowledgebase::iterator.
			// none of these depend on the learning_algorithm,
			// but it depends on them.
			return 0;
		}

		void invalidate();
		bool is_valid();
};

#endif // __libalf_dispatcher_co_knowledgebase_iterator_h__

