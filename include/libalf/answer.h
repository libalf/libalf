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

#include <stdint.h>

#ifndef __libalf_answer_h__
# define __libalf_answer_h__

namespace libalf {

// a possible <answer> has to implement the following:
//
// bool operator==(&answer)
// bool operator!=(&answer)
// bool operator>(&answer) , where true > possibly true > unknown > possibly false > false
// void operator=(&answer)
//
// bool operator==(bool)
// bool operator!=(bool)
// bool operator>(bool)
// void operator=(bool)
//
// operator int32_t()
// void operator=(int32_t)
//
// has to be castable to int32_t
//
// all the above are valid for the builtin-type bool.


// an example implementation:


class extended_bool {

	public:
		enum e_extended_bool {
			EBOOL_FALSE = 0,
			EBOOL_UNKNOWN = 1,
			EBOOL_TRUE = 1
		};

		enum e_extended_bool value;


		bool __attribute__((const)) operator==(extended_bool &other);

		bool __attribute__((const)) operator!=(extended_bool &other);

		void operator=(extended_bool &other);

		bool __attribute__((const)) operator>(extended_bool &other);

		bool __attribute__((const)) operator==(bool other);

		bool __attribute__((const)) operator>(bool other);

		void operator=(bool other);

		operator int32_t();

		void operator=(int32_t other);
};

bool __attribute__((const)) operator==(extended_bool a, extended_bool b);

}

#endif // __libalf_answer_h__

