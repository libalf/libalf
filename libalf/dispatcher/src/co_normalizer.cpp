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

#include <libalf/normalizer.h>
#include <libalf/normalizer_msc.h>

#include "co_normalizer.h"

using namespace libalf;

co_normalizer::co_normalizer(enum libalf::normalizer::type type)
{{{
	switch(type) {
		default:
			this->sv->clog("somehow bad request reached co_normalizer constructor (bad type of normalizer: %d). killing client.\n", (int)type);
			exit(-1);

		case normalizer::NORMALIZER_MSC:
			o = new normalizer_msc;
			break;
	}
}}};

co_normalizer::~co_normalizer()
{{{
	set<int>::iterator si;

	for(si = referring_learning_algorithms.begin(); si != referring_learning_algorithms.end(); si++)
		this->sv->objects[*si]->deref_normalizer(this->id);

	if(o) {
		delete o;
	}
}}};

bool co_normalizer::handle_command(int command, basic_string<int32_t> & command_data)
{
	basic_string<int32_t> serial;
	basic_string<int32_t>::iterator si;

	switch(command) {
		case NORMALIZER_SERIALIZE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			serial = o->serialize();
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_raw_blob(serial);
		case NORMALIZER_DESERIALIZE:
			si = command_data.begin();
			if(!o->deserialize(si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			return this->sv->send_errno(ERR_SUCCESS);
		case NORMALIZER_DESERIALIZE_EXTENSION:
			si = command_data.begin();
			if(!o->deserialize_extension(si, command_data.end()))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(si != command_data.end())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			return this->sv->send_errno(ERR_SUCCESS);
		case NORMALIZER_GET_TYPE:
		case NORMALIZER_NORMALIZE_A_WORD_PNF:
		case NORMALIZER_NORMALIZE_A_WORD_SNF:
			return this->sv->send_errno(ERR_NOT_IMPLEMENTED);
		default:
			return this->sv->send_errno(ERR_BAD_COMMAND);
	}

	return false;
};

void co_normalizer::ref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.insert(oid);
}}};

void co_normalizer::deref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.erase(oid);
}}};

