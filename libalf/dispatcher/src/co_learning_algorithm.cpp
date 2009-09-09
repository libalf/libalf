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

#include <stdlib.h>

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
{
	referenced_knowledgebase = -1;
	referenced_logger = -1;
	referenced_normalizer = -1;

	switch(alg) {
		default:
			log("client %d: somehow bad request reached co_learning_algorithm constructor. killing client.\n", getpid());
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
};

co_learning_algorithm::~co_learning_algorithm()
{
	if(referenced_knowledgebase != -1)
		this->sv->objects[referenced_knowledgebase]->deref_learning_algorithm(this->id);
	if(referenced_logger != -1)
		this->sv->objects[referenced_logger]->deref_learning_algorithm(this->id);
	if(referenced_normalizer != -1)
		this->sv->objects[referenced_normalizer]->deref_learning_algorithm(this->id);

	if(o) {
		delete o;
	}
};

bool co_learning_algorithm::handle_command(int command, basic_string<int32_t> & command_data)
{
	
	return false;
};

void co_learning_algorithm::ref_knowledgebase(int oid)
{
	referenced_knowledgebase = oid;
};

void co_learning_algorithm::deref_knowledgebase(int oid)
{
	referenced_knowledgebase = -1;
	if(o)
		o->set_knowledge_source(NULL);
};

void co_learning_algorithm::ref_logger(int oid)
{
	referenced_logger = oid;
};

void co_learning_algorithm::deref_logger(int oid)
{
	referenced_logger = -1;
	if(o)
		o->set_logger(NULL);
};

void co_learning_algorithm::ref_normalizer(int oid)
{
	referenced_normalizer = oid;
};

void co_learning_algorithm::deref_normalizer(int oid)
{
	referenced_normalizer = -1;
	if(o)
		o->unset_normalizer();
};

