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
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#ifndef __defs_h__
# define __defs_h__

// from include/libalf/learning_algorithm.h:
enum learning_algorithm_type {
	ALG_NONE = 0,
	ALG_ANGLUIN = 1,
	ALG_ANGLUIN_COLUMN = 2,
	ALG_KVTREE = 3,
	ALG_BIERMANN = 4,
	ALG_BIERMANN_ANGLUIN = 5,
	ALG_NL_STAR = 6,
	ALG_RPNI = 7,
	ALG_DELETE2 = 8,
	ALG_LAST_INVALID = 9
};

// from include/libalf/normalizer.h:
enum normalizer_type {
	NORMALIZER_NONE = 0,
	NORMALIZER_MSC = 1,
	NORMALIZER_LAST_INVALID = 2,
};

#endif // __defs_h__

