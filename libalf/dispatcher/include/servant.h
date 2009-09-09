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

#ifndef __libalf_dispatcher_servant_h__
# define __libalf_dispatcher_servant_h__

#include <sys/types.h>
#include <unistd.h>

#include <vector>

#include "main.h"
#include "serversocket.h"
#include "protocol.h"
#include "client_object.h"

using namespace std;

class client_object;

class servant {
	private:
		string capa;
		bool capa_sent;
		unsigned int pid;

	public: // required by client_objects
		serversocket * client;
		vector<client_object *> objects;
		unsigned int get_free_id();

	public:
		servant(serversocket *connection);
		~servant();

		bool serve();

	private:
		bool reply_create_object();
		bool reply_delete_object();
		bool reply_get_objecttype();
		bool reply_object_command();
		bool reply_capabilities();
		bool reply_version();
		bool reply_count_dispatcher_references();
		bool reply_hello_carsten();

};

#endif // __libalf_dispatcher_servant_h__

