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

#include <libalf/alf.h>

namespace libalf {

using namespace std;

static const char* libalf_version_string = "libalf version " VERSION;

const char* libalf_version()
{{{
	return libalf_version_string;
}}}

}

