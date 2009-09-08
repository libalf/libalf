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

#ifndef __libalf_dispatcher_client_object_h__
# define __libalf_dispatcher_client_object_h__

#include <sys/types.h>
#include <unistd.h>

#include <string>

#include <libalf/knowledgebase.h>
#include <libalf/answer.h>
#include <libalf/learning_algorithm.h>
#include <libalf/learning_algorithm.h>
#include <libalf/normalizer.h>

#include "servant.h"
#include "protocol.h"

using namespace libalf;

class servant;

class client_object {
	private:
		servant * sv;

	public:
		virtual ~client_object()
		{ };

		void set_servant(servant * sv)
		{ this->sv = sv; };

		virtual enum object_type get_type()
		{ return OBJ_NONE; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data)
		{
			log("client %d: empty object shall handle a command [%d]! disconnecting.\n", getpid(), command);
			return false;
		};
};

#endif // __libalf_dispatcher_client_object_h__

