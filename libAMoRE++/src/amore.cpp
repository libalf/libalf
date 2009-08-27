/* $Id$
 * vim: fdm=marker
 *
 * amore++
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <amore/ext.h>
#include <amore/buffer.h>
// <amore/vars> defines specific vars required by libAMoRE-1.0,
// like the alphabet
#include <amore/vars.h>

void _init()
{
	// initialize libAMoRE buffers
	initbuf();
}

void _fini()
{
	// free libAMoRE buffers
	freebuf();
}

namespace amore {

static const char* libamorepp_version_string = "libAMoRE++ version " VERSION;

const char* libamorepp_version()
{{{
	return libamorepp_version_string;
}}}

}; // end of namespace amore

