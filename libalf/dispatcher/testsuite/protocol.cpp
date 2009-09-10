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

#include <string>
#include <iostream>
#include <iterator>

#include <poll.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#include "protocol.h"
#include "defs.h"

using namespace std;


void print_blob(basic_string<int32_t> blob)
{{{
	basic_string<int32_t>::iterator bi;

	for(bi = blob.begin(); bi != blob.end(); bi++)
		cout << *bi << ";";
	cout << "\n";
}}}


basic_string<int32_t> receive_blob_raw(int sock, int count)
{{{
	basic_string<int32_t> buf;

	for(int i = 0; i < count; i++) {
		int32_t c;
		if(sizeof(c) != recv(sock, &c, sizeof(c), MSG_WAITALL)) {
			cout << "failed to read int32_t.\n";
			exit(-1);
		}
		buf += c;
	}

	return buf;
}}}

void send_blob_raw(int sock, basic_string<int32_t> blob)
{{{
	basic_string<int32_t>::iterator bi;

	for(bi = blob.begin(); bi != blob.end(); bi++) {
		int32_t s = *bi;
		if(sizeof(int32_t) != send(sock, (void*) &s, sizeof(int32_t), 0)) {
			cout << "failed to write int32_t.\n";
			exit(-2);
		}
	}
}}}


basic_string<int32_t> receive_blob(int sock, int count)
{{{
	basic_string<int32_t> buf;

	for(int i = 0; i < count; i++) {
		int32_t c;
		if(sizeof(c) != recv(sock, &c, sizeof(c), MSG_WAITALL)) {
			cout << "failed to read int32_t.\n";
			exit(-1);
		}
		buf += c;
	}

	basic_string<int32_t>::iterator si;
	for(si = buf.begin(); si != buf.end(); si++)
		*si = ntohl(*si);

	return buf;
}}}

void send_blob(int sock, basic_string<int32_t> blob)
{{{
	basic_string<int32_t>::iterator bi;

	for(bi = blob.begin(); bi != blob.end(); bi++) {
		int32_t s = htonl(*bi);
		if(sizeof(int32_t) != send(sock, (void*) &s, sizeof(int32_t), 0)) {
			cout << "failed to write int32_t.\n";
			exit(-2);
		}
	}
}}}


string receive_string(int sock)
{{{
	string r;
	int32_t size = -128;

	if(sizeof(size) != recv(sock, &size, sizeof(size), MSG_WAITALL)) {
		cout << "failed to read size of string.\n";
		exit(-1);
	}
	size = ntohl(size);
	cout << "receiving string size "<<size<<".\n";
	while(size) {
		char d;
		if(sizeof(d) != recv(sock, &d, sizeof(d), MSG_WAITALL)) {
			cout << "failed to read char of string.\n";
			exit(-1);
		}
		r.push_back(d);
		size--;
	}

	return r;
}}}


int main()
{
	int sock;

	{{{ // connect socket to server
		struct hostent * host;
		int opt = 1;
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt)) == -1) {
			cout << "failed to init socket\n";
			return 1;
		}
		host = gethostbyname("localhost");
		if(host == NULL) {
			cout << "failed to look up hostname.\n";
			return 2;
		}
		struct sockaddr_in serv_addr;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(DISPATCHER_DEFAULT_PORT);
		serv_addr.sin_addr.s_addr = *((int *) host->h_addr);
		if(0 > connect(sock, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr))) {
			cout << "failed to connect to server.\n";
			return 3;
		}
	}}}

	basic_string<int32_t> cmd;
	basic_string<int32_t> ret;

	// get initial CAPA:
	ret = receive_blob(sock, 1); // eat initial ERR_SUCCESS
	cout << "initial CAPA: \"" << receive_string(sock) << "\".\n";

	// create a logger:
	cout << "\n";
	int logger_id;
	cmd.clear();
	cmd.push_back(CLCMD_CREATE_OBJECT);
	cmd.push_back(OBJ_LOGGER);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "create object resulted in " << ret[0] << ".\n";

	if(ret[0] == 0) {
		ret = receive_blob(sock, 1);
		logger_id = ret[0];
		cout << "object id " << logger_id << ".\n";
	}

	// call receive&flush on logger
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(logger_id);
	cmd.push_back(LOGGER_RECEIVE_AND_FLUSH);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "receive&flush resulted in " << ret[0] << ".\n";

	if(ret[0] == 0) {
		cout << "logger string: \"" << receive_string(sock) << "\".\n";
	}

	// delete logger:
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_DELETE_OBJECT);
	cmd.push_back(logger_id);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "delete resulted in " << ret[0] << ".\n";

	// disconnect
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_DISCONNECT);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "disconnect resulted in " << ret[0] << ".\n";

	return 0;
}

