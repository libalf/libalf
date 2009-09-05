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

#include <sys/types.h>
#include <unistd.h>

#include "main.h"
#include "object.h"

object::object()
{{{
	type = OBJ_NONE;
	o_logger = NULL;
	reference_count = 0;
}}}

object::~object()
{{{
	if(reference_count != 0) {
		// ouch!
		log("client %d: ouch! deleting object with reference_count != 0!\n", getpid());
	}
	if(o_logger != NULL) {
		switch(type) {
			case OBJ_NONE:
				log("client %d: ouch! object is OBJ_NONE but has content. abandoning object.\n", getpid());
				break;
			case OBJ_LOGGER:
				delete o_logger;
				break;
			case OBJ_KNOWLEDGEBASE:
				delete o_knowledgebase;
				break;
			case OBJ_KNOWLEDGEBASE_ITERATOR:
				delete o_kiterator;
				break;
			case OBJ_ALGORITHM:
				delete o_algorithm;
				break;
			case OBJ_NORMALIZER:
				delete o_normalizer;
				break;
		}
	}
}}}

void object::set_servant(servant * sv)
{{{
	this->sv = sv;
}}}

enum object_type object::get_type()
{{{
	return type;
}}}

bool object::create(enum object_type t)
{
	if(type != OBJ_NONE && o_logger != NULL) {
		log("client %d: trying to override existing object... rejecting.\n", getpid());
		return false;
	}

	type = t;

	switch (t) {
		case OBJ_NONE:
			log("client %d: created blank object.\n", getpid());
			return false;
		case OBJ_LOGGER:
			
			break;
		case OBJ_KNOWLEDGEBASE:
			
			break;
		case OBJ_KNOWLEDGEBASE_ITERATOR:
			
			break;
		case OBJ_ALGORITHM:
			
			break;
		case OBJ_NORMALIZER:
			
			break;
	}

	
	return true;
}

int object::get_reference_count()
{{{
	return reference_count;
}}}

void object::inc_reference_count()
{{{
	reference_count++;
}}}

void object::dec_reference_count()
{{{
	reference_count--;
}}}

