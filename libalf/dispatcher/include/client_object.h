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

#ifndef __libalf_dispatcher_client_object_h__
# define __libalf_dispatcher_client_object_h__

#include <sys/types.h>
#include <unistd.h>

#include <string>

#include <libalf/knowledgebase.h>
#include <libalf/answer.h>
#include <libalf/learning_algorithm.h>
#include <libalf/learning_algorithm.h>
#include <libalf/normalizer.h>

#include "main.h"
#include "servant.h"
#include "protocol.h"

using namespace libalf;

class servant;

class client_object {
	protected:
		servant * sv;
		int id;

	public:
		client_object()
		{ sv = NULL; id = -1; };

		virtual ~client_object()
		{ };

		void set_servant(servant * sv)
		{ this->sv = sv; };

		void set_id(int id)
		{ this->id = id; };

		int get_id()
		{ return this->id; };

		virtual enum object_type get_type()
		{ return OBJ_NONE; };

		virtual bool handle_command(int command, basic_string<int32_t> & command_data) = 0;

		virtual void ref_knowledgebase(int oid)
		{ log("client %d: BAD CALL to ref kb (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_knowledgebase(int oid)
		{ log("client %d: BAD CALL to deref kb (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };

		virtual void ref_knowledgebase_iterator(int oid)
		{ log("client %d: BAD CALL to ref ki (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_knowledgebase_iterator(int oid)
		{ log("client %d: BAD CALL to deref ki (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };

		virtual void ref_logger(int oid)
		{ log("client %d: BAD CALL to ref l (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_logger(int oid)
		{ log("client %d: BAD CALL to deref l (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };

		virtual void ref_normalizer(int oid)
		{ log("client %d: BAD CALL to ref no (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_normalizer(int oid)
		{ log("client %d: BAD CALL to deref no (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };

		virtual void ref_learning_algorithm(int oid)
		{ log("client %d: BAD CALL to ref la (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_learning_algorithm(int oid)
		{ log("client %d: BAD CALL to deref la (oid %d, type %s[%d])!\n", getpid(), id, obj2string(this->get_type()),this->get_type()); };

		virtual int get_reference_count()
		{
			log("client %d: BAD CALL to get_reference_count (oid %d, type %s[%d])!\n",
					getpid(), id, obj2string(this->get_type()),this->get_type());
			return 0;
		};

		virtual int get_depending_reference_count()
		{
			log("client %d: BAD CALL to get_depending_reference_count (oid %d, type %s[%d])!\n",
					getpid(), id, obj2string(this->get_type()),this->get_type());
			return 0;
		};

};

#endif // __libalf_dispatcher_client_object_h__

