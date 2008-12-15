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

	if(!client->stream_get_int(cmd)) {
		cout << "socket to client failed. disconnecting.\n";
		return false;
	}

	switch(cmd) {
		default:
		case CM_STARTTLS: // not implemented.
			cout << "received invalid command " << cmd << " from client. disconnecting.\n";
			return false;
		case CM_DISCONNECT:
			cout << "client said FIN. disconnecting.\n";
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
			if(!client->stream_get_int(session_id)) {
				cout << "failed to receive session id in session-related command. disconnecting.\n";
				return false;
			}
			if(session_id < 0 || session_id > (int)sessions.size()) {
				cout << "received invalid session id " << session_id << " from client. disconnecting.\n";
				return false;
			}

			session * ses = sessions[session_id];

			// FIXME

			break;
	}

	return true;
}

bool servant::send_capabilities()
{
	return true;
}

bool servant::new_session()
{
	int ret = sessions.size();
	sessions.push_back(new session);

	// parse structures from session command
	
	// return session id
	

	return true;
}

