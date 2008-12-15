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

#ifndef __libalf_session_h__
# define __libalf_session_h__

#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>
#include <libalf/answer.h>

using namespace libalf;

class session {
	private:
		buffered_logger logger;
		learning_algorithm<extended_bool> * alg;
	public:
};


#endif // __libalf_session_h__

