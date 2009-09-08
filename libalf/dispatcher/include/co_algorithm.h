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

#ifndef __libalf_dispatcher_co_algorithm_h__
# define __libalf_dispatcher_co_algorithm_h__

#include <libalf/learning_algorithm.h>

#include "client_object.h"

using namespace libalf;

class co_algorithm : public client_object {
	private:
		learning_algorithm<extended_bool> * o;

	public:
		// alg MUST be valid.
		co_algorithm(enum libalf::learning_algorithm<extended_bool>::algorithm alg);

		virtual ~co_algorithm();

		virtual enum object_type get_type()
		{ return OBJ_ALGORITHM; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data);
};

#endif // __libalf_dispatcher_co_algorithm_h__

