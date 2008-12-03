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

#ifndef __libalf_network_connection_h__
# define __libalf_network_connection_h__

namespace libalf {

using namespace std;

class network_connection {
	public:
		virtual ~network_connection() = { };

		virtual connect(char * host, unsigned int port);
		virtual listen(unsigned int port);

	protected:
		virtual send(char *buf, unsigned int size);
		virtual recv(char *buf, unsigned int size);
}

}; // end namespace libalf

#endif // __libalf_network_connection_h__

