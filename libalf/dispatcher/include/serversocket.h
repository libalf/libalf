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


		int stream_send(const void *msg, int length);
		int stream_receive(void *msg, int length);

		bool stream_receive_int(int32_t & ret);
		bool stream_receive_blob(basic_string<int32_t> & blob, int length);

		bool stream_send_int(int32_t val);
		bool stream_send_string(const char * str);
		bool stream_send_blob(basic_string<int32_t> & blob);
};

#endif // __libalf_dispatcher_serversocket_h__

