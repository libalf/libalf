/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libAMoRE++
 *
 * libAMoRE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libAMoRE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libAMoRE++.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
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

