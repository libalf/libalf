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

#ifndef __libalf_dispatcher_co_knowledgebase_h__
# define __libalf_dispatcher_co_knowledgebase_h__

#include <libalf/knowledgebase.h>
#include <libalf/answer.h>

#include "client_object.h"

using namespace libalf;

class co_knowledgebase : public client_object {
	private:
		knowledgebase<extended_bool> * o;

	public:
		co_knowledgebase();

		virtual ~co_knowledgebase();

		virtual enum object_type get_type()
		{ return OBJ_KNOWLEDGEBASE; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data);
};

#endif // __libalf_dispatcher_co_knowledgebase_h__

