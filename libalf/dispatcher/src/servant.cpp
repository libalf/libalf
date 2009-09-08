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

#include <sys/types.h>
#include <unistd.h>

#include <libalf/alf.h>

#include "main.h"
#include "servant.h"
#include "serversocket.h"
#include "protocol.h"

#include "client_object.h"
#include "co_algorithm.h"
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
}}}

servant::~servant()
{{{
	if(client)
		delete client;
}}}



bool servant::serve()
{{{
	if(client == NULL)
		return false;

	if(!capa_sent) {
		if(!reply_capabilities()) {
			log("client %d: sending of initial CAPA failed. disconnecting.\n", pid);
			return false;
		}
		capa_sent = true;
	}

	// get command field from next command
	int32_t cmd;

	if(!client->stream_receive_int(cmd)) {
		log("socket to client %d failed. disconnecting.\n", pid);
		return false;
	}
	cmd = ntohl(cmd);

#ifdef READABLE
	log("client %d: command %d (%s).\n", pid, cmd, cmd2string(cmd));
#else
	log("client %d: command %d.\n", pid, cmd);
#endif

	switch(cmd) {
		case CLCMD_REQ_CAPA:
			if(!reply_capabilities()) {
				log("client %d: failed to send requested CAPA. disconnecting.\n", pid);
				return false;
			}
			return true;

		case CLCMD_REQ_VERSION:
			if(!reply_version()) {
				log("client %d: failed to send requested version. disconnecting.\n", pid);
				return false;
			}
			return true;

		case CLCMD_DISCONNECT:
			if(!client->stream_send_int(1)) {
				log("client %d: failed to ACK disconnect. disconnecting anyway ;)\n", pid);
			} else {
				log("client %d: valid disconnect. bye bye.\n", pid);
			}
			return false;

		case CLCMD_CREATE_OBJECT:
			if(!reply_create_object()) {
				log("client %d: create object command failed. disconnecting.\n", pid);
				return false;
			}
			return true;

		case CLCMD_DELETE_OBJECT:
			if(!reply_delete_object()) {
				log("client %d: delete object command failed. disconnecting.\n", pid);
				return false;
			}
			return true;

		case CLCMD_GET_OBJECTTYPE:
			if(!reply_get_objecttype()) {
				log("client %d: get objecttype command failed. disconnecting.\n", pid);
				return false;
			}
			return true;

		case CLCMD_OBJECT_COMMAND:
			if(!reply_object_command()) {
				log("client %d: object command failed. disconnecting.\n", pid);
				return false;
			}
			return true;

		case CLCMD_HELLO_CARSTEN:
			if(!reply_hello_carsten()) {
				log("client %d: tried hello carsten, but failed. disconnecting.\n", pid);
				return false;
			};
			return true;

		case CLCMD_STARTTLS:
		case CLCMD_AUTH:
		default:
			log("client %d: sent invalid command %d. disconnecting.\n", pid, cmd);
			return false;
	}

	// should never be reached.
	log("INTERNAL ERROR: reached invalid code (client %d). disconnecting this client.\n", pid);

	return false;
}}}



bool servant::reply_capabilities()
{{{
	if(!client->stream_send_int(htonl(1)))
		return false;
	return client->stream_send_string(capa.c_str());
}}}

bool servant::reply_version()
{{{
	if(!client->stream_send_int(htonl(1)))
		return false;
	return client->stream_send_string(dispatcher_version());
}}}



bool servant::reply_create_object()
{{{
	int t;
	enum object_type type;
	int size;
	basic_string<int32_t> data;

	if(!client->stream_receive_int(t))
		return false;
	type = (enum object_type)ntohl(t);

	if(!client->stream_receive_int(size))
		return false;
	size = ntohl(size);

	if(!client->stream_receive_blob(data, size))
		return false;

	// find free object-id
	unsigned int new_id;

	for(new_id = 0; new_id < objects.size(); new_id++)
		if(objects[new_id] == NULL)
			break;
	if(new_id == objects.size())
		objects.push_back((client_object *) NULL);

	switch(type) {
		case OBJ_LOGGER:
			if(data.size() != 0)
				goto bad_data;

			objects[new_id] = new co_logger;

			break;
		case OBJ_KNOWLEDGEBASE:
			if(data.size() != 0)
				goto bad_data;

			objects[new_id] = new co_knowledgebase;

			break;
		case OBJ_KNOWLEDGEBASE_ITERATOR:
			if(data.size() != 0)
				goto bad_data;

			objects[new_id] = new co_knowledgebase_iterator;

			break;
		case OBJ_ALGORITHM:
			if(data.size() != 1)
				goto bad_data;
			t = ntohl(data[0]);
			if(t < 1 || t > 7)
				goto bad_data;

			objects[new_id] = new co_algorithm( (enum libalf::learning_algorithm<extended_bool>::algorithm) t);

			break;
		case OBJ_NORMALIZER:
			if(data.size() != 1)
				goto bad_data;
			t = ntohl(data[0]);
			if(t < 1 || t > 1)
				goto bad_data;

			objects[new_id] = new co_normalizer( (enum libalf::normalizer::type) t);

			break;
		default:
			return client->stream_send_int(htonl(0));
	}

	if(!client->stream_send_int(1))
		return false;
	return (client->stream_send_int(1));

bad_data:
	return client->stream_send_int(htonl(0));
}}}

bool servant::reply_delete_object()
// FIXME: what if an object is referenced by another object?
{{{
	int id;

	if(!client->stream_receive_int(id))
		return false;

	id = ntohl(id);
	if(id < 0 || id >= (int)objects.size() || objects[id] == NULL) {
		return client->stream_send_int(htonl(0));
	} else {
		delete objects[id];
		objects[id] = NULL;
		return client->stream_send_int(htonl(1));
	}
}}}

bool servant::reply_get_objecttype()
{{{
	int id;

	if(!client->stream_receive_int(id))
		return false;

	id = ntohl(id);
	if(id < 0 || id >= (int)objects.size() || objects[id] == NULL) {
		return client->stream_send_int(htonl(0));
	} else {
		if(!client->stream_send_int(htonl(1)))
			return false;
		return client->stream_send_int(htonl( (int) objects[id]->get_type() ));
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
	id = ntohl(id);

	if(!client->stream_receive_int(command))
		return false;
	command = ntohl(id);

	if(!client->stream_receive_int(size))
		return false;
	size = ntohl(size);

	if(!client->stream_receive_blob(command_data, size))
		return false;

	// check validity of object-id
	if(id < 0 || id >= (int)objects.size() || objects[id] == NULL)
		return client->stream_send_int(htonl(0));

	return objects[id]->handle_command(command, command_data);
}}}



bool servant::reply_hello_carsten()
{{{
	int32_t count;

	if(!client->stream_receive_int(count))
		return false;

	log("client %d: said %d times hello to carsten. how nice of him!\n", pid, ntohl(count));

	if(!client->stream_send_int(1))
		return false;

	return client->stream_send_int(count);
}}}

