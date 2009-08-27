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
			NORMALIZER_MSC = 1
		};

		virtual ~normalizer() { };

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

