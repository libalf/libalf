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

co_knowledgebase_iterator::~co_knowledgebase_iterator()
{{{
	if(referenced_knowledgebase != -1)
		this->sv->objects[referenced_knowledgebase]->deref_knowledgebase_iterator(this->id);

	if(o) {
		delete o;
	}
}}};

bool co_knowledgebase_iterator::handle_command(int command, basic_string<int32_t> & command_data)
{
	
	return false;
};

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

