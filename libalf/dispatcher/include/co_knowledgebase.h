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

#ifndef __libalf_dispatcher_co_knowledgebase_h__
# define __libalf_dispatcher_co_knowledgebase_h__

#include <set>

#include <libalf/knowledgebase.h>
#include <libalf/answer.h>

#include "client_object.h"

class co_knowledgebase;

#include "co_learning_algorithm.h"

using namespace libalf;

class co_knowledgebase : public client_object {
	private:
		co_knowledgebase(knowledgebase<extended_bool> * base);

	public:
		knowledgebase<extended_bool> * o;

		set<int> referring_learning_algorithms;
		set<int> referring_iterators;
	public:
		co_knowledgebase();

		virtual ~co_knowledgebase();

		virtual enum object_type get_type()
		{ return OBJ_KNOWLEDGEBASE; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data);

		virtual void ref_learning_algorithm(int oid);
		virtual void deref_learning_algorithm(int oid);

		virtual void ref_knowledgebase_iterator(int oid);
		virtual void deref_knowledgebase_iterator(int oid);

		virtual int get_reference_count()
		{
			int refs = 0;
			refs += referring_learning_algorithms.size();
			refs += referring_iterators.size();
			return refs;
		}
		virtual int get_depending_reference_count()
		{ return get_reference_count(); };
};

#endif // __libalf_dispatcher_co_knowledgebase_h__

