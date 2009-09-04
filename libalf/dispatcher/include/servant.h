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

class servant {
	private:
		serversocket * client;
		bool capa_sent;

	public:
		servant();
		servant(serversocket *connection);
		~servant();

		bool serve();

	private:
		bool send_capabilities();
		bool send_version();

};

#endif // __libalf_servant_h__

