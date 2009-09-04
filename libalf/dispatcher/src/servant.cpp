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
{
	
}

bool servant::reply_delete_object()
{
	
}

bool servant::reply_get_objecttype()
{
	
}

bool servant::reply_object_command()
{
	
}

bool servant::reply_hello_carsten()
{{{
	int32_t count;

	if(!client->stream_receive_int(count))
		return false;

	log("client %d: said %d times hello to carsten. how nice of him!\n", pid, ntohl(count));

	if(!client->stream_send_int(1))
		return false;
	if(!client->stream_send_int(count))
		return false;

	return true;
}}}

