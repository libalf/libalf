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

#include "serversocket.h"

class servant {
	private:
		serversocket * client;
	public:
		servant();
		servant(serversocket *connection);
		~servant();

		bool serve();

};

#endif // __libalf_servant_h__

