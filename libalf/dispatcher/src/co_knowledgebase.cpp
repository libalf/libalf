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

co_knowledgebase::co_knowledgebase()
{
	o = new knowledgebase<extended_bool>;
};

co_knowledgebase::~co_knowledgebase()
{
	set<int>::iterator si;

	for(si = referring_learning_algorithms.begin(); si != referring_learning_algorithms.end(); si++)
		this->sv->objects[*si]->deref_knowledgebase(this->id);

	for(si = referring_iterators.begin(); si != referring_iterators.end(); si++)
		this->sv->objects[*si]->deref_knowledgebase(this->id);

	if(o) {
		delete o;
	}
};

bool co_knowledgebase::handle_command(int command, basic_string<int32_t> & command_data)
{
	
	return false;
};

void co_knowledgebase::ref_learning_algorithm(int oid)
{
	referring_learning_algorithms.insert(oid);
};

void co_knowledgebase::deref_learning_algorithm(int oid)
{
	referring_learning_algorithms.erase(oid);
};

void co_knowledgebase::ref_knowledgebase_iterator(int oid)
{
	referring_iterators.insert(oid);
};

void co_knowledgebase::deref_knowledgebase_iterator(int oid)
{
	referring_iterators.erase(oid);
};

