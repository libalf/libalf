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
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>

#include <libalf/alf.h>
#include <libalf/learning_algorithm.h>
#include <libalf/normalizer.h>

#include "main.h"
#include "servant.h"
#include "serversocket.h"
#include "protocol.h"

#include "client_object.h"
#include "co_learning_algorithm.h"
#include "co_knowledgebase.h"
#include "co_knowledgebase_iterator.h"
#include "co_logger.h"
#include "co_normalizer.h"

using namespace std;
using namespace libalf;

servant::servant(serversocket *connection)
{{{
	client = connection;
	capa = "protocol-version-1";
	capa_sent = false;
	pid = getpid();
	usleep(100000); // wait for parent to print its connection-message
}}}

servant::~servant()
{{{
	if(client)
		delete client;
}}}

unsigned int servant::store_object(client_object * o)
{{{
	unsigned int new_id;

	// try to find free slot
	for(new_id = 0; new_id < objects.size(); new_id++)
		if(objects[new_id] == NULL)
			break;

	// or create a new one
	if(new_id == objects.size())
		objects.push_back((client_object *) NULL);

	objects[new_id] = o;
	o->set_servant(this);
	o->set_id(new_id);

	return new_id;
}}}


bool servant::serve()
{{{
	if(client == NULL)
		return false;

	if(!capa_sent) {
#ifdef VERBOSE_DEBUG
	clog1("CLCMD: initial %s[%d].\n", cmd2string(CLCMD_REQ_CAPA), CLCMD_REQ_CAPA);
#endif
		if(!reply_capabilities()) {
			clog("sending of initial CAPA failed. DISCONNECTING.\n");
			return false;
		}
		capa_sent = true;
	}

	// get command field from next command
	int32_t cmd;

	if(!client->stream_receive_int(cmd)) {
		clog("socket failed right before CLCMD. DISCONNECTING.\n");
		return false;
	}

#ifdef VERBOSE_DEBUG
	clog1("CLCMD: %s[%d].\n", cmd2string(cmd), cmd);
#endif

	enum command_error_code r;

	switch(cmd) {
		case CLCMD_REQ_CAPA:
			if(!reply_capabilities()) {
				clog("failed to send requested CAPA. DISCONNECTING.\n");
				return false;
			}
			return true;

		case CLCMD_REQ_VERSION:
			if(!reply_version()) {
				clog("failed to send requested version. DISCONNECTING.\n");
				return false;
			}
			return true;

		case CLCMD_DISCONNECT:
			{
				vector<client_object *>::iterator vi;
				int c = 0;
				for(vi = objects.begin(); vi != objects.end(); vi++)
					if(*vi)
						c++;
				if(c)
					r = ERR_REMAINING_OBJECTS;
				else
					r = ERR_SUCCESS;
			}

			if(!send_errno(r)) {
				clog("failed to ACK disconnect. DISCONNECTING anyway ;)\n");
			} else {
#ifdef VERBOSE_DEBUG
				clog("valid disconnect. bye bye. DISCONNECTING.\n");
#endif
			}
			return false;

		case CLCMD_CREATE_OBJECT:
			if(!reply_create_object()) {
				clog("create object command failed. DISCONNECTING.\n");
				return false;
			}
			return true;

		case CLCMD_DELETE_OBJECT:
			if(!reply_delete_object()) {
				clog("delete object command failed. DISCONNECTING.\n");
				return false;
			}
			return true;

		case CLCMD_GET_OBJECTTYPE:
			if(!reply_get_objecttype()) {
				clog("get objecttype command failed. DISCONNECTING.\n");
				return false;
			}
			return true;

		case CLCMD_OBJECT_COMMAND:
			if(!reply_object_command()) {
				clog("object command failed. DISCONNECTING.\n");
				return false;
			}
			return true;

		case CLCMD_COUNT_DISPATCHER_REFERENCES:
			if(!reply_count_dispatcher_references()) {
				clog("count_dispatcher_references command failed. DISCONNECTING.\n");
				return false;
			};
			return true;

		case CLCMD_HELLO_CARSTEN:
			if(!reply_hello_carsten()) {
				clog("tried hello carsten, but failed. DISCONNECTING.\n");
				return false;
			};
			return true;

		case CLCMD_STARTTLS:
		case CLCMD_AUTH:
			clog("command %d requested but is not implemented. DISCONNECTING.\n", cmd);
			send_errno(ERR_NOT_IMPLEMENTED);
			return false;
		default:
			clog("sent invalid command %d. DISCONNECTING.\n", cmd);
			return false;
	}

	// should never be reached.
	clog("INTERNAL ERROR: reached invalid code (client %d). DISCONNECTING this client.\n");

	return false;
}}}

bool servant::send_errno(enum command_error_code err)
{{{
#ifdef VERBOSE_DEBUG
	clog("result: %s (%d)\n", err2string(err), err);
#endif
	return client->stream_send_int(err);
}}}

void servant::clog1(const char * format, ...)
{{{
	va_list ap;

	print_time();

	printf(" client %d: ", pid);

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}}}

void servant::clog(const char * format, ...)
{{{
	va_list ap;

	print_time();

#ifdef VERBOSE_DEBUG
	printf(" client %d:     ", pid);
#else
	printf(" client %d: ", pid);
#endif

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}}}


bool servant::reply_capabilities()
{{{
	if(!send_errno(ERR_SUCCESS))
		return false;
	return client->stream_send_string(capa.c_str());
}}}

bool servant::reply_version()
{{{
	if(!send_errno(ERR_SUCCESS))
		return false;
	return client->stream_send_string(dispatcher_version());
}}}



