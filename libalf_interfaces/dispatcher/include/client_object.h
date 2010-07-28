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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
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

		virtual void ref_knowledgebase(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to ref kb (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_knowledgebase(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to deref kb (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };

		virtual void ref_knowledgebase_iterator(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to ref ki (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_knowledgebase_iterator(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to deref ki (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };

		virtual void ref_logger(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to ref l (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_logger(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to deref l (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };

		virtual void ref_normalizer(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to ref no (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_normalizer(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to deref no (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };

		virtual void ref_learning_algorithm(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to ref la (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };
		virtual void deref_learning_algorithm(int __attribute__ ((__unused__)) oid)
		{ log("client %d: BAD CALL to deref la (oid %d, type %s[%d])!\n",
				getpid(), id, obj2string(this->get_type()),this->get_type()); };

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

