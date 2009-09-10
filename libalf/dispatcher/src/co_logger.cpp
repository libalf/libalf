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

#include "co_logger.h"

co_logger::co_logger()
{{{
	o = new buffered_logger;
}}};

co_logger::~co_logger()
{{{
	set<int>::iterator si;

	for(si = referring_learning_algorithms.begin(); si != referring_learning_algorithms.end(); si++)
		this->sv->objects[*si]->deref_logger(this->id);

	if(o) {
		delete o;
	}
}}};

bool co_logger::handle_command(int command, basic_string<int32_t> & command_data)
{{{
	string *s;
	int level;

	switch(command) {
		case LOGGER_RECEIVE_AND_FLUSH:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);

			s = o->receive_and_flush();

			if(!this->sv->send_errno(ERR_SUCCESS)) {
				delete s;
				return false;
			}

			if(!this->sv->client->stream_send_string(s->c_str())) {
				delete s;
				return false;
			}

			delete s;
			return true;
		case LOGGER_SET_MIN_LOGLEVEL:
			if(command_data.size() != 1)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			level = ntohl(command_data[0]);
			if(level < 1 || level > 4)
				return this->sv->send_errno(ERR_BAD_PARAMETERS);

			o->set_minimal_loglevel((enum logger_loglevel) level);
		case LOGGER_LOG_ALGORITHM:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			o->set_log_algorithm(true);
			return this->sv->send_errno(ERR_SUCCESS);
		case LOGGER_NOT_LOG_ALGORITHM:
			if(command_data.size() != 0)
				return this->sv->send_errno(ERR_BAD_PARAMETER_COUNT);
			o->set_log_algorithm(false);
			return this->sv->send_errno(ERR_SUCCESS);
		default:
			return this->sv->send_errno(ERR_BAD_COMMAND);
	}

	return false;
}}};

void co_logger::ref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.insert(oid);
}}};

void co_logger::deref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.erase(oid);
}}};

