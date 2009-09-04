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

#ifndef __libalf_servant_h__
# define __libalf_servant_h__

#include <vector>

#include <libalf/logger.h>
#include <libalf/knowledgebase.h>
#include <libalf/answer.h>
#include <libalf/learning_algorithm.h>
#include <libalf/learning_algorithm.h>
#include <libalf/normalizer.h>

#include "serversocket.h"
#include "protocol.h"

using namespace std;
using namespace libalf;

struct object {
	enum object_type type;

	union {
		logger * o_logger;
		knowledgebase<extended_bool> * o_knowledgebase;
		knowledgebase<extended_bool>::iterator * o_kiterator;
		learning_algorithm<extended_bool> * o_algorithm;
		normalizer * o_normalizer;
	};
};

class servant {
	private:
		string capa;
		serversocket * client;
		bool capa_sent;
		vector<object> objects;

	public:
		servant(serversocket *connection);
		~servant();

		bool serve();

	private:
		bool send_capabilities();
		bool reply_version();
		bool reply_hello_carsten();

};

#endif // __libalf_servant_h__

