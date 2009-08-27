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

#ifndef __libalf_servant_h__
# define __libalf_servant_h__

#include <vector>

#include "serversocket.h"
#include "session.h"

class servant {
	private:
		serversocket * client;
		buffered_logger logger;
		bool capa_sent;
		vector<session*> sessions;

	public:
		servant();
		servant(serversocket *connection);
		~servant();

		bool serve();

	private:
		bool send_capabilities();
		bool send_version();
		bool new_session();

};

#endif // __libalf_servant_h__

