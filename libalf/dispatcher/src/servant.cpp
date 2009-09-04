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

#include <iostream>
#include <string>

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
			cout << "client " << getpid() << ": sending of initial CAPA failed. disconnecting.\n";
			return false;
		}
		capa_sent = true;
	}

	// get command field from next command
	int32_t cmd;

	if(!client->stream_receive_int(cmd)) {
		print_time();
		cout << "socket to client " << getpid() << " failed. disconnecting.\n";
		return false;
	}
	cmd = ntohl(cmd);

	print_time();
	cout << "client " << getpid() << ": command " << cmd << ".\n";

	switch(cmd) {
		case CLCMD_REQ_CAPA:
			if(!send_capabilities()) {
				cout << "client "<< getpid() << ": failed to send requested CAPA. disconnecting.\n";
				return false;
			}
			return true;

		case CLCMD_REQ_VERSION:
			if(!reply_version()) {
				cout << "client "<<getpid()<<": failed to send requested version. disconnecting.\n";
				return false;
			}
			return true;

		case CLCMD_DISCONNECT:
			if(!client->stream_send_int(1)) {
				cout << "client "<<getpid()<<": failed to ACK disconnect. disconnecting anyway ;)\n";
			} else {
				cout << "client "<<getpid()<<": valid disconnect. bye bye.\n";
			}
			return false;

		case CLCMD_CREATE_OBJECT:
			

		case CLCMD_DELETE_OBJECT:
			

		case CLCMD_GET_OBJECTTYPE:
			

		case CLCMD_OBJECT_COMMAND:
			

		case CLCMD_HELLO_CARSTEN:
			if(!reply_hello_carsten()) {
				cout << "client "<<getpid()<<": tried hello carsten, but failed. disconnecting.\n";
				return false;
			};
			return true;

		case CLCMD_STARTTLS:
		case CLCMD_AUTH:
		default:
			cout << "client " << getpid() << " sent invalid command " << cmd << ". disconnecting.\n";
			return false;
	}

	// should never be reached.
	print_time();
	cout << "INTERNAL ERROR: reached invalid code (client " << getpid() << "). disconnecting this client.\n";

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

	cout << "client "<<getpid()<<" said "<< ntohl(count) <<" times hello to carsten. how nice of him!\n";

	if(!client->stream_send_int(1))
		return false;
	if(!client->stream_send_int(count))
		return false;

	return true;
}}}

