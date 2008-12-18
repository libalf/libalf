// vim: fdm=marker

#include <string>
#include <iostream>
#include <iterator>

#include <poll.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

void print_blob(basic_string<int32_t> blob)
{{{
	basic_string<int32_t>::iterator bi;

	for(bi = blob.begin(); bi != blob.end(); bi++) {
		cout << ntohl(*bi);
		cout << ";";
	}
	cout << "\n";
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

int main()
{
	int sock;

	// connect socket to server
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
	serv_addr.sin_port = htons(23005);
	serv_addr.sin_addr.s_addr = *((int *) host->h_addr);
	if(0 > connect(sock, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr))) {
		cout << "failed to connect to server.\n";
		return 3;
	}

	basic_string<int32_t> data;
	// receive CAPA
	data = receive_blob(sock, 3);
	cout << "CAPA\n";
	print_blob(data);

	data.clear();
	data += 20; // REQ SESSION
	data += 1; // angluin
	data += 5; // alphabet size
	send_blob(sock, data);

	data = receive_blob(sock, 2);
	cout << "session " << ntohl(data[1]) << " answer " << ntohl(data[0]) << ".\n";
	print_blob(data);

	data.clear();
	data += 25; // ADVANCE
	data += 0; // session id
	send_blob(sock, data);

	data = receive_blob(sock, 2);
	cout << "answer " << ntohl(data[0]) << ".\n";
	print_blob(data);
	data = receive_blob(sock, ntohl(data[1]));
	print_blob(data);

	data.clear();
	data += 0; // DISCONNECT
	send_blob(sock, data);
}
