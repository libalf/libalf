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

#include <iostream>
#include <string>

#include <libalf/alf.h>

#include "servant.h"
#include "serversocket.h"
#include "protocol.h"

using namespace std;
using namespace libalf;

servant::servant()
{{{
	client = NULL;
	capa_sent = false;
}}}

servant::servant(serversocket *connection)
{{{
	client = connection;
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
			cout << "sending of initial CAPA failed. disconnecting.\n";
			return false;
		}
		capa_sent = true;
	}

	// get command field from next command
	int32_t cmd;
	int32_t session_id;

	if(!client->stream_receive_int(cmd)) {
		cout << "socket to client failed. disconnecting.\n";
		return false;
	}
	cmd = ntohl(cmd);

	cout << "client command " << cmd << ".\n";

	switch(cmd) {
		default:
		case CM_STARTTLS: // not implemented.
			cout << "received invalid command " << cmd << " from client. disconnecting.\n";
			return false;
		case CM_DISCONNECT:
			if(!client->stream_send_int(SM_ACK_DISCONNECT))
				cout << "tried to send SM_ACK_DISCONNET. failed. disconnecting anyway ;)\n";
			else
				cout << "client said CM_DISCONNECT. disconnecting.\n";
			return false;
		case CM_REQ_CAPA:
			if(!send_capabilities()) {
				cout << "sending of initial CAPA failed. disconnecting.\n";
				return false;
			}
			break;
		case CM_REQ_SESSION:
			if(!new_session()) {
				cout << "new session failed. disconnecting.\n";
				return false;
			}
			break;
		case CM_SES_SET_MODALITIES:
		case CM_SES_REQ_STATUS:
		case CM_SES_SET_STATUS:
		case CM_SES_CONJECTURE:
		case CM_SES_ADVANCE:
		case CM_SES_ANSWER_SQT:
		case CM_SES_GIVE_COUNTEREXAMPLES:
		case CM_SES_REQ_STATS:
		case CM_SES_LOG:
			if(!client->stream_receive_int(session_id)) {
				cout << "failed to receive session id in session-related command. disconnecting.\n";
				return false;
			}
			session_id = ntohl(session_id);
			if(session_id < 0 || session_id > (int)sessions.size()) {
				cout << "received invalid session id " << session_id << " from client. disconnecting.\n";
				return false;
			}

cout << "session command, session " << session_id << ".\n";

			session * ses = sessions[session_id];

			// FIXME

			break;
	}

	return true;
}

bool servant::send_capabilities()
{
	// prepare CAPAs
	basic_string<int32_t> capa;

	capa += htonl(SM_ACK_CAPA);
	capa += htonl(0);
	// no upcoming CAPA fields. we are capable of nothing.
	capa += htonl(0);

	return client->stream_send_blob(capa);
}

bool servant::new_session()
{
	int ret = sessions.size();
	session * ses = new session;

	sessions.push_back(ses);

	// parse structures from session command
	
	// return session id
	

	return true;
}

