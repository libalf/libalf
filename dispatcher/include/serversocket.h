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

#ifndef __libalf_serversocket_h__
# define __libalf_serversocket_h__

#include <string>

class serversocket {
	protected:
		int sock;


	public:
		serversocket();
		serversocket(int socketid);
		~serversocket();

		bool alive();
		void close();

		bool bind(std::string & listen_address, uint16_t listen_port);
		bool listen(int queue_length);
		serversocket *accept();



		int stream_send(void *msg, int length);
		int stream_receive(void *msg, int length);
		bool stream_get_int(int32_t & ret);
};

#endif // __libalf_serversocket_h__

