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

#ifndef __libalf_knowledgebase_h__
# define __libalf_knowledgebase_h__

#include <iterator>
#include <list>
#include <vector>
#include <string>
#include <arpa/inet.h>

#include <libalf/answer.h>
#include <libalf/alphabet.h>
#include <libalf/statistics.h>

namespace libalf {

using namespace std;

template <class answer>
class knowledgebase {
	private:
		class node {
			public:	// types
				enum status {
					NODE_IGNORE = 0,
					NODE_REQUIRED = 1,
					NODE_ANSWERED = 2
				};

			public:	// data
				node * parent; // NULL for root-node
				vector<node *> children; // NULL is a valid placeholder

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
					typename vector<node*>::iterator ci;
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							delete (*ci);
				}}}

				node * find_child(int label)
				{{{
					if(label < children.size())
						return children[label];
					else
						return NULL;
				}}}

				node * create_or_find_child(int label)
				{{{
					if(label >= children.size()) {
						// push back NULL until new alphabet-size is met
						for(int c = label - children.size(); c >= 0; c--) {
							children.push_back((node *)NULL);
						}
					}

					if(children[label] == NULL) {
						// on demand create new child
						children[label]->parent = this;
						children[label] = new node(children.size());
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

					ret = sizeof(this) + sizeof(node *) * children.size();
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							ret += (*ci)->get_size();

					return ret;
				}}}

				node* get_next(node * current_child)
				{{{
					vector<node *>::iterator ci;

					// find next after current child
					if(current_child != NULL) {
						ci = find(children.begin(), children.end(), current_child);
						if(ci == children.end())
							ci = children.begin();
						else
							ci++;
					} else {
						ci = children.begin();
					}

					// check children
					for(/* nothing */; ci != children.end(); ci++)
						if(*ci != NULL)
							return *ci;

					// refer to parent
					if(parent)
						return parent->get_next(this);
					else
						return NULL;
				}}}

		}; // end of knowledgebase::node

		class iterator_deref {
			private:
				node * current;
			public:
				void set_current(node * current)
				{{{
					this->current = current;
				}}}
				list<int> get_word()
				{{{
					node * n;
					list<int> w;

					n = current;

					while(n != NULL) {
						w.push_front(n->label);
						n = n->parent;
					}

					return w;
				}}}
				void answer(answer a)
				{{{
					current->answer = a;
				}}}
				bool is_answered()
				{{{
					return current->status == node::status::NODE_ANSWERED;
				}}}
				bool is_required()
				{{{
					return current->status == node::status::NODE_REQUIRED;
				}}}
		}; // end of knowledgebase::iterator_deref

		class iterator : forward_iterator<> {
			private:
				bool queries_only;
				node * current;
				knowledgebase * knowledge;
				iterator_deref deref;
			public:
				iterator()
				{{{
					queries_only = false;
					current = NULL;
				}}}
				iterator(bool queries_only, node * current, knowledgebase * knowledge)
				{{{
					this->queries_only = queries_only;
					this->current = current;
					this->knowledge = knowledge;
				}}}
				iterator & operator++()
				{{{
					if(queries_only) {
						list<node *>::iterator qi;
						qi = find(knowledge->required.begin(), knowledge->required.end(), current);
						if(qi != knowledge->required.end())
							current = *(qi++);
						else
							current = required.front();
					} else {
						current = current->get_next(NULL);
					}
					return *this;
				}}}
				bool operator==(const iterator & it)
				{{{
					return (current == it.current);
				}}}
				iterator & operator=(const iterator & it)
				{{{
					queries_only = it.queries_only;
					current = it.current;
					knowledge = it.knowledge;
				}}}
				iterator_deref & operator*()
				{{{
					deref.set_current(current);
					return * deref;
				}}}

		}; // end of knowledgebase::iterator

	private:
		// full tree
		node * root;
		// list of all nodes that are required
		list<node *> required;

		statistics * stat;
	public:
		knowledgebase()
		{{{
			root = NULL;
			stat = NULL;
		}}}
		knowledgebase(statistics * = NULL)
		{{{
			this->stat = stat;
			root = new node();
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
				root = new node();
			}
		}}}

		void add_query(list<int> & word, int prefix_count)
		{{{
			node * current;
			list<int>::iterator wi;
			int skip_prefixes;

			skip_prefixes = word.size() - prefix_count;
			current = root;

			// walk path to target and mark all required prefixes
			for(wi = word.begin(); wi != word.end(); wi++, skip_prefixes--) {
				if(skip_prefixes <= 0) {
					// if already known or marked, skip
					if((*current)->status == node::NODE_IGNORE) {
						(*current)->status = node::NODE_REQUIRED;
						required.push_back(*current);
					}
				}
				current = (*current)->create_or_find_child(*wi);
			}

			// mark target itself
			if((*current)->status == node::NODE_IGNORE) {
				(*current)->status = node::NODE_REQUIRED;
				required.push_back(*current);
			}
		}}}

		bool resolve_query(list<int> & word, answer & acceptance)
		{{{
			node * current;
			list<int>::iterator wi;

			current = root;

			for(wi = word.begin(); wi != word.end(); wi++) {
				current = (*current)->find_child(*wi);
				if(current == NULL)
					return false;
			}

			if((*current)->status == node::NODE_ANSWERED) {
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

		iterator begin()
		{{{
			iterator it(false, root, this);
			return it;
		}}}

		iterator end()
		{{{
			iterator it();
			return it;
		}}}

		iterator qbegin()
		{{{
			if(required.empty()) {
				return qend();
			} else {
				iterator it(false, required.front(), this);
				return it;
			}
		}}}

		iterator qend()
		{{{
			iterator it();
			return it;
		}}}


};

}; // end of namespace libalf

#endif // __libalf_knowledgebase_h__

