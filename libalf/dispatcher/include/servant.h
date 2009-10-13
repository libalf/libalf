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
		unsigned int store_object(client_object * o);

	public:
		servant(serversocket *connection);
		~servant();

		bool serve();
		bool send_errno(enum command_error_code);
		void clog1(const char * format, ...);
		void clog(const char * format, ...);

	private:
		bool reply_capabilities();
		bool reply_version();
		bool reply_create_object();
		bool reply_delete_object();
		bool reply_get_objecttype();
		bool reply_object_command();
		bool reply_count_dispatcher_references();
		bool reply_hello_carsten();

};

#endif // __libalf_dispatcher_servant_h__