bool servant::reply_create_object()
{{{
	int t, u;
	enum object_type type;
	int size;
	basic_string<int32_t> data;

	if(!client->stream_receive_int(t))
		return false;
	type = (enum object_type) t;

	if(!client->stream_receive_int(size))
		return false;

	if(size < 0) {
		clog("BAD PARAMETER SIZE %d for object command\n", size);
		return false;
	}

	if(!client->stream_receive_raw_blob(data, size))
		return false;

	// find free object-id
	unsigned int new_id;

	switch(type) {
		case OBJ_LOGGER:
			if(data.size() != 0)
				goto bad_parameter_count;

			new_id = store_object(new co_logger);
			break;
		case OBJ_KNOWLEDGEBASE:
			if(data.size() != 0)
				goto bad_parameter_count;

			new_id = store_object(new co_knowledgebase);
			break;
		case OBJ_KNOWLEDGEBASE_ITERATOR:
			if(data.size() != 0)
				goto bad_parameter_count;

			new_id = store_object(new co_knowledgebase_iterator);
			break;
		case OBJ_LEARNING_ALGORITHM:
			if(data.size() != 2)
				goto bad_parameter_count;
			t = ntohl(data[0]); // algorithm type
			u = ntohl(data[1]); // alphabet_size

			// FIXME: catch not-implemented algorithms!

			if(t <= learning_algorithm<extended_bool>::ALG_NONE || t >= learning_algorithm<extended_bool>::ALG_LAST_INVALID)
				goto bad_parameters;
			if(u <= 0)
				goto bad_parameters;

			new_id = store_object(new co_learning_algorithm( (enum libalf::learning_algorithm<extended_bool>::algorithm) t, u));
			break;
		case OBJ_NORMALIZER:
			if(data.size() != 1)
				goto bad_parameter_count;
			t = ntohl(data[0]); // normalizer type
			if(t <= normalizer::NORMALIZER_NONE || t >= normalizer::NORMALIZER_LAST_INVALID)
				goto bad_parameters;

			new_id = store_object(new co_normalizer( (enum libalf::normalizer::type) t));
			break;
		default:
			goto bad_parameters;
	}

	if(!send_errno(ERR_SUCCESS))
		return false;

#ifdef VERBOSE_DEBUG
	clog("created object %d type %s (%d).\n", new_id, obj2string(type), type);
#endif

	return (client->stream_send_int(new_id));

bad_parameter_count:
	return send_errno(ERR_BAD_PARAMETER_COUNT);
bad_parameters:
	return send_errno(ERR_BAD_PARAMETERS);
}}}

bool servant::reply_delete_object()
{{{
	int id;

	if(!client->stream_receive_int(id))
		return false;

	if(id < 0 || id >= (int)objects.size() || objects[id] == NULL) {
		return send_errno(ERR_NO_OBJECT);
	} else {
		enum command_error_code r;
		if(objects[id]->get_depending_reference_count() != 0)
			r = ERR_UNRESOLVED_REFERENCES_REMOVED;
		else
			r = ERR_SUCCESS;
#ifdef VERBOSE_DEBUG
		clog("deleting object %d type %s[%d].\n", id, obj2string(objects[id]->get_type()), objects[id]->get_type(), err2string(r));
#endif
		delete objects[id];
		objects[id] = NULL;
		return send_errno(r);
	}
}}}

bool servant::reply_get_objecttype()
{{{
	int id;

	if(!client->stream_receive_int(id))
		return false;

	if(id < 0 || id >= (int)objects.size() || objects[id] == NULL) {
		return send_errno(ERR_NO_OBJECT);
	} else {
#ifdef VERBOSE_DEBUG
		clog("object %d: requested type is %s[%d].\n", id, obj2string(objects[id]->get_type()), objects[id]->get_type());
#endif
		if(!send_errno(ERR_SUCCESS))
			return false;
		return client->stream_send_int( (int) objects[id]->get_type() );
	}
}}}

bool servant::reply_object_command()
// expects that object[]->handle_command sends all replies to the
// client (including the bool indicating validity of command)
{{{
	int id;
	int command;
	int size;
	basic_string<int32_t> command_data;

	if(!client->stream_receive_int(id))
		return false;

	if(!client->stream_receive_int(command))
		return false;

	if(!client->stream_receive_int(size))
		return false;

	if(!client->stream_receive_raw_blob(command_data, size))
		return false;

	// check validity of object-id
	if(id < 0 || id >= (int)objects.size() || objects[id] == NULL)
		return send_errno(ERR_NO_OBJECT);

#ifdef VERBOSE_DEBUG
	clog("object command %d on object %d type %s[%d] with parameters of size %d\n",
		command, id, obj2string(objects[id]->get_type()), objects[id]->get_type(), command_data.size());
#endif

	return objects[id]->handle_command(command, command_data);
}}}


bool servant::reply_count_dispatcher_references()
{{{
	int id;

	if(!client->stream_receive_int(id))
		return false;

	// check validity of object-id
	if(id < 0 || id >= (int)objects.size() || objects[id] == NULL)
		return send_errno(ERR_NO_OBJECT);

	if(!send_errno(ERR_SUCCESS))
		return false;

	return client->stream_send_int(objects[id]->get_reference_count());
}}}


bool servant::reply_hello_carsten()
{{{
	int32_t count;

	if(!client->stream_receive_int(count))
		return false;

	clog("said %d times hello to carsten. how nice of him!\n", count);

	if(!send_errno(ERR_SUCCESS))
		return false;

	return client->stream_send_int(count);
}}}


