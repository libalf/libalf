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
			return true;
		case CM_REQ_SESSION:
			if(!new_session()) {
				cout << "new session failed. disconnecting.\n";
				return false;
			}
			return true;
		case CM_SES_SET_MODALITIES:
		case CM_SES_REQ_STATUS:
		case CM_SES_SET_STATUS:
		case CM_SES_CONJECTURE:
		case CM_SES_ADVANCE:
		case CM_SES_ANSWER_SQT:
		case CM_SES_GIVE_COUNTEREXAMPLES:
		case CM_SES_REQ_ALPHABET_SIZE:
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

			session * ses = sessions[session_id];

			switch(cmd) {
				case CM_SES_SET_MODALITIES:
					return ses->set_modalities(client);
				case CM_SES_REQ_STATUS:
					return ses->answer_status(client);
				case CM_SES_SET_STATUS:
					return ses->set_status(client);
				case CM_SES_CONJECTURE:
					return ses->answer_conjecture(client);
				case CM_SES_ADVANCE:
					return ses->advance(client);
				case CM_SES_ANSWER_SQT:
					return ses->get_sqt(client);
				case CM_SES_GIVE_COUNTEREXAMPLES:
					return ses->get_counterexamples(client);
				case CM_SES_REQ_ALPHABET_SIZE:
					return ses->answer_alphabet_size(client);
				case CM_SES_REQ_STATS:
					return ses->answer_stats(client);
				case CM_SES_LOG:
					return ses->answer_log_request(client);
				default:
					cout << "FIXME: unimplemented session command " << cmd << "!\n";
					return false;
			}
	}

	// should never be reached.
	cout << "reached invalid code.\n";

	return false;
}

bool servant::send_capabilities()
{{{
	// prepare CAPAs
	basic_string<int32_t> capa;

	capa += htonl(SM_ACK_CAPA);
	capa += htonl(0);
	// no upcoming CAPA fields. we are capable of nothing.
	capa += htonl(0);

	return client->stream_send_blob(capa);
}}}

bool servant::new_session()
{{{
	int32_t s;
	enum learning_algorithm<extended_bool>::algorithm algorithm;
	int alphabet_size;

	if(!client->stream_receive_int(s))
		return false;
	algorithm = (learning_algorithm<extended_bool>::algorithm) ntohl(s);

	if(!client->stream_receive_int(s))
		return false;
	alphabet_size = ntohl(s);

	int snum = sessions.size();

	sessions.push_back(new session(algorithm, alphabet_size));

	if(!client->stream_send_int(htonl(SM_ACK_SESSION)))
		return false;
	if(!client->stream_send_int(htonl(snum)))
		return false;

	return true;
}}}

