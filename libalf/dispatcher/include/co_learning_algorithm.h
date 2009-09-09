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

class co_learning_algorithm : public client_object {
	private:
		learning_algorithm<extended_bool> * o;

		int referenced_knowledgebase;
		int referenced_logger;
		int referenced_normalizer;
	public:
		// alg MUST be valid.
		co_learning_algorithm(enum libalf::learning_algorithm<extended_bool>::algorithm alg, int alphabet_size);

		virtual ~co_learning_algorithm();

		virtual enum object_type get_type()
		{ return OBJ_LEARNING_ALGORITHM; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data);

		virtual void ref_knowledgebase(int oid);
		virtual void deref_knowledgebase(int oid);

		virtual void ref_logger(int oid);
		virtual void deref_logger(int oid);

		virtual void ref_normalizer(int oid);
		virtual void deref_normalizer(int oid);

		virtual int get_reference_count()
		{
			int refs = 0;
			if(referenced_knowledgebase != -1)
				refs++;
			if(referenced_logger != -1)
				refs++;
			if(referenced_normalizer != -1)
				refs++;
			return refs;
		}
};

#endif // __libalf_dispatcher_co_algorithm_h__

