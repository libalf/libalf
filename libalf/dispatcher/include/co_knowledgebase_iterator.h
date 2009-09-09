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

#ifndef __libalf_dispatcher_co_knowledgebase_iterator_h__
# define __libalf_dispatcher_co_knowledgebase_iterator_h__

#include <libalf/knowledgebase.h>
#include <libalf/answer.h>

#include "client_object.h"

using namespace libalf;

class co_knowledgebase_iterator : public client_object {
	private:
		knowledgebase<extended_bool>::iterator * o;

		int referenced_knowledgebase;
	public:
		co_knowledgebase_iterator();

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

		void invalidate();
		bool is_valid();
};

#endif // __libalf_dispatcher_co_knowledgebase_iterator_h__

