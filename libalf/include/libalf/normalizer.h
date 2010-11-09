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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_normalizer_h__
# define __libalf_normalizer_h__

#include <stdint.h>
#include <sys/types.h>

#include <list>
#include <string>

#define __helper__serialize_only_forward_declarations__
#include <libalf/serialize.h>

namespace libalf {

class normalizer {
	public:
		enum type {
			NORMALIZER_NONE = 0,
			NORMALIZER_MSC = 1,
			NORMALIZER_LAST_INVALID = 2,
		};

		virtual ~normalizer() { };

		virtual enum type get_type() const
		{ return NORMALIZER_NONE; };

		virtual std::basic_string<int32_t> serialize() const = 0;
		virtual bool deserialize(serial_stretch & serial) = 0;

		virtual bool deserialize_extension(serial_stretch & serial) = 0;

		virtual std::list<int> prefix_normal_form(const std::list<int> & w, bool &bottom) const = 0;
		virtual std::list<int> suffix_normal_form(const std::list<int> & w, bool &bottom) const = 0;
		// if the normalizer finds that the word is not in the MSCs language, bottom will be set to true.
		// that way, an learning algorithm can automatically set the words row to bottom.
};

}; // end of namespace libalf

#endif // __libalf_normalizer_h__

