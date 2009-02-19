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

#include <list>
#include <string>
#include <vector>
#include <arpa/inet.h>

#include <libalf/answer.h>
#include <libalf/alphabet.h>
#include <libalf/statistics.h>

namespace libalf {

using namespace std;

template <class answer>
class prefix_enabled_query {
	private:
		list<int> word;
		int prefix_count;
		list<answer> acceptance;
		// list of acceptances corresponds to prefixes in this way:
		// first acceptance <-> full word (ex. .2.1.2.0. )
		// next acceptance <-> longest prefix ( .2.1.2. )
		// next acceptance <-> shorter prefix ( .2.1. )
		// ...

	public:
		prefix_enabled_query()
		{{{
			prefix_count = -1;
		}}}
		prefix_enabled_query(list<int> &word, int prefix_count)
		{{{
			set_query(word, prefix_count);
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
		void clear_acceptances()
		{{{
			acceptance.clear();
		}}}
		bool is_answered()
		{{{
			return (prefix_count + 1 == (int)acceptance.size());
		}}}
		void set_answers(list<answer> & acceptance)
		{{{
			typename list<answer>::iterator ai;

			this->acceptance.clear();

			for(ai = acceptance.begin(); ai != acceptance.end(); ai++)
				this->acceptance.push_back(*ai);
		}}}

		bool matches(list<int> & query, answer & acceptance)
		// returns true if this query matches word and stores the corresponding answer in acceptance.
		{{{
			typename list<int>::iterator wi, qi;
			int wl, ql;
			typename list<answer>::iterator ai;

			wl = word.size();
			ql = query.size();

			wi = word.begin();
			qi = query.begin();

			// if supposed prefix is longer, its not a prefix
			if(ql > wl)
				return false;

			// check equality of prefix
			for(/* -- */ ; ql > 0 ; wl--, ql--, wi++, qi++)
				if(*wi != *qi)
					return false;

			// check if prefix is in prefix range
			if(wl > prefix_count)
				return false;

			for(ai = this->acceptance.begin(); wl > 0; ai++, ql--)
				if(ai == this->acceptance.end()) {
					printf("FIXME: prefix_enabled_query::matches MISMATCH!\n");
					return false; // no corresponding acceptance in list!
				}

			acceptance = *ai;
			return true;
		}}}

		basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;
			typename list<answer>::iterator ai;

			// NOTE: structured_query_tree::deserialize depends on string[0] being size!
			// see also doc/implementation_notes on (de)serialization
			ret += 0; // length field, filled in later.

			// prefix count
			ret += htonl(prefix_count);

			// word
			ret += serialize_word(word);

			// acceptances
			ret += htonl(acceptance.size());
			for(ai = acceptance.begin(); ai != acceptance.end(); ai++)
				ret += htonl( (int32_t)(*ai) );

			ret[0] = htonl( ret.length() - 1 );
			return ret;
		}}}
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			int size;
			int count;

			if(it == limit)
				goto deserialization_failed;

			// size of serialized data stream.
			size = ntohl(*it);
			it++; if(size <= 0 || limit == it) goto deserialization_failed;

			// prefix count
			prefix_count = ntohl(*it);
			size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;

			// word
			if( ! deserialize_word(word, it, limit) )
				goto deserialization_failed;
			size -= word.size() + 1; if(size <= 0 || limit == it) goto deserialization_failed;

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
		void print(ostream &os)
		{{{
			typename list<answer>::iterator ai;
			print_word(os, word);
			os << ", " << prefix_count << " prefixes; acceptances: ";
			for(ai = acceptance.begin(); ai != acceptance.end(); ai++)
				os << ( (int32_t)(*ai) );
			os << "\n";
		}}}

		basic_string<int32_t> serialize_acceptances()
		// NOTE: not conforming to doc/implementation_notes / notes on (de)serialization!
		// this is a plain string of answers!
		// it is thus REQUIRED (and not checked) that serialize_acceptances is only called if is_answered!
		{{{
			basic_string<int32_t> ret;
			typename list<answer>::iterator ai;

			for(ai = acceptance.begin(); ai != acceptance.end(); ai++)
				ret += htonl( (int32_t)(*ai) );

			return ret;
		}}}

