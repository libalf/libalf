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
{
	
	return false;
};

void co_logger::ref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.insert(oid);
}}};

void co_logger::deref_learning_algorithm(int oid)
{{{
	referring_learning_algorithms.erase(oid);
}}};

