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
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_normalizer_h__
# define __libalf_normalizer_h__

#include <list>
#include <string>

namespace libalf {

using namespace std;

class normalizer {
	public:
		enum type {
			NORMALIZER_NONE = 0,
			NORMALIZER_MSC = 1,
			NORMALIZER_LAST_INVALID = 2,
		};

		virtual ~normalizer() { };

		virtual enum type get_type()
		{ return NORMALIZER_NONE; };

		virtual basic_string<int32_t> serialize() = 0;
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		virtual bool deserialize_extension(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		virtual list<int> prefix_normal_form(list<int> & w, bool &bottom) = 0;
		virtual list<int> suffix_normal_form(list<int> & w, bool &bottom) = 0;
		// if the normalizer finds that the word is not in the MSCs language, bottom will be set to true.
		// that way, an learning algorithm can automatically set the words row to bottom.
};

}; // end of namespace libalf

#endif // __libalf_normalizer_h__