		bool deserialize_acceptances(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		// NOTE: not conforming to doc/implementation_notes / notes on (de)serialization!
		{{{
			int c;
			answer a;

			if(it == limit) goto serialization_failed;

			acceptance.clear();

			// acceptance of word itself
			a = (int32_t) ntohl(*it);
			acceptance.push_back(a);

			// acceptances of all prefixes
			for(c = prefix_count; c > 0; c--) {
				it++; if(it == limit) goto serialization_failed;
				a = (int32_t) ntohl(*it);
				acceptance.push_back(a);
			}
			it++;

			return true;

		serialization_failed:
			acceptance.clear();
			return false;
		}}}

};

template <class answer>
class structured_query_tree {
	// to answer all queries, iterate over all members and it->answer all of them.
	public:
		typedef typename list< prefix_enabled_query<answer> >::iterator iterator;

	private:
		list< prefix_enabled_query<answer> > queries;
		statistics * stat;

	public:
		structured_query_tree()
		{{{
			stat = NULL;
		}}}

		void set_statistics(statistics * stat)
		{{{
			this->stat = stat;
		}}}
		void unset_statistics()
		{{{
			this->stat = NULL;
		}}}

		basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;
			basic_string<int32_t> q;
			iterator qi;

			ret += 0; // length field, filled in later

			// number of queries
			ret += htonl(queries.size());

			// queries
			for(qi = queries.begin(); qi != queries.end(); qi++)
				// depend on qi->serialized[0] == size!
				ret += qi->serialize();

			ret[0] = htonl( ret.length() - 1 );
			return ret;
		}}}
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			int size;
			int count;

			queries.clear();

			if(it == limit)
				goto deserialization_failed;

			// size of serialized data stream
			size = ntohl(*it);
			it++; if(size <= 0 || limit == it) goto deserialization_failed;

			//number of queries
			count = ntohl(*it);
			it++; size--; if(size <= 0 | limit == it) goto deserialization_failed;

			// queries
			for(/* -- */; count--; count > 0 && it != limit && size != 0) {
				prefix_enabled_query<answer> q;
				// depend on qi->serialized[0] == size!
				// peek into serialized prefix_enabled_query:
				size -= ntohl(*it) + 1;
				if( ! q.deserialize(it, limit) )
					goto deserialization_failed;
				queries.push_back(q);
			}

			if(size || count > -1)
				goto deserialization_failed;

			return true;

		deserialization_failed:
			queries.clear();
			return false;
		}}}
		basic_string<int32_t> serialize_acceptances()
		// this will reduce the serialized data to acceptances only.
		// only to be used if is_answered(). otherwise it will fail with
		// an empty basic_string<int32_t>.
		{{{
			basic_string<int32_t> ret;
			iterator qi;

			if( ! is_answered )
				return ret; // empty

			ret += 0; // data length, filled in later.

			for(qi = queries.begin(); qi != queries.end(); qi++)
				ret += qi->serialize_acceptances();

			ret[0] = htonl( ret.length() - 1 );

			return ret;
		}}}
		bool deserialize_acceptances(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			iterator qi;
			int size;
			int peq = 0;

			if(limit == it) goto deserialization_failed;

			// note: not checking size in any way.
			size = ntohl(*it);
			it++; if(it == limit) goto deserialization_failed;

			for(qi = queries.begin(); qi != queries.end(); qi++, peq++)
				if( ! qi->deserialize_acceptances(it, limit) )
					goto deserialization_failed;

			return true;

		deserialization_failed:
			for(qi = queries.begin(); qi != queries.end(); qi++)
				qi->clear_acceptances();
			return false;
		}}}
		void print(ostream &os)
		{{{
			iterator qi;
			os << "structured query tree\n";
			for(qi = queries.begin(); qi != queries.end(); qi++) {
				os << "\t";
				qi->print(os);
			}
		}}}

		void add_query(list<int> &word, int prefix_count)
		{{{
			prefix_enabled_query<answer> q;
			q.set_query(word, prefix_count);
			queries.push_back(q);
			/*
			iterator qi;

			for(qi = queries.begin(); qi != queries.end(); qi++) {
				if(is_smaller(qi->
			}
			*/
		}}}
		bool is_answered()
		{{{
			iterator qi;

			for(qi = this->begin(); qi != this->end(); qi++)
				if( ! qi->is_answered())
					return false;
			return true;
		}}}

		bool resolve_query(list<int> word, answer & acceptance)
		{{{
			iterator qi;

			for(qi = queries.begin(); qi != queries.end(); qi++)
				if(qi->matches(word, acceptance)) {
					if(stat)
						stat->query_count.membership++;
					return true;
				}

			return false;
		}}}

		void clear()
		{{{
			queries.clear();
		}}}

		bool is_empty()
		{{{
			return queries.is_empty();
		}}}
		iterator begin()
		{{{
			return queries.begin();
		}}}
		iterator end()
		{{{
			return queries.end();
		}}}
};



