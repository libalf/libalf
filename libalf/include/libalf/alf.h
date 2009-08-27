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
#include <libalf/answer.h>
#include <libalf/automaton.h>
#include <libalf/basic_string.h>
#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>
#include <libalf/logger.h>
#include <libalf/normalizer.h>
#include <libalf/statistics.h>

namespace libalf {

using namespace std;

const char* libalf_version();

};

#endif

