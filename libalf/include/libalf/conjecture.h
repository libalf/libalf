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

