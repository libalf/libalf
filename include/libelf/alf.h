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

#include <libalf/automata.h>
#include <libalf/logger.h>
#include <libalf/oracle.h>
#include <libalf/teacher.h>
#include <libalf/statistics.h>


namespace libalf {

enum learning_algorithms {
	LEARN_ANGLUIN,
	LEARN_RIVEST_SHAPIRE
};

automata* learn_finite_automata( enum learning_algorithm, teacher, oracle, statistics * = NULL, logger * = NULL );




}; // end namespace libalf