/*************************************************************************************************/

	namespace sqt {

		template <class answer>
		class node {
			public:	// types
				enum status {
					NODE_IGNORE = 0,
					NODE_REQUIRED = 1,
					NODE_ANSWERED = 2
				};

			public:	// data
				node<answer> * parent; // NULL for root-node
				vector<node<answer> *> children; // NULL is a valid placeholder

				// label is reduntant as this == parent->children[label]
				// except for root, where label should be -1 == epsilon
				int label;
				enum status status;
				answer ans;

			public:	// methods
				node()
				{{{
					parent = NULL;
					label = -1;
					status = NODE_IGNORE;
				}}}

				~node()
				// reference in parent will stay ; all children will be deleted
				{{{
					typename vector<node<answer>*>::iterator ci;
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							delete (*ci);
				}}}

				node<answer> * find_child(int label)
				{{{
					if(label < children.size())
						return children[label];
					else
						return NULL;
				}}}

				node<answer> * create_or_find_child(int label)
				{{{
					if(label >= children.size()) {
						// push back NULL until new alphabet-size is met
						for(int c = label - children.size(); c >= 0; c--) {
							children.push_back((node<answer> *)NULL);
						}
					}

					if(children[label] == NULL) {
						// on demand create new child
						children[label]->parent = this;
						children[label] = new node<answer>(children.size());
						children[label]->label = label;
					}

					return children[label];
				}}}

				bool all_answered()
				{{{
					if(status == NODE_REQUIRED)
						return false;

					typename vector<node*>::iterator ci;

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							if( ! (*ci)->all_answered() )
								return false;

					return true;
				}}}

				int get_size()
				{{{
					int ret;
					typename vector<node*>::iterator ci;

					ret = sizeof(this) + sizeof(node<answer> *) * children.size();
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							ret += (*ci)->get_size();

					return ret;
				}}}

		};

	}; // end of namespace sqt;

template <class answer>
class new_sqt {
	private:
		sqt::node<answer> * root;
		statistics * stat;
	public:
		new_sqt()
		{{{
			root = NULL;
			stat = NULL;
		}}}
		new_sqt(statistics * = NULL)
		{{{
			this->stat = stat;
			root = new sqt::node<answer>();
		}}}

		int get_size_in_bytes()
		{{{
			return sizeof(this) + (root ? root->get_size() : 0);
		}}}

		void print(ostream &os)
		{
		}

		void set_statistics(statistics * stat)
		{{{
			this->stat = stat;
		}}}
		void unset_statistics()
		{{{
			this->stat = NULL;
		}}}


		basic_string<int32_t> serialize()
		{
		}
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
		}

		basic_string<int32_t> serialize_acceptances()
		{
		}
		// this will reduce the serialized data to acceptances only. only to be used if is_answered() !
		bool deserialize_acceptances(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
		}

		void clear()
		// does not clear stats, only the tree
		{{{
			if(root) {
				delete root;
				root = new sqt::node<answer>();
			}
		}}}

		void add_query(list<int> & word, int prefix_count)
		{{{
			sqt::node<answer> * current;
			list<int>::iterator wi;
			int skip_prefixes;

			skip_prefixes = word.size() - prefix_count;
			current = root;

			// walk path to target and mark all required prefixes
			for(wi = word.begin(); wi != word.end(); wi++, skip_prefixes--) {
				if(skip_prefixes <= 0)
					if((*current)->status == sqt::node<answer>::NODE_IGNORE) // if already known or marked, skip
						(*current)->status = sqt::node<answer>::NODE_REQUIRED;
				current = (*current)->create_or_find_child(*wi);
			}

			// mark target itself
			if((*current)->status == sqt::node<answer>::NODE_IGNORE)
				(*current)->status = sqt::node<answer>::NODE_ANSWERED;
		}}}

		bool resolve_query(list<int> & word, answer & acceptance)
		{{{
			sqt::node<answer> * current;
			list<int>::iterator wi;

			current = root;

			for(wi = word.begin(); wi != word.end(); wi++) {
				current = (*current)->find_child(*wi);
				if(current == NULL)
					return false;
			}

			if((*current)->status == sqt::node<answer>::NODE_ANSWERED) {
				acceptance = (*current)->answer;
				if(stat)
					stat->query_count.membership++;
				return true;
			} else {
				return false;
			}
		}}}

		bool is_answered()
		{{{
			if(root)
				return root->all_answered();
			else
				return true; // empty tree is alway answered
		}}}

		bool is_empty()
		{{{
			return (root == NULL);
		}}}

};


}; // end of namespace libalf

#endif // __libalf_structured_query_tree_h__

