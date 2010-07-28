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

#include <libalf/answer.h>
#include <libalf/serialize.h>
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
{{{
	basic_string<int32_t> serial;
	serial_stretch ser(serial);

	list<int> word, nword;
	bool bottom;

	switch(command) {
		case NORMALIZER_SERIALIZE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			serial = o->serialize();
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_raw_blob(serial);
		case NORMALIZER_DESERIALIZE:
			if(!o->deserialize(ser))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(!ser.empty())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			return this->sv->send_errno(ERR_SUCCESS);
		case NORMALIZER_DESERIALIZE_EXTENSION:
			if(!o->deserialize_extension(ser))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(!ser.empty())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			return this->sv->send_errno(ERR_SUCCESS);
		case NORMALIZER_GET_TYPE:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			return this->sv->client->stream_send_int((int)o->get_type());
		case NORMALIZER_NORMALIZE_A_WORD_PNF:
			if(!::deserialize(word, ser))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(!ser.empty())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			nword = o->prefix_normal_form(word, bottom);
			if(bottom) {
				return this->sv->client->stream_send_int(1);
			} else {
				if(!this->sv->client->stream_send_int(0))
					return false;
				serial = ::serialize(word);
				return this->sv->client->stream_send_raw_blob(serial);
			}
		case NORMALIZER_NORMALIZE_A_WORD_SNF:
			if(!::deserialize(word, ser))
				return this->sv->send_errno(ERR_BAD_PARAMETERS);
			if(!serial.empty())
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			if(!this->sv->send_errno(ERR_SUCCESS))
				return false;
			nword = o->suffix_normal_form(word, bottom);
			if(bottom) {
				return this->sv->client->stream_send_int(1);
			} else {
				if(!this->sv->client->stream_send_int(0))
					return false;
				serial = serialize_word(nword);
				return this->sv->client->stream_send_raw_blob(serial);
			}
		default:
			return this->sv->send_errno(ERR_BAD_COMMAND);
	}

	return false;
}}};

void co_normalizer::ref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.insert(oid);
}}};

void co_normalizer::deref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.erase(oid);
}}};

