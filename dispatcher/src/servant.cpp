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
{{{
	if(client == NULL)
		return false;

	if(!capa_sent) {
		if(!send_capabilities()) {
			cout << "client " << getpid() << " sending of initial CAPA failed. disconnecting.\n";
			return false;
		}
		capa_sent = true;
	}

	// get command field from next command
	int32_t cmd;
	int32_t session_id;

	if(!client->stream_receive_int(cmd)) {
		cout << "socket to client " << getpid() << " failed. disconnecting.\n";
		return false;
	}
	cmd = ntohl(cmd);

	cout << "client " << getpid() << " command " << cmd << ".\n";

	switch(cmd) {
		default:
		case CM_STARTTLS: // not implemented.
			cout << "received invalid command " << cmd << " from " << getpid() << " client. disconnecting.\n";
			return false;
		case CM_DISCONNECT:
			if(!client->stream_send_int(SM_ACK_DISCONNECT))
				cout << "socket to client " << getpid() << " failed when trying to send SM_ACK_DISCONNET. disconnecting anyway ;)\n";
			else
				cout << "client " << getpid() << " said CM_DISCONNECT. disconnecting.\n";
			return false;
		case CM_REQ_CAPA:
			if(!send_capabilities()) {
				cout << "socket to client " << getpid() << " failed when trying to send CAPA. disconnecting.\n";
				return false;
			}
			return true;
		case CM_REQ_SESSION:
			if(!new_session()) {
				cout << "INTERNAL ERROR: new_session() failed (client " << getpid() << ". disconnecting.\n";
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
		case CM_SES_GIVE_COUNTEREXAMPLES_AND_ANSWERS:
		case CM_SES_REQ_ALPHABET_SIZE:
		case CM_SES_SET_ALPHABET_SIZE:
		case CM_SES_INC_ALPHABET_SIZE:
		case CM_SES_REQ_STATS:
		case CM_SES_SET_STATS:
		case CM_SES_REQ_LOG:
		case CM_SES_LOG_TABLE:
		case CM_SES_NORMALIZE_WORD:
			if(!client->stream_receive_int(session_id)) {
				cout << "client " << getpid() << ": failed to receive session id in session-related command. disconnecting.\n";
				return false;
			}
			session_id = ntohl(session_id);
			if(session_id < 0 || session_id > (int)sessions.size()) {
				cout << "client " << getpid() << ": received invalid session id " << session_id << ". disconnecting.\n";
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
				case CM_SES_GIVE_COUNTEREXAMPLES_AND_ANSWERS:
					return ses->get_counterexamples_and_answer(client);
				case CM_SES_REQ_ALPHABET_SIZE:
					return ses->answer_alphabet_size(client);
				case CM_SES_SET_ALPHABET_SIZE:
					return ses->set_alphabet_size(client);
				case CM_SES_INC_ALPHABET_SIZE:
					return ses->increase_alphabet_size(client);
				case CM_SES_REQ_STATS:
					return ses->answer_stats(client);
				case CM_SES_SET_STATS:
					return ses->set_stats(client);
				case CM_SES_REQ_LOG:
					return ses->answer_log_request(client);
				case CM_SES_LOG_TABLE:
					return ses->log_table(client);
				case CM_SES_NORMALIZE_WORD:
					return ses->normalize_word(client);
				default:
					cout << "FIXME: unimplemented session command " << cmd << " from client " << getpid() << "!\n";
					return false;
			}
	}

	// should never be reached.
	cout << "INTERNAL ERROR: reached invalid code (client " << getpid() << ")\n";

	return false;
}}}

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

