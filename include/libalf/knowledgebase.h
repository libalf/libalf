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
			friend class knowledgebase;
			friend class knowledgebase::iterator;
			public: // types
				enum status_e {
					NODE_IGNORE = 0,
					NODE_REQUIRED = 1,
					NODE_ANSWERED = 2
				};
			protected: // data
				knowledgebase * base;
				node * parent; // NULL for root-node
				vector<node *> children; // NULL is a valid placeholder
				// label is reduntant as this == parent->children[label]
				// except for root, where label should be -1 == epsilon
				int label;
				enum status_e status;
				answer ans;
			protected: // methods
				node* get_next(node * current_child)
				{{{
					typename vector<node *>::iterator ci;

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
			public: // methods
				node(knowledgebase * base)
				{{{
					this->base = base;
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

				node * get_parent()
				{{{
					  return parent;
				}}}
				node * child(int label)
				{{{
					if(label >= 0 && label < children.size())
						return children[label];
					else
						return NULL;
				}}}
				node * find_or_create_child(int label)
				{{{
					if(label < 0)
						return NULL;
					if(label >= children.size()) {
						// push back NULL until new alphabet-size is met
						for(int c = label - children.size(); c >= 0; c--) {
							children.push_back((node *)NULL);
						}
					}

					if(children[label] == NULL) {
						// on demand create new child
						children[label] = new node(base);
						children[label]->parent = this;
						children[label]->label = label;
					}

					return children[label];
				}}}
				node * find_or_create_child(list<int>::iterator infix_start, list<int>::iterator infix_limit)
				{{{
					node * n = this;

					while(infix_start != infix_limit && n != NULL) {
						n = n->find_or_create_child(*infix_start);
						infix_start++;
					}

					return n;
				}}}

				list<int> get_word()
				{{{
					node * n = this;
					list<int> w;

					while(n != NULL) {
						if(n->label >= 0)
							w.push_front(n->label);
						n = n->parent;
					}

					return w;
				}}}
				bool set_answer(answer ans)
				{{{
					// check for inconsistencies
					if(status == NODE_ANSWERED)
						return (this->ans == ans);

					if(status == NODE_REQUIRED)
						base->required.remove(this);

					status = NODE_ANSWERED;
					this->ans = ans;
				}}}
				answer get_answer()
				{{{
					  return ans;
				}}}
				bool is_required()
				{{{
					return status == NODE_REQUIRED;
				}}}
				bool is_answered()
				{{{
					return status == NODE_ANSWERED;
				}}}
				bool no_subqueries()
				{{{
					if(status == NODE_REQUIRED)
						return false;

					typename vector<node*>::iterator ci;

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							if( ! (*ci)->no_subqueries() )
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
		}; // end of knowledgebase::node



		class iterator /* : forward_iterator< FIXME > */ {
			private:
				bool queries_only;
				node * current;
				knowledgebase * knowledge;
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
						typename list<node *>::iterator qi;
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
				bool operator!=(const iterator & it)
				{{{
					return (current != it.current);
				}}}
				iterator & operator=(const iterator & it)
				{{{
					queries_only = it.queries_only;
					current = it.current;
					knowledge = it.knowledge;
				}}}
				node & operator*()
				{{{
					return * current;
				}}}
		}; // end of knowledgebase::iterator



		friend class knowledgebase::iterator;
	private:
		// full tree
		node * root;
		// list of all nodes that are required
		list<node *> required;

		statistics * stat;
	public:
		knowledgebase()
		{{{
			stat = NULL;
			root = new node(this);
		}}}
		knowledgebase(statistics * stat)
		{{{
			this->stat = stat;
			root = new node(this);
		}}}

		~knowledgebase()
		{{{
			delete root;
		}}}

		int get_size_in_bytes()
		{{{
			int ret;

			ret = sizeof(this);
			ret += root->get_size();
			ret += sizeof(node*) * required.size();

			return ret;
		}}}

		void print(ostream &os)
		{{{
			iterator ki;

			for(ki = this->begin(); ki != this->end(); ki++) {
				os << "node ";
				print_word(ki->get_word(), os);
				os << " marked " << ki->is_answered() ? "!" : ki->is_required() ? "?" : "%";
				if(ki->is_answered())
					os << " answered " << (ki->get_answer() == true) ? "+" : (ki->get_answer() == false) ? "-" : "?";
				os << "\n";
			}
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
		{
		}
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
		}
		bool deserialize_queries(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
		}

		knowledgebase * create_query_tree()
		// TODO: optimize
		{{{
			knowledgebase * query_tree;
			iterator qi;

			query_tree = new knowledgebase(NULL);

			for(qi = this->qbegin(); qi != this->qend(); qi++)
				query_tree->add_query(qi->get_word());

			return query_tree;
		}}}
		bool merge_query_tree(knowledgebase * query_tree)
		{{{
			iterator ki;
			for(ki = query_tree->begin(); ki != query_tree->end(); ki++)
				if(ki->is_answered())
					if(!add_knowledge(ki->get_word(), ki->get_answer()))
						return false;
			return true;
		}}}

		// assistant / filter

		void clear()
		// does not clear stats, only the tree
		{{{
			delete root;
			root = new node(this);
			required.clear();
		}}}

		bool add_knowledge(list<int> & word, answer acceptance)
		// will return false if knowledge for this word was already set and is != acceptance.
		// in this case, the holder is in an inconsistent state and
		// the knowledgebase will not change itself.
		{{{
			node * n;
			list<int>::iterator wi;

			n = root;
			for(wi = word.begin(); wi != word.end(); wi++) {
				n = n->find_or_create_child(*wi);
			}

			return n->set_answer(acceptance);
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
				current = (*current)->find_or_create_child(*wi);
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
				current = (*current)->child(*wi);
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

		bool resolve_or_add_query(list<int> & word, answer & acceptance)
		// TODO: optimize
		{{{
			node * current;
			list<int>::iterator wi;

			current = root;

			for(wi = word.begin(); wi != word.end(); wi++) {
				current = (*current)->child(*wi);
				if(current == NULL) {
					add_query(word, 0);
					return false;
				}
			}

			if((*current)->status == node::NODE_ANSWERED) {
				acceptance = (*current)->answer;
				if(stat)
					stat->query_count.membership++;
				return true;
			} else {
				add_query(word, 0);
				return false;
			}
		}}}

		bool is_answered()
		{{{
			return root->no_subqueries();
		}}}

		bool is_empty()
		{{{
			iterator it;
			for(it = this->begin(); it != this->end(); it++) {
				if(it->is_answered())
					return false;
				if(it->is_required())
					return false;
			}
			return true;
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

