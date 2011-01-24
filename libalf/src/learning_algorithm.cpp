/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2011 David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include "libalf/learning_algorithm.h"

namespace libalf {

// algorithm names (StringList) {{{

struct type2name_mapping_s {
	enum learning_algorithm_type type;
	const char * name;
};

static const struct type2name_mapping_s learning_algorithm_typename_list[] = {
	{ ALG_NONE,			"invalid type (none)" },
	// Valid algorithms:
	{ ALG_ANGLUIN,			"angluin_simple_table" },
	{ ALG_ANGLUIN_COLUMN,		"angluin_col_table" },
	{ ALG_RIVEST_SCHAPIRE,		"rivest_schapire_table" },
	{ ALG_NL_STAR,			"NLstar_table" },
	{ ALG_MVCA_ANGLUINLIKE,		"mVCA_angluinlike" },
	{ ALG_BIERMANN,			"biermann" },
	{ ALG_RPNI,			"RPNI" },
	{ ALG_DELETE2,			"DeLeTe2" },
	{ ALG_BIERMANN_ORIGINAL,	"original_biermann" },
	{ ALG_KEARNS_VAZIRANI,		"kearns_vazirani" },
	// Terminator:
	{ ALG_LAST_INVALID,		NULL }
};

static const char * learning_algorithm_invalid_typename = "invalid type (fallthrough)";

// }}}

const char * learning_algorithm_name(enum learning_algorithm_type type)
{{{
	int i = 0;

	while(learning_algorithm_typename_list[i].name != NULL) {
		if(learning_algorithm_typename_list[i].type == type)
			return learning_algorithm_typename_list[i].name;
		++i;
	}

	// fallthrough
	return learning_algorithm_invalid_typename;
}}}

} // end of namespace libalf.

