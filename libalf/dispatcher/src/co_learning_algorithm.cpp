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

#include <libalf/learning_algorithm.h>

#include <libalf/algorithm_angluin.h>
//#include <libalf/KVtree.h>
#include <libalf/algorithm_biermann_minisat.h>
//#include BIERMANN_ANGLUIN
#include <libalf/algorithm_NLstar.h>
#include <libalf/algorithm_RPNI.h>
#include <libalf/algorithm_DeLeTe2.h>

#include "co_learning_algorithm.h"

using namespace std;
using namespace libalf;

co_learning_algorithm::co_learning_algorithm(enum libalf::learning_algorithm<extended_bool>::algorithm alg, int alphabet_size)
{{{
	referenced_knowledgebase = -1;
	referenced_logger = -1;
	referenced_normalizer = -1;

	switch(alg) {
		default:
			this->sv->clog("somehow bad request reached co_learning_algorithm constructor (bad type of learning algorithm: %d). killing client.\n", (int)alg);
			exit(-1);

		case learning_algorithm<extended_bool>::ALG_ANGLUIN:
			o = new angluin_simple_table<extended_bool>(NULL, NULL, alphabet_size);
			break;
		case learning_algorithm<extended_bool>::ALG_ANGLUIN_COLUMN:
			o = new angluin_col_table<extended_bool>(NULL, NULL, alphabet_size);
			break;
//		case learning_algorithm<extended_bool>::ALG_KVTREE:
//			o = new KVtree<extended_bool>(NULL, NULL, alphabet_size);
//			break;
		case learning_algorithm<extended_bool>::ALG_BIERMANN:
			o = new MiniSat_biermann<extended_bool>(NULL, NULL, alphabet_size);
			break;
//		case learning_algorithm<extended_bool>::ALG_BIERMANN_ANGLUIN:
//			o = new algorithm_biermann_angluin<extended_bool>(NULL, NULL, alphabet_size);
//			break;
		case learning_algorithm<extended_bool>::ALG_NL_STAR:
			o = new NLstar_table<extended_bool>(NULL, NULL, alphabet_size);
			break;
		case learning_algorithm<extended_bool>::ALG_RPNI:
			o = new RPNI<extended_bool>(NULL, NULL, alphabet_size);
			break;
		case learning_algorithm<extended_bool>::ALG_DELETE2:
			o = new DeLeTe2<extended_bool>(NULL, NULL, alphabet_size);
			break;
	}
}}};

co_learning_algorithm::~co_learning_algorithm()
{{{
	if(referenced_knowledgebase != -1)
		this->sv->objects[referenced_knowledgebase]->deref_learning_algorithm(this->id);
	if(referenced_logger != -1)
		this->sv->objects[referenced_logger]->deref_learning_algorithm(this->id);
	if(referenced_normalizer != -1)
		this->sv->objects[referenced_normalizer]->deref_learning_algorithm(this->id);

	if(o) {
		delete o;
	}
}}};

bool co_learning_algorithm::handle_command(int command, basic_string<int32_t> & command_data)
{
	basic_string<int32_t> serial;
	basic_string<int32_t>::iterator si;
	string s;

	switch(command) {
		case LEARNING_ALGORITHM_SERIALIZE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			serial = o->serialize();
			return this->sv->client->stream_send_raw_blob(serial);
		case LEARNING_ALGORITHM_DESERIALIZE:
			si = command_data.begin();
			if(!o->deserialize(si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			return this->sv->send_errno(ERR_SUCCESS);
		case LEARNING_ALGORITHM_TO_STRING:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			s = o->tostring();
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_string(s.c_str());
		case LEARNING_ALGORITHM_DESERIALIZE_MAGIC:
			si = command_data.begin();
			if(!o->deserialize_magic(si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			return this->sv->send_errno(ERR_SUCCESS);
		case LEARNING_ALGORITHM_ASSOCIATE_LOGGER:
		case LEARNING_ALGORITHM_REMOVE_LOGGER:
		case LEARNING_ALGORITHM_SET_KNOWLEDGE_SOURCE:
		case LEARNING_ALGORITHM_GET_KNOWLEDGE_SOURCE:
		case LEARNING_ALGORITHM_SET_NORMALIZER:
		case LEARNING_ALGORITHM_GET_NORMALIZER:
		case LEARNING_ALGORITHM_UNSET_NORMALIZER:
		case LEARNING_ALGORITHM_GET_MEMORY_STATISTICS:
		case LEARNING_ALGORITHM_GET_TIMING_STATISTICS:
		case LEARNING_ALGORITHM_ENABLE_TIMING:
		case LEARNING_ALGORITHM_DISABLE_TIMING:
		case LEARNING_ALGORITHM_RESET_TIMING:
		case LEARNING_ALGORITHM_GET_ALPHABET_SIZE:
		case LEARNING_ALGORITHM_INCREASE_ALPHABET_SIZE:
		case LEARNING_ALGORITHM_CONJECTURE_READY:
		case LEARNING_ALGORITHM_ADVANCE:
		case LEARNING_ALGORITHM_ADD_COUNTEREXAMPLE:
		case LEARNING_ALGORITHM_SUPPORTS_SYNC:
		case LEARNING_ALGORITHM_SYNC_TO_KNOWLEDGEBASE:
			return this->sv->send_errno(ERR_NOT_IMPLEMENTED);
		default:
			return this->sv->send_errno(ERR_BAD_COMMAND);
	}

	return false;
};

void co_learning_algorithm::ref_knowledgebase(int oid)
{{{
	referenced_knowledgebase = oid;
}}};

void co_learning_algorithm::deref_knowledgebase(int oid)
{{{
	referenced_knowledgebase = -1;
	if(o)
		o->set_knowledge_source(NULL);
}}};

void co_learning_algorithm::ref_logger(int oid)
{{{
	referenced_logger = oid;
}}};

void co_learning_algorithm::deref_logger(int oid)
{{{
	referenced_logger = -1;
	if(o)
		o->set_logger(NULL);
}}};

void co_learning_algorithm::ref_normalizer(int oid)
{{{
	referenced_normalizer = oid;
}}};

void co_learning_algorithm::deref_normalizer(int oid)
{{{
	referenced_normalizer = -1;
	if(o)
		o->unset_normalizer();
}}};

