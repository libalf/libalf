/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 by David R. Piegdon, Chair of Computer Science 2 and 7, RWTH-Aachen
 *        <david-i2@piegdon.de>
 * and 2008 by Stefan Rieger, Chair of Computer Science 2 and 7, RWTH-Aachen
 *
 */

#include <iostream>
#include <string>
#include <poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "serversocket.h"

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


int serversocket::stream_send(const void *msg, int length)
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

int serversocket::stream_receive(void *msg, int length)
{{{
	return recv(sock, msg, length, MSG_WAITALL);
}}}


bool serversocket::stream_receive_int(int32_t & ret)
{{{
	int s;
	s = recv(sock, &ret, sizeof(int32_t), MSG_WAITALL);
	if(s != sizeof(int32_t))
		return false;
	ret = ntohl(ret);
	return true;
}}}

bool serversocket::stream_send_int(int32_t val)
{{{
	val = htonl(val);
	return stream_send(&val, sizeof(int32_t));
}}}

bool serversocket::stream_receive_raw_int(int32_t & ret)
{{{
	int s;
	s = recv(sock, &ret, sizeof(int32_t), MSG_WAITALL);
	if(s != sizeof(int32_t))
		return false;
	return true;
}}}

bool serversocket::stream_send_raw_int(int32_t val)
{{{
	return stream_send(&val, sizeof(int32_t));
}}}


bool serversocket::stream_receive_raw_blob(basic_string<int32_t> & blob, int length)
{{{
	blob.clear();

	if(length < 0)
		return false;

	while(length > 0) {
		int i;
		if(!stream_receive_raw_int(i)) {
			return false;
		}
		blob.push_back(i);
		length--;
	}

	return true;
}}}

bool serversocket::stream_send_raw_blob(basic_string<int32_t> & blob)
{{{
	basic_string<int32_t>::iterator bi;

	for(bi = blob.begin(); bi != blob.end(); bi++)
		if(!stream_send_raw_int(*bi))
			return false;

	return true;
}}}


bool serversocket::stream_send_string(const char * str)
{{{
	if(!stream_send_int(strlen(str)))
		return false;
	return stream_send(str, strlen(str));
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

	ret = ::bind(sock, (struct sockaddr *) &my_addr, sizeof(struct sockaddr));

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

serversocket *serversocket::accept(struct sockaddr_in & remote_addr, socklen_t & sin_size)
{{{
	serversocket *newsock;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	newsock = new serversocket(::accept(sock, (struct sockaddr *) &remote_addr, &sin_size));
	if (!newsock->alive()) {
		delete newsock;
		return NULL;
	}
	return newsock;
}}}

