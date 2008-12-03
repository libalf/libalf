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

#ifndef __libalf_structured_query_tree_h__
# define __libalf_structured_query_tree_h__

namespace libalf {

#include <list>
#include <string>
#include <arpa/inet.h>

#include <libalf/answer.h>
#include <libalf/alphabet.h>

using namespace std;

template <class answer>
class prefix_enabled_query {
	private:
		list<int> word;
		int prefix_count;
		list<answer> acceptance;
	public:
		prefix_enabled_query()
		{{{
			int prefix_count = -1;
		}}}

		list<int> get_word()
		{{{
			return word;
		}}}
		int get_prefix_count()
		{{{
			return prefix_count;
		}}}
		void set_query(list<int> &word, int prefix_count)
		{{{
			this->word = word;
			this->prefix_count = prefix_count;
		}}}
		bool is_answered()
		{{{
			return (prefix_count + 1 == acceptance.length());
		}}}
		void answer(list<answer> acceptance)
		{{{
			this->acceptance = acceptance;
		}}}

		basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;
			list<answer>::iterator ai;

			ret += 0; // length field, filled in later.

			// word
			ret += serialize_word(word);

			// prefix count
			ret += htonl(prefix_count);

			// acceptances
			ret += htonl(acceptance.length());
			for(ai = acceptance.begin(); ai != acceptance.end(); ai++)
				ret += htonl( (int32_t)(*ai) );

			ret[0] = htonl( ret.length() - 1 );
			return ret;
		}}}

		bool deserialize(basic_string<int_32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			int size;
			int s, count;

			if(it == limit)
				return goto deserialization_failed;

			// size of serialized data stream.
			size = ntohl(*it);
			it++; if(size <= 0 || limit == it) goto deserialization_failed;

			// word
			if( ! deserialize_word(word, it, limit) )
				goto deserialization_failed;
			size -= word.size() + 1; if(size <= 0 || limit == it) goto deserialization_failed;

			// prefix count
			prefix_count = ntohl(*it);
			size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;

			// acceptances
			count = ntohl(*it);
			size--; it++; if(size <= 0 || limit == it || count != size) goto deserialization_failed;
			if(count != size) goto deserialization_failed;

			for(/* -- */; count > 0 && it != limit; count--, it++) {
				answer a;
				a = (int32_t)(ntohl(*it));
				acceptance.push_back(a);
			}

			if(count) goto deserialization_failed;

			return true;

		deserialization_failed:
			prefix_count = -1;
			word.clear();
			return false;
		}}}

}

template <class answer>
class structured_query_tree {
	public:
		typedef list<prefix_enabled_query>::iterator iterator;

	private:
		list<prefix_enabled_query> queries;

	public:
		basic_string<int32_t> serialize()
		{
		}
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
		}
		void print()
		{
		}
		void add_query(list<int> word, int prefix_count)
		{
		}
		void answer_query(list<int> word, answer acceptance)
		{
		}
		bool is_answered()
		{
			structured_query_tree::iterator qi;
			for(qi = this->begin(); qi != this->end(); qi++)
				if(!qi->is_answered())
					return false;
			return true;
		}
		void clear()
		{
		}

		structured_query_tree::iterator begin()
		{{{
			return queries.begin();
		}}}

		structured_query_tree::iterator end()
		{{{
			return queries.end();
		}}}
}


} // end of namespace libalf

#endif // __libalf_structured_query_tree_h__

