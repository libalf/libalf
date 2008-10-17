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

#ifndef __libalf_alf_h__
# define __libalf_alf_h__

#include <libalf/alphabet.h>
#include <libalf/automata.h>
#include <libalf/logger.h>
#include <libalf/observationtable.h>
#include <libalf/oracle.h>
#include <libalf/statistics.h>
#include <libalf/teacher.h>

namespace libalf {

enum learning_algorithm {
	LEARN_ANGLUIN,
	LEARN_RIVEST_SHAPIRE,
	LEARN_LSTAR,
	LEARN_RFSA,

	LEARN_BIERMANN,
	LEARN_RPNI,

	LEARN_ANGLUIN_BIERMANN
};

//template <class answer>
//automata* learn_finite_automata( enum learning_algorithm, teacher<answer>, oracle, statistics * = NULL, logger * = NULL );




}; // end namespace libalf

#endif

