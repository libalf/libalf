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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
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

#include <libalf/learning_algorithm.h>
//#include <libalf/normalizer.h>

using namespace std;


#define C_NOTE "\x1b[;0;44m"
#define C_RESET "\x1b[m"


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

	// get version
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_REQ_VERSION);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "request version resulted in " << ret[0] << ".\n";

	if(ret[0] == 0) {
		cout << "version string: {\n" C_NOTE << receive_string(sock) << C_RESET "}.\n";
	}

	// create a logger:
	cout << "\n";
	int logger_id;
	cmd.clear();
	cmd.push_back(CLCMD_CREATE_OBJECT);
	cmd.push_back(OBJ_LOGGER);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "create L object resulted in " << ret[0] << ".\n";

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

	if(ret[0] == 0) {
		cout << "logger string: {\n" C_NOTE << receive_string(sock) << C_RESET "}.\n";
	} else {
		cout << "LOGGER FAILED!\n";
	}

	// create a knowledgebase:
	cout << "\n";
	int kb_id;
	cmd.clear();
	cmd.push_back(CLCMD_CREATE_OBJECT);
	cmd.push_back(OBJ_KNOWLEDGEBASE);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "create KB object resulted in " << ret[0] << ".\n";

	if(ret[0] == 0) {
		ret = receive_blob(sock, 1);
		kb_id = ret[0];
		cout << "object id " << kb_id << ".\n";
	}

	// serialize knowledgebase
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(kb_id);
	cmd.push_back(KNOWLEDGEBASE_SERIALIZE);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "serialize KB object resulted in " << ret[0] << ".\n";

	if(ret[0] == 0) {
		ret = receive_blob(sock,1);
		cout << "kb is " << ret[0] << " long:\n";
		ret = receive_blob(sock,ret[0]);
		print_blob(ret);
	}

	// create learning_algorithm
	int alg_id;
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_CREATE_OBJECT);
	cmd.push_back(OBJ_LEARNING_ALGORITHM);
	cmd.push_back(2);
	cmd.push_back(libalf::learning_algorithm<bool>::ALG_NL_STAR);
	cmd.push_back(2);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "create ALG object resulted in " << ret[0] << ".\n";

	if(ret[0] == 0) {
		ret = receive_blob(sock, 1);
		alg_id = ret[0];
		cout << "object id " << alg_id << ".\n";
	}

	// associate logger with alg
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(alg_id);
	cmd.push_back(LEARNING_ALGORITHM_ASSOCIATE_LOGGER);
	cmd.push_back(1);
	cmd.push_back(logger_id);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "associate logger&alg: " << ret[0] << ".\n";

	// associate knowledgebase with alg
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(alg_id);
	cmd.push_back(LEARNING_ALGORITHM_SET_KNOWLEDGE_SOURCE);
	cmd.push_back(1);
	cmd.push_back(kb_id);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "associate kb&alg: " << ret[0] << ".\n";

	// call receive&flush on logger
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(logger_id);
	cmd.push_back(LOGGER_RECEIVE_AND_FLUSH);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);

	if(ret[0] == 0) {
		cout << "logger string: {\n" C_NOTE << receive_string(sock) << C_RESET "}.\n";
	} else {
		cout << "LOGGER FAILED!\n";
	}




	// advance algorithm
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(alg_id);
	cmd.push_back(LEARNING_ALGORITHM_ADVANCE);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "advance: " << ret[0] << ".\n";
	if(ret[0] == ERR_SUCCESS) {
		ret = receive_blob(sock, 1);
		cout << "A: " << ret[0] << "\n";
	}

	// count required
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(kb_id);
	cmd.push_back(KNOWLEDGEBASE_COUNT_QUERIES);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "count requires: " << ret[0] << ".\n";
	if(ret[0] == ERR_SUCCESS) {
		ret = receive_blob(sock, 1);
		cout << "required: "<<ret[0]<<"\n";
	}

	// answer queries (all positive)
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(kb_id);
	cmd.push_back(KNOWLEDGEBASE_DESERIALIZE_QUERY_ACCEPTANCES);
	cmd.push_back(2); // para size
	cmd.push_back(1); // answer size
	cmd.push_back(2); // 2 = true; 0 = false;
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "answer queries: " << ret[0] << ".\n";




	// advance algorithm
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(alg_id);
	cmd.push_back(LEARNING_ALGORITHM_ADVANCE);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "advance: " << ret[0] << ".\n";
	if(ret[0] == ERR_SUCCESS) {
		ret = receive_blob(sock, 1);
		cout << "A: " << ret[0] << "\n";
	}

	// count required
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(kb_id);
	cmd.push_back(KNOWLEDGEBASE_COUNT_QUERIES);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "count requires: " << ret[0] << ".\n";
	if(ret[0] == ERR_SUCCESS) {
		ret = receive_blob(sock, 1);
		cout << "required: "<<ret[0]<<"\n";
	}

	// answer queries (all positive)
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(kb_id);
	cmd.push_back(KNOWLEDGEBASE_DESERIALIZE_QUERY_ACCEPTANCES);
	cmd.push_back(3); // para size
	cmd.push_back(2); // answer size
	cmd.push_back(0); // 2 = true; 0 = false;
	cmd.push_back(2); // 2 = true; 0 = false;
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "answer queries: " << ret[0] << ".\n";







	// advance algorithm
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(alg_id);
	cmd.push_back(LEARNING_ALGORITHM_ADVANCE);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "advance: " << ret[0] << ".\n";
	if(ret[0] == ERR_SUCCESS) {
		ret = receive_blob(sock, 1);
		cout << "A: " << ret[0] << "\n";
	}

	// count required
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(kb_id);
	cmd.push_back(KNOWLEDGEBASE_COUNT_QUERIES);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "count requires: " << ret[0] << ".\n";
	if(ret[0] == ERR_SUCCESS) {
		ret = receive_blob(sock, 1);
		cout << "required: "<<ret[0]<<"\n";
	}

	// answer queries (all positive)
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(kb_id);
	cmd.push_back(KNOWLEDGEBASE_DESERIALIZE_QUERY_ACCEPTANCES);
	cmd.push_back(3); // para size
	cmd.push_back(2); // answer size
	cmd.push_back(0); // 2 = true; 0 = false;
	cmd.push_back(0); // 2 = true; 0 = false;
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "answer queries: " << ret[0] << ".\n";





	// advance algorithm
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(alg_id);
	cmd.push_back(LEARNING_ALGORITHM_ADVANCE);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "advance: " << ret[0] << ".\n";
	if(ret[0] == ERR_SUCCESS) {
		int size;
		ret = receive_blob(sock, 1);
		cout << "A: " << ret[0] << "\n";
		ret = receive_blob(sock, 1);
		size = ret[0];
		ret = receive_blob(sock, size);
		ret = size + ret;
		cout << " {\n  ";
		print_blob(ret);
		cout << " }\n";
	}












	// say hello to carsten
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_HELLO_CARSTEN);
	cmd.push_back(23);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "hello carsten (23) resulted in " << ret[0] << ".\n";
	if(ret[0] == 0) {
		ret = receive_blob(sock, 1);
		cout << "and replied " << ret[0] << ".\n";
	}

	// resolve membership-info for .
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(kb_id);
	cmd.push_back(KNOWLEDGEBASE_RESOLVE_QUERY);
	cmd.push_back(1); // para size
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "resolve . resulted in " << ret[0] << ".\n";
	if(ret[0] == 0) {
		ret = receive_blob(sock, 1);
		if(ret[0]) {
			cout << "known: ";
			ret = receive_blob(sock, 1);
			cout << ret[0] << ".\n";
		} else {
			cout << "unknown.\n";
		}
	};

	// call receive&flush on logger
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_OBJECT_COMMAND);
	cmd.push_back(logger_id);
	cmd.push_back(LOGGER_RECEIVE_AND_FLUSH);
	cmd.push_back(0);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);

	if(ret[0] == 0) {
		cout << "logger string: {\n" C_NOTE << receive_string(sock) << C_RESET "}.\n";
	} else {
		cout << "LOGGER FAILED!\n";
	}

	// delete algorithm:
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_DELETE_OBJECT);
	cmd.push_back(alg_id);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "delete ALG resulted in " << ret[0] << ".\n";

	// delete knowledgebase:
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_DELETE_OBJECT);
	cmd.push_back(kb_id);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "delete KB resulted in " << ret[0] << ".\n";

	// delete logger:
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_DELETE_OBJECT);
	cmd.push_back(logger_id);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "delete logger resulted in " << ret[0] << ".\n";

	// disconnect
	cout << "\n";
	cmd.clear();
	cmd.push_back(CLCMD_DISCONNECT);
	send_blob(sock, cmd);

	ret = receive_blob(sock, 1);
	cout << "disconnect resulted in " << ret[0] << ".\n";

	return 0;
}

