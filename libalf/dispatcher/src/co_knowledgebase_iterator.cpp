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

#include "co_knowledgebase_iterator.h"

co_knowledgebase_iterator::co_knowledgebase_iterator()
{{{
	referenced_knowledgebase = -1;

	o = new knowledgebase<extended_bool>::iterator;
}}};

co_knowledgebase_iterator::co_knowledgebase_iterator(knowledgebase<extended_bool>::iterator * o)
{{{
	referenced_knowledgebase = -1;

	this->o = o;
}}}

co_knowledgebase_iterator::~co_knowledgebase_iterator()
{{{
	if(referenced_knowledgebase != -1)
		this->sv->objects[referenced_knowledgebase]->deref_knowledgebase_iterator(this->id);

	if(o) {
		delete o;
	}
}}};

bool co_knowledgebase_iterator::handle_command(int command, basic_string<int32_t> & command_data)
{{{
	int i;

	switch(command) {
		case KITERATOR_IS_VALID:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int( o->is_valid() ? 1 : 0 );
		case KITERATOR_ASSIGN:
			// don't check for iterator validity, kb->end() is invalid.
			if(command_data.size() != 1)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			i = ntohl(command_data[0]);
			if(i < 0 || i >= (int)sv->objects.size())
				return this->sv->send_errno(ERR_NO_OBJECT);
			if(this->sv->objects[i]->get_type() != OBJ_KNOWLEDGEBASE_ITERATOR)
				return this->sv->send_errno(ERR_BAD_OBJECT);

			*o = *(dynamic_cast<co_knowledgebase_iterator *>(this->sv->objects[i])->o);

			return this->sv->send_errno(ERR_SUCCESS);
		case KITERATOR_COMPARE:
			// don't check for iterator validity, kb->end() is invalid.
			if(command_data.size() != 1)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			i = ntohl(command_data[0]);
			if(i < 0 || i >= (int)sv->objects.size())
				return this->sv->send_errno(ERR_NO_OBJECT);
			if(this->sv->objects[i]->get_type() != OBJ_KNOWLEDGEBASE_ITERATOR)
				return this->sv->send_errno(ERR_BAD_OBJECT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			if(*o == *(dynamic_cast<co_knowledgebase_iterator *>(this->sv->objects[i])->o))
				this->sv->client->stream_send_int(1);
			else
				this->sv->client->stream_send_int(0);
		case KITERATOR_IS_ANSWERED:
			if(!o->is_valid())
				return this->sv->send_errno(ERR_BAD_OBJECT_STATE);
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int((*o)->is_answered() ? 1 : 0);
		case KITERATOR_IS_REQUIRED:
			
			return this->sv->send_errno(ERR_NOT_IMPLEMENTED);
		case KITERATOR_GET_WORD:
			if(!o->is_valid()) {
				return this->sv->send_errno(ERR_BAD_OBJECT_STATE);
			} else {
				if(command_data.size() != 0)
					return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
				list<int> word = (*o)->get_word();
				basic_string<int32_t> serial = serialize_word(word);
				if(!this->sv->send_errno(ERR_SUCCESS))
					return false;
				return this->sv->client->stream_send_raw_blob(serial);
			}
		case KITERATOR_GET_ANSWER:
			if(!o->is_valid())
				return this->sv->send_errno(ERR_BAD_OBJECT_STATE);
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			if((*o)->is_answered()) {
				extended_bool a;
				if(!this->sv->client->stream_send_int(1))
					return false;
				a = (*o)->get_answer();
				return this->sv->client->stream_send_int( (int32_t) a);
			} else {
				return this->sv->client->stream_send_int(0);
			}
		case KITERATOR_NEXT:
			if(!o->is_valid())
				return this->sv->send_errno(ERR_BAD_OBJECT_STATE);
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			++(*o);
			return this->sv->send_errno(ERR_SUCCESS);
		case KITERATOR_ANSWER:
			if(!o->is_valid()) {
				return this->sv->send_errno(ERR_BAD_OBJECT_STATE);
			} else {
				if(command_data.size() != 1)
					return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);

				extended_bool a;

				i = ntohl(command_data[0]);
				if(i < 0 || i > 2)
					return this->sv->send_errno(ERR_BAD_PARAMETERS);
				a = (int32_t)i;
				if(!this->sv->send_errno(ERR_SUCCESS))
					return false;
				return this->sv->client->stream_send_int((*o)->set_answer(a) ? 1 : 0);
			}
		default:
			return this->sv->send_errno(ERR_BAD_COMMAND);
	}

	return false;
}}};

void co_knowledgebase_iterator::ref_knowledgebase(int oid)
{{{
	referenced_knowledgebase = oid;
}}};

void co_knowledgebase_iterator::deref_knowledgebase(int oid)
{{{
	referenced_knowledgebase = -1;
	invalidate();
}}};

bool co_knowledgebase_iterator::is_valid()
{{{
	if(!o)
		return false;
	return o->is_valid();
}}};

void co_knowledgebase_iterator::invalidate()
{{{
	delete o;
	o = new knowledgebase<extended_bool>::iterator;
}}};

