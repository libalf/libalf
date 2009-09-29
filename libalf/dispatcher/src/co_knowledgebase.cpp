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

#include "co_knowledgebase.h"
#include "co_knowledgebase_iterator.h"

co_knowledgebase::co_knowledgebase()
{{{
	o = new knowledgebase<extended_bool>;
}}};

co_knowledgebase::co_knowledgebase(knowledgebase<extended_bool> * o)
{{{
	this->o = o;
}}}

co_knowledgebase::~co_knowledgebase()
{{{
	set<int>::iterator si;

	for(si = referring_learning_algorithms.begin(); si != referring_learning_algorithms.end(); si++)
		this->sv->objects[*si]->deref_knowledgebase(this->id);

	for(si = referring_iterators.begin(); si != referring_iterators.end(); si++)
		this->sv->objects[*si]->deref_knowledgebase(this->id);

	if(o) {
		delete o;
	}
}}};

bool co_knowledgebase::handle_command(int command, basic_string<int32_t> & command_data)
{{{
	string s;
	basic_string<int32_t> serial;
	basic_string<int32_t>::iterator si;
	list<int> word;
	extended_bool acceptance;
	int i;
	knowledgebase<extended_bool>::iterator * it;
	knowledgebase<extended_bool>::iterator it2;

	switch(command) {
		case KNOWLEDGEBASE_SERIALIZE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			serial = o->serialize();
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_raw_blob(serial);
		case KNOWLEDGEBASE_DESERIALIZE:
			si = command_data.begin();
			if(!o->deserialize(si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			return this->sv->send_errno(ERR_SUCCESS);
		case KNOWLEDGEBASE_ASSIGN:
			
			return this->sv->send_errno(ERR_NOT_IMPLEMENTED);
		case KNOWLEDGEBASE_TO_DOTFILE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			s = o->generate_dotfile();
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_string(s.c_str());
		case KNOWLEDGEBASE_TO_STRING:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			s = o->tostring();
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_string(s.c_str());
		case KNOWLEDGEBASE_IS_ANSWERED:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(o->is_answered() ? 1 : 0);
		case KNOWLEDGEBASE_IS_EMPTY:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(o->is_empty() ? 1 : 0);
		case KNOWLEDGEBASE_GET_ALPHABET_SIZE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(o->get_alphabet_size());
		case KNOWLEDGEBASE_COUNT_QUERIES:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(o->count_queries());
		case KNOWLEDGEBASE_COUNT_ANSWERS:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(o->count_answers());
		case KNOWLEDGEBASE_COUNT_RESOLVED_QUERIES:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(o->count_resolved_queries());
		case KNOWLEDGEBASE_GET_MEMORY_USAGE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(o->get_memory_usage());
		case KNOWLEDGEBASE_RESOLVE_QUERY:
			si = command_data.begin();
			if(!deserialize_word(word, si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			if(o->resolve_query(word, acceptance)) {
				// known
				if(!this->sv->client->stream_send_int(1))
					return false;
				return this->sv->client->stream_send_int(acceptance);
			} else {
				// unknown
				return this->sv->client->stream_send_int(0);
			}
		case KNOWLEDGEBASE_RESOLVE_OR_ADD_QUERY:
			si = command_data.begin();
			if(!deserialize_word(word, si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			if(o->resolve_or_add_query(word, acceptance)) {
				// known
				if(!this->sv->client->stream_send_int(1))
					return false;
				return this->sv->client->stream_send_int(acceptance);
			} else {
				// unknown
				return this->sv->client->stream_send_int(0);
			}
		case KNOWLEDGEBASE_ADD_KNOWLEDGE:
			si = command_data.begin();
			if(!deserialize_word(word, si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si == command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			acceptance = (int32_t)ntohl(*si);
			if(acceptance != extended_bool::EBOOL_FALSE && acceptance != extended_bool::EBOOL_UNKNOWN && acceptance != extended_bool::EBOOL_TRUE)
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			si++;
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return sv->client->stream_send_int(o->add_knowledge(word, acceptance) ? 1 : 0);
		case KNOWLEDGEBASE_GET_QUERY_TREE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			{
				// get query-tree
				knowledgebase<extended_bool> * qry_tree;
				qry_tree = o->create_query_tree();

				if(!this->sv->send_errno(ERR_SUCCESS))
					return false;

				// create&return a new co_knowledgebase-object
				return this->sv->client->stream_send_int( this->sv->store_object(new co_knowledgebase(qry_tree)) );
			}
		case KNOWLEDGEBASE_MERGE_TREE:
			if(command_data.size() != 1)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			i = ntohl(command_data[0]);
			if(i < 0 || i >= (int)sv->objects.size())
				return this->sv->send_errno(ERR_NO_OBJECT);
			if(this->sv->objects[i]->get_type() != OBJ_KNOWLEDGEBASE)
				return this->sv->send_errno(ERR_BAD_OBJECT);

			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			if(o->merge_knowledgebase( * dynamic_cast<co_knowledgebase*>(this->sv->objects[i])->o ))
				return this->sv->client->stream_send_int(1);
			else
				return this->sv->client->stream_send_int(0);
		case KNOWLEDGEBASE_DESERIALIZE_QUERY_ACCEPTANCES:
			si = command_data.begin();
			if(!o->deserialize_query_acceptances(si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			return this->sv->send_errno(ERR_SUCCESS);
		case KNOWLEDGEBASE_SERIALIZE_QUERIES:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			for(it2 = o->qbegin(); it2 != o->qend(); it2++) {
				word = it2->get_word();
				serial = serialize_word(word);
				if(!this->sv->client->stream_send_raw_blob(serial))
					return false;
			}
			return this->sv->client->stream_send_int(-1);
		case KNOWLEDGEBASE_BEGIN:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);

			it = new knowledgebase<extended_bool>::iterator;
			*it = o->begin();
			i = this->sv->store_object( new co_knowledgebase_iterator(it) );
			this->sv->objects[i]->ref_knowledgebase(this->id);
			this->ref_knowledgebase_iterator(i);

			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(i);
		case KNOWLEDGEBASE_END:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);

			it = new knowledgebase<extended_bool>::iterator;
			*it = o->end();
			i = this->sv->store_object( new co_knowledgebase_iterator(it) );
			this->sv->objects[i]->ref_knowledgebase(this->id);
			this->ref_knowledgebase_iterator(i);

			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(i);
		case KNOWLEDGEBASE_QBEGIN:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);

			it = new knowledgebase<extended_bool>::iterator;
			*it = o->qbegin();
			i = this->sv->store_object( new co_knowledgebase_iterator(it) );
			this->sv->objects[i]->ref_knowledgebase(this->id);
			this->ref_knowledgebase_iterator(i);

			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(i);
		case KNOWLEDGEBASE_QEND:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);

			it = new knowledgebase<extended_bool>::iterator;
			*it = o->qend();
			i = this->sv->store_object( new co_knowledgebase_iterator(it) );
			this->sv->objects[i]->ref_knowledgebase(this->id);
			this->ref_knowledgebase_iterator(i);

			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(i);
		case KNOWLEDGEBASE_CLEAR:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			o->clear();
			return this->sv->send_errno(ERR_SUCCESS);
		case KNOWLEDGEBASE_CLEAR_QUERIES:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			o->clear_queries();
			return this->sv->send_errno(ERR_SUCCESS);
		case KNOWLEDGEBASE_UNDO:
			if(command_data.size() != 1)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			i = ntohl(command_data[0]);
			if(i < 1)
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int(o->undo(i) ? 1 : 0);
		default:
			return this->sv->send_errno(ERR_BAD_COMMAND);
	}

	return false;
}}};

void co_knowledgebase::ref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.insert(oid);
}}};

void co_knowledgebase::deref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.erase(oid);
}}};

void co_knowledgebase::ref_knowledgebase_iterator(int oid)
{{{
	referring_iterators.insert(oid);
}}};

void co_knowledgebase::deref_knowledgebase_iterator(int oid)
{{{
	referring_iterators.erase(oid);
}}};

