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
#include <string.h>

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
{
	if(client == NULL)
		return false;

	if(!capa_sent) {
		if(!send_capabilities()) {
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

	log("client %d: comand %d.\n", pid, cmd);

	switch(cmd) {
		case CLCMD_REQ_CAPA:
			if(!send_capabilities()) {
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
			

		case CLCMD_DELETE_OBJECT:
			

		case CLCMD_GET_OBJECTTYPE:
			

		case CLCMD_OBJECT_COMMAND:
			

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
}

bool servant::send_capabilities()
{{{
	if(!client->stream_send_int(htonl(1)))
		return false;
	if(!client->stream_send_int(htonl(strlen(capa.c_str()))))
		return false;
	return client->stream_send(capa.c_str(), strlen(capa.c_str()));
}}}

bool servant::reply_version()
{{{
	if(!client->stream_send_int(htonl(1)))
		return false;

	char * version;
	int verlen;

	version = dispatcher_version();
	verlen = strlen(version);

	if(!client->stream_send_int(htonl(verlen)))
		return false;
	return client->stream_send(version, verlen);
}}}

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

