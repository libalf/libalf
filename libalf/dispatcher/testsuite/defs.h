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

