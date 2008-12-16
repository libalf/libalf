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

#include <libalf/learning_algorithm.h>
#include <libalf/algorithm_angluin.h>
#include <libalf/answer.h>
#include <libalf/logger.h>

#include <serversocket.h>
#include <session.h>
#include <protocol.h>

using namespace std;
using namespace libalf;

session::session()
{{{
	alg = NULL;
	logger.set_minimal_loglevel(LOGGER_DEBUG);
	logger.set_log_algorithm(true);
}}}

session::session(enum learning_algorithm<extended_bool>::algorithm algorithm, int alphabet_size)
{{{
	logger.set_minimal_loglevel(LOGGER_DEBUG);
	logger.set_log_algorithm(true);
	switch (algorithm) {
		case angluin_simple_observationtable<extended_bool>::ALG_ANGLUIN:
			alg = new angluin_simple_observationtable<extended_bool>(NULL, &logger, alphabet_size);
			logger(LOGGER_INFO, "new session: angluin observationtable\n");
cout << "new session angluin\n";
			break;
		default:
			alg = NULL;
			logger(LOGGER_ERROR, "new session: received invalid algorithm '%d'\n", algorithm);
cout << "new session default\n";
			return;
	}
}}}

session::~session()
{{{
	if(alg)
		delete alg;
}}}

bool session::set_modalities(serversocket * sock)
{
cout << "session set_modalities\n";
return false;
}
bool session::answer_status(serversocket * sock)
{{{
cout << "session answer_status\n";
	if(!sock->stream_send_int(htonl(SM_SES_ACK_REQ_STATUS)))
		return false;
	basic_string<int32_t> blob;
	blob = alg->serialize();
	return sock->stream_send_blob(blob);
}}}
bool session::set_status(serversocket * sock)
{
cout << "session set_status\n";
return false;
}
bool session::answer_conjecture(serversocket * sock)
{{{
cout << "session answer_conjecture\n";
	if(!sock->stream_send_int(htonl(SM_SES_ACK_CONJECTURE)))
		return false;
	return sock->stream_send_int(htonl( alg->conjecture_ready() ? 1 : 0 ));
}}}
bool session::advance(serversocket * sock)
{
cout << "session advance\n";
return false;
}
bool session::get_sqt(serversocket * sock)
{
cout << "session get_sqt\n";
return false;
}
bool session::get_counterexamples(serversocket * sock)
{
cout << "session get_counterexamples\n";
return false;
}
bool session::answer_alphabet_size(serversocket * sock)
{{{
cout << "session answer_alphabet_size\n";
	if(!sock->stream_send_int(htonl(SM_SES_ACK_ALPHABET_SIZE)))
		return false;

	if(alg)
		if(!sock->stream_send_int(htonl(alg->get_alphabet_size())))
			return false;
	else
		if(!sock->stream_send_int(htonl(0)))
			return false;

	return true;
}}}
bool session::answer_stats(serversocket * sock)
{{{
cout << "session answer_stats\n";
	if(!sock->stream_send_int(htonl(SM_SES_ACK_STATS)))
		return false;
	basic_string<int32_t> blob;
	blob = stats.serialize();
	return sock->stream_send_blob(blob);
}}}
bool session::answer_log_request(serversocket * sock)
{{{
cout << "session answer_log_request\n";
	string * s;
	const char * c;
	int l;

	if(!sock->stream_send_int(htonl(SM_SES_ACK_LOG)))
		return false;

	s = logger.receive_and_flush();
	c = s->c_str();
	l = s->length();

	if(!sock->stream_send_int(htonl(l)))
		return false;
	if(!sock->stream_send(c, l))
		return false;

	delete s;

	return true;
}}}

