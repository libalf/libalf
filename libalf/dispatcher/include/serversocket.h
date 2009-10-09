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
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 * (c) by Stefan Rieger, i2 Informatik RWTH-Aachen
 *
 * see LICENSE file for licensing information.
 */

#ifndef __libalf_dispatcher_serversocket_h__
# define __libalf_dispatcher_serversocket_h__

#include <sys/types.h>
#include <arpa/inet.h>

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
		serversocket *accept(struct sockaddr_in & remote_addr, socklen_t & sin_size);

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

