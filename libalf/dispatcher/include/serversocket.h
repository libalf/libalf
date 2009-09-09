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

#ifndef __libalf_dispatcher_serversocket_h__
# define __libalf_dispatcher_serversocket_h__

#include <string>

using namespace std;

class serversocket {
	public:
		int sock;

		serversocket();
		serversocket(int socketid);
		~serversocket();

		bool alive();
		void close();

		bool bind(std::string & listen_address, uint16_t listen_port);
		bool listen(int queue_length);
		serversocket *accept();

		// sending of raw data:
		int stream_send(const void *msg, int length);
		int stream_receive(void *msg, int length);

		// will do htonl:
		bool stream_receive_int(int32_t & ret);
		bool stream_send_int(int32_t val);
		// will do no htonl:
		bool stream_receive_raw_int(int32_t & ret);
		bool stream_send_raw_int(int32_t val);

		// will send the string without byte-order conversion:
		bool stream_receive_raw_blob(basic_string<int32_t> & blob, int length);
		bool stream_send_raw_blob(basic_string<int32_t> & blob);

		// send string with initial length field (in network byte order)
		bool stream_send_string(const char * str);

};

#endif // __libalf_dispatcher_serversocket_h__

