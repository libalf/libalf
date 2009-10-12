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
 * (c) 2008,2009 by David R. Piegdon, Chair of Computer Science 2 and 7, RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#include <string>
#include <set>
#include <map>

#ifndef __libalf_conjecture_h__
# define __libalf_conjecture_h__

namespace libalf {

using namespace std;

class conjecture {
	public: // types
		enum type {
			CONJECTURE_NONE = 0,
			// BEGIN

			CONJECTURE_SIMPLE_AUTOMATON = 1,

			// END
			CONJECTURE_LAST_INVALID = 2
		};
	public: // members
		virtual ~conjecture()
		{ };

		virtual bool is_valid()
		{ return false; };

		virtual conjecture::type get_type()
		{ return CONJECTURE_NONE; }

		virtual void clear() = 0;

		// serializer
		virtual basic_string<int32_t> serialize() = 0;
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		// human readable version
		virtual string write() = 0;
		virtual bool read(string input) = 0;

		// visual version (dotfile preferred)
		virtual string visualize() = 0;

};

class simple_automaton : public conjecture {
	public: // yes, by all means!
		bool valid;

		bool is_deterministic;
		int alphabet_size;
		int state_count;
		set<int> initial;
		set<int> final;
		multimap<pair<int, int>, int> transitions;

	public:
		simple_automaton();
		virtual ~simple_automaton();
		virtual void clear();
		virtual bool is_valid();
		virtual conjecture::type get_type()
		{ return CONJECTURE_SIMPLE_AUTOMATON; }

		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);
		virtual string write();
		virtual bool read(string input);

		virtual string visualize();

		// calculate if automaton is deterministic. this function does not rely on is_deterministic,
		// but sets it.
		bool calculate_determinism();
};


}; // end of namespace libalf

#endif // __libalf_conjecture_h__

