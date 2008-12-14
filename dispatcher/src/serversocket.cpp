/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 * (c) by Stefan Rieger, i2 Informatik RWTH-Aachen
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>
#include <string>
#include <poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

serversocket::serversocket()
{{{
	sock = -1;
}}}

serversocket::serversocket(int socketid)
{{{
	sock = socketid;
}}}

serversocket::~serversocket()
{{{
	close();
}}}

bool serversocket::alive()
{{{
	struct sockaddr tmp;
	socklen_t len = sizeof(tmp);
	return getpeername(sock, &tmp, &len) == -1 ? false : true;
}}}

void serversocket::close()
{{{
	if(sock >= 0)
		::close(sock);
}}}

int serversocket::stream_send(void *msg, int length)
{{{
	int total = 0;		// how many bytes we've sent
	int bytesleft = length;	// how many we have left to send
	int n = -1;

	while (total < length) {
		if (!alive())
			return -1;
		n =::send(sock, ((char *) msg) + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft -= n;
	}
	return n == -1 ? -1 : total;	// return -1 on failure, total on success
}}}

int serversocket::stream_receive(void *msg, int data_len)
{{{
	return recv(sock, msg, data_len, MSG_WAITALL);
}}}

bool serversocket::bind(string & listen_address, uint16_t listen_port)
{{{
	struct sockaddr_in my_addr;
	int ret, opt = 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) == -1)
		return -1;

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(listen_port);
	// FIXME: use listen_address, not INADDR_ANY
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(sock, (struct sockaddr *) &my_addr, sizeof(struct sockaddr));

	if (ret == -1) {
		close();
		return false;
	} else
		return true;
}}}

bool serversocket::listen(int queue_length)
{{{
	return::listen(sock, queue_length) == -1 ? false : true;
}}}

serversocket *serversocket::accept()
{{{
	serversocket *newsock;
	struct sockaddr_in remote_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	newsock = new serversocket(::accept(sock, (struct sockaddr *) &remote_addr, &sin_size));
	if (!newsock->alive()) {
		delete newsock;
		return NULL;
	}
	return newsock;
}}}

