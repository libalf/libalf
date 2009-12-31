/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libmVCA.
 *
 * libmVCA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libmVCA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libmVCA.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libmvca_nondeterministic_mvca_h__
# define __libmvca_nondeterministic_mvca_h__

#include <list>
#include <set>
#include <string>
#include <map>

#include <libmVCA/pushdown.h>
#include <libmVCA/transition_function.h>
#include <libmVCA/mVCA.h>

namespace libmVCA {

using namespace std;

// NOTE: this implementation DOES NOT SUPPORT epsilon transitions.

class nondeterministic_mVCA : public mVCA {
	protected: // data
		map<int, nondeterministic_transition_function> delta_function;

	public: // methods
		virtual set<int> transition(const set<int> & from, int & m, int label);
		virtual string generate_dotfile();
		virtual enum mVCA::mVCA_derivate get_derivate_id()
		{ return mVCA::DERIVATE_NONDETERMINISTIC; };
	protected:
		virtual basic_string<int32_t> serialize_derivate();
		virtual bool deserialize_derivate(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);
};

}; // end of namespace libmVCA.

#endif // __libmvca_nondeterministic_mvca_h__

