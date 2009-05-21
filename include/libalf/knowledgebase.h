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
#include <sstream>
#include <arpa/inet.h>

#include <libalf/answer.h>
#include <libalf/alphabet.h>
#include <libalf/statistics.h>

// the knowledgebase holds membership information about a language.
// to obtain information about a word w, use ::resolve_query().
// if the requested information is not known, false will be returned.
// if you wish it to be marked as to be acquired (status==NODE_REQUIRED),
// use ::resolve_or_add_query().
//
// all membership information can be iterated via ::begin() .. ::end().
// all queries via ::qbegin() .. ::qend().
// the iterators resolve to a ::node. each node represents a word ::node::get_word().
//
// all nodes are contained in a single tree, with ::root representing epsilon.
// thus, you can use ::node::parent() or ::node::find_child() to find all prefixes
// of w or all words prefixed by w. if ::node::find_child() returns NULL, the concat
// is not yet contained in the tree. using find_or_create_child() solves this
// by adding a new node marked ::node::status==NODE_IGNORE and returning it.
// for this task, you can also use ::add_knowledge().
//
// ::create_query_tree() creates a new knowledgebase containing only the
// queries.
//
// ::merge_knowledgebase() merges membership information (no queries) from
// another knowledgebase into this (e.g. an answered query tree created before)

namespace libalf {

using namespace std;

template <class answer>
class knowledgebase {
	public: // types
		class node {
			friend class libalf::knowledgebase<answer>;
			friend class libalf::knowledgebase<answer>::iterator;
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
				unsigned int timestamp;
				enum status_e status;
				answer ans;
			protected: // internal methods
				node* get_next(node * current_child)
				// used during iterator++
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
				void serialize_subtree(basic_string<int32_t> & into)
				// used during serialization
				{{{
					typename vector<node *>::iterator ci;

					into += htonl(label);
					into += htonl(timestamp);
					into += htonl(status);

					if(is_answered())
						into += htonl((int32_t)ans);

					int childcount = 0;
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci != NULL)
							childcount++;
					into += htonl(childcount);

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci != NULL)
							(*ci)->serialize_subtree(into);

				}}}
				bool deserialize_subtree(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit, int & count)
				{{{
					int childcount;

					if(it == limit) return false;

					// skip label, was set by parent
					it++, count--; if(it == limit) return false;
					timestamp = (int)ntohl(*it);
					if(base->timestamp <= timestamp)
						base->timestamp = timestamp + 1;

					it++, count--; if(it == limit) return false;
					status = (enum node::status_e)ntohl(*it);

					if(is_answered()) {
						it++, count--; if(it == limit) return false;
						ans = (int32_t)ntohl(*it);
					}

					it++, count--; if(it == limit) return false;
					childcount = ntohl(*it);
					it++, count--;

					for(/* nothing */; childcount > 0; childcount--) {
						node* c;

						if(it == limit) return false;

						c = this->find_or_create_child(ntohl(*it));
						c->deserialize_subtree(it, limit, count);
					}

					return true;
				}}}

			public: // public methods
				node(knowledgebase * base)
				{{{
					this->base = base;
					parent = NULL;
					label = -1;
					status = NODE_IGNORE;
					timestamp = 0;
				}}}
				~node()
				// reference in parent will stay ; all children will be deleted
				{{{
					typename vector<node*>::iterator ci;
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							delete (*ci);
				}}}

				node * get_selfptr()
				{{{
					return this;
				}}}

				node * get_parent()
				{{{
					  return parent;
				}}}
				node * find_child(int label)
				{{{
					if(label >= 0 && label < (int)children.size())
						return children[label];
					else
						return NULL;
				}}}
				node * find_child(list<int>::iterator infix_start, list<int>::iterator infix_limit)
				{{{
					node * n = this;

					while(infix_start != infix_limit && n != NULL) {
						n = n->find_child(*infix_start);
						++infix_start;
					}

					return n;
				}}}
				node * find_or_create_child(int label)
				{{{
					if(label < 0)
						return NULL;
					if(label >= (int)children.size()) {
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
						base->nodecount++;
					}

					return children[label];
				}}}
				node * find_or_create_child(list<int>::iterator infix_start, list<int>::iterator infix_limit)
				{{{
					node * n = this;

					while(infix_start != infix_limit && n != NULL) {
						n = n->find_or_create_child(*infix_start);
						++infix_start;
					}

					return n;
				}}}

				list<int> get_word()
				{{{
					list<int> w;
					node * n = this;

					while(n != NULL) {
						if(n->label >= 0)
							w.push_front(n->label);
						n = n->parent;
					}

					return w;
				}}}
				bool mark_required()
				// returns true if node is now required,
				// false if knowledge is already known.
				{{{
					if(status == NODE_IGNORE) {
						status = NODE_REQUIRED;
						timestamp = base->timestamp;
						base->required.push_back(this);
						return true;
					} else {
						return (status == NODE_REQUIRED);
					}
				}}}
				bool is_required()
				{{{
					return status == NODE_REQUIRED;
				}}}
				bool is_answered()
				{{{
					return status == NODE_ANSWERED;
				}}}
				bool set_answer(answer ans)
				// return false in case of inconsistency (if this was already known and both knowledges differ)
				{{{
					// check for inconsistencies
					if(status == NODE_ANSWERED)
						return ((answer)this->ans == (answer)ans);

					if(status == NODE_REQUIRED)
						base->required.remove(this);

					status = NODE_ANSWERED;
					this->ans = ans;
					this->timestamp = base->timestamp;
					base->timestamp++;

					base->answercount++;

					return true;
				}}}
				answer get_answer()
				{{{
					  return ans;
				}}}
				bool no_subqueries(bool check_self = true)
				{{{
					if(check_self && status == NODE_REQUIRED)
						return false;

					typename vector<node*>::iterator ci;

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							if( ! (*ci)->no_subqueries(true) )
								return false;

					return true;
				}}}
				bool different(node * other)
				{{{
					if(is_answered() && other->is_answered()) {
						return (
							(get_answer() == true && other->get_answer() == false)
						     || (get_answer() == false && other->get_answer() == true)
						);
					} else {
						return false;
					}
				}}}
				bool recursive_different(node * other, int depth)
				// two nodes are recursive different if there exists a word w
				// of maximum length `depth' in Sigma* with this.w is different from other.w .
				// to allow infinite long words, use depth = -1.
				{{{
					typename vector<node*>::iterator ci, oci;

					if(different(other))
						return true;

					if(depth == 0)
						return false;

					for(ci = children.begin(), oci = other->children.begin();
					    ci != children.end() && oci != other->children.end();
					    ci++, oci++)   {
						if((*ci) == NULL || (*oci) == NULL)
							continue;
						if((*ci)->recursive_different(*oci), depth > 0 ? depth-1 : -1)
							return true;
					}

					return false;
				}}}
				int get_memory_usage()
				{{{
					int ret;
					typename vector<node*>::iterator ci;

					ret = sizeof(this) + sizeof(node *) * children.size();
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							ret += (*ci)->get_memory_usage();

					return ret;
				}}}

				void ignore()
				// set status to ignore (and thus delete answer)
				{{{
					if(status == NODE_REQUIRED)
						base->required.remove(this);
					if(status == NODE_ANSWERED)
						base->answercount--;

					status = NODE_IGNORE;
					timestamp = 0;
				}}}
				bool cleanup()
				// remove all branches that consist only of ignores.
				// returns true if tree is empty (no answers, no queries)
				{{{
					bool may_remove_self;

					may_remove_self = ( this->status == NODE_IGNORE );

					// remove all children that may be removed
					typename vector<node*>::iterator ci;
					for(ci = children.begin(); ci != children.end(); ++ci) {
						if(*ci != NULL) {
							if( ! (*ci)->cleanup() ) {
								may_remove_self = false;
							} else {
								delete *ci;
								*ci = NULL;
							}
						}
					}

					return may_remove_self;
				}}}

		}; // end of knowledgebase::node



		class iterator : std::iterator<std::forward_iterator_tag, node> {
			private:
				knowledgebase * base;

				node * current;

				bool queries_only;
				typename list<node*>::iterator qi;
			public:
				iterator()
				{{{
					queries_only = false;
					current = NULL;
					qi = base->required.end();
				}}}
				iterator(const iterator & other)
				{{{
					base = other.base;

					current = other.current;

					queries_only = other.queries_only;
					qi = other.qi;
				}}}
				iterator(bool queries_only, typename list<node*>::iterator currentquery, node * current, knowledgebase * base)
				{{{
					this->base = base;
					this->current = current;
					this->queries_only = queries_only;
					this->qi = qi;
				}}}

				iterator & operator++()
				{{{
					if(queries_only) {
						// FIXME: we really dont want to search this each time.
						// but how?!
						qi = find(base->required.begin(), base->required.end(), current);
						if(qi != base->required.end()) {
							qi++;
							if(qi != base->required.end())
								current = *qi;
							else
								current = NULL;
						} else
							current = base->required.front();
					} else {
						current = current->get_next(NULL);
					}
					return *this;
				}}}
				iterator operator++(int foo)
				{{{
					iterator tmp = (*this);
					operator++();
					return tmp;
				}}}

				node & operator*()
				{{{
					return *current;
				}}}
				node * operator->()
				{{{
					return current;
				}}}
				iterator & operator=(const iterator & it)
				{{{
					base = it.base;

					current = it.current;

					queries_only = it.queries_only;
					qi = it.qi;

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

		}; // end of knowledgebase::iterator



		friend class libalf::knowledgebase<answer>::node;
		friend class libalf::knowledgebase<answer>::iterator;
	protected: // data
		// full tree
		node * root;
		// list of all nodes that are required
		list<node *> required;

		int nodecount;
		int answercount;
		// count_queries is required.size().

		unsigned int timestamp;

		statistics * stat;
	public: // methods
		knowledgebase()
		{{{
			root = NULL;
			clear();
			stat = NULL;
		}}}
		knowledgebase(statistics * stat)
		{{{
			root = NULL;
			clear();
			this->stat = stat;
		}}}

		~knowledgebase()
		{{{
			delete root;
		}}}

		void clear()
		// does not clear stats, only the tree
		{{{
			if(root) // check only required so we dont bang in constructors
				delete root;

			timestamp = 1;

			required.clear();

			root = new node(this);

			nodecount = 1;
			answercount = 0;
		}}}

		void clear_queries()
		// remove all query-marked nodes
		{{{
			while(!required.empty())
				required.front()->ignore();
			root->cleanup();
		}}}

		bool undo(unsigned int count)
		{{{
			iterator it;
printf("undo %d with current timestamp %d\n", count, timestamp);
			for(it = this->begin(); it != this->end(); it++)
				if(it->timestamp >= (timestamp - (int)count))
					it->ignore();
			timestamp -= count;
			if(timestamp < 1)
				timestamp = 1;

			root->cleanup();

			return true;
		}}}

		int get_memory_usage()
		{{{
			int ret;

			ret = sizeof(this);
			ret += root->get_memory_usage();
			ret += sizeof(node*) * required.size();

			return ret;
		}}}

		bool is_answered()
		{{{
			return (required.size() == 0);
		}}}
		bool is_empty()
		{{{
			return ( (required.size() == 0) && (answercount == 0) );
		}}}

		unsigned int get_timestamp()
		{{{
			return timestamp;
		}}}

		int count_nodes()
		{{{
			return nodecount;
		}}}
		int count_answers()
		{{{
			return answercount;
		}}}
		int count_queries()
		{{{
			return required.size();
		}}}


		void print(ostream &os)
		{{{
			iterator ki;
				list<int> w;

			os << "knowledgebase {\n";

			for(ki = this->begin(); ki != this->end(); ki++) {
				os << "\tnode ";
				w = ki->get_word();
				print_word(os, w);
				os << " marked " << ( (ki->is_answered()) ? "!" : ( (ki->is_required()) ? "?" : "%" ) );
				if(ki->is_answered())
					os << " answered " << ( (ki->get_answer() == true) ? "+" : ( (ki->get_answer() == false) ? "-" : "?" ) );
				os << "\n";
			}

			os << "}\n";
		}}}
		string tostring()
		{{{
			stringstream str;
			this->print(str);
			return str.str();
		}}}
		string generate_dotfile()
		{{{
			string ret;

			char buf[128];
			iterator it;

			list<int> word;
			string wname;

			ret = "digraph knowledgebase {\n"
				"\trankdir=LR;\n"
				"\tsize=8;\n";

			// add all nodes
			for(it = this->begin(); it != this->end(); it++) {
				word = it->get_word();
				wname = word2string(word);
				snprintf(buf, 128, "\tnode [shape=\"%s\", style=\"filled\", color=\"%s\"]; \"%s [%d]\";\n",
						// shape
						it->is_answered() ? "ellipse" : "pentagon",
						// color
						it->is_answered() ? ( (it->get_answer() == true) ? "green" : (it->get_answer() == false ? "red" : "blue"))
								  : (it->is_required() ? "darkorange" : "grey"),
						// name
						wname.c_str(), it->timestamp
					);
				buf[127] = 0;
				ret += buf;
			}
			// and add all connections
			for(it = this->begin(); it != this->end(); it++) {
				typename vector<node *>::iterator ci;
				string toname;

				word = it->get_word();
				wname = word2string(word);
				for(ci = it->children.begin(); ci != it->children.end(); ci++) {
					if(*ci) {
						word = (*ci)->get_word();
						toname = word2string(word);

						snprintf(buf, 128, "\t\"%s [%d]\" -> \"%s [%d]\" [ label = \"%d\" ];\n",
								wname.c_str(), it->timestamp,
								toname.c_str(), (*ci)->timestamp,
								(*ci)->label
							);
						buf[127] = 0;
						ret += buf;
					}
				}
			}

			ret += "}\n";
			return ret;
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

			ret += 0; // sizeof, will be filled in later
			root->serialize_subtree(ret);
			ret[0] += htonl(ret.size() - 1);

			return ret;
		}}}
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			int size;

			if(it == limit)
				goto deserialization_failed;

			clear();

			size = ntohl(*it);
			it++; if(it == limit) goto deserialization_failed;

// network byte order... try not to mess with negative numbers...
//			if(ntohl(*it) != -1)
//				goto deserialization_failed;

			if(!root->deserialize_subtree(it, limit, size))
				goto deserialization_failed;

			if(size == 0)
				return true;

		deserialization_failed:
			clear();
			return false;
		}}}
		bool deserialize_query_acceptances(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		// please see create_query_tree() for the expected order
		{{{
			int size;
			iterator ki;

			if(it == limit)
				goto deserialization_failed;

			size = ntohl(*it);
			it++;

			// we expect _exactly_ as much answers as we have required nodes
			if(size != (int)required.size())
				goto deserialization_failed;

			while( ! required.empty() ) {
				ki = this->qbegin();
				if(it == limit)
					goto deserialization_failed;
				answer a;
				a = (int32_t)ntohl(*it);
				ki->set_answer(a);
				size--;
			}

			if(size == 0)
				return true;

		deserialization_failed:
			clear();
			return false;
		}}}

		knowledgebase * create_query_tree()
		// the timestamp in the resulting query tree represents
		// the order we expect in deserialize_query_acceptances()
		{{{
			knowledgebase * query_tree;
			iterator qi;

			query_tree = new knowledgebase(NULL);

			for(qi = this->qbegin(); qi != this->qend(); ++qi) {
				list<int> qw;
				qw = qi->get_word();
				query_tree->add_query(qw, 0);
				query_tree->timestamp++;
			}

			return query_tree;
		}}}
		bool merge_knowledgebase(knowledgebase & other_tree)
		// only merges answered information, no queries!
		// returns false if knowledge of the trees is inconsistent.
		// all new knowledge will have the same timestamp!
		{{{
			iterator ki;
			int static_timestamp = timestamp; // we want one timestamp for
			// the whole merge, so we have to keep it static!

			for(ki = other_tree.begin(); ki != other_tree.end(); ++ki)
				if(ki->is_answered()) {
					list<int> w;
					w = ki->get_word();
					if(!this->add_knowledge(w, ki->get_answer()))
						return false;
					timestamp = static_timestamp;
				}
			// increment timestamp so we mark one complete merge with a single timestamp
			timestamp++;
			return true;
		}}}

		/*
		 * TODO: assistant / filter
		 *
		 * template <class answer> class assistant {
		 *	public:
		 *		bool resolve_query(list<int> word, knowledgebase & base, answer & result)
		 * }
		 *
		 * internal: list<assistant*>
		 *
		 * add_assistant(assistant*)
		 * remove_assistant(assistant*)
		 * clear_assistants()
		 */

		bool add_knowledge(list<int> & word, answer acceptance)
		// will return false if knowledge for this word was already set and is != acceptance.
		// in this case, the holder is in an inconsistent state and
		// the knowledgebase will not change itself.
		{{{
			return root->find_or_create_child(word.begin(), word.end())->set_answer(acceptance);
		}}}
		int add_query(list<int> & word, int prefix_count = 0)
		// returns the number of new required nodes (excluding those already known.
		{{{
			node * current;
			list<int>::iterator wi;
			int skip_prefixes;
			int new_queries = 0;

			skip_prefixes = word.size() - prefix_count;
			current = root;

			// walk path to target and mark all required prefixes
			for(wi = word.begin(); wi != word.end(); wi++, skip_prefixes--) {
				if(skip_prefixes <= 0)
					if(current->mark_required())
						new_queries++;
				current = current->find_or_create_child(*wi);
			}

			// mark target itself
			if(current->mark_required())
				new_queries++;

			return new_queries;
		}}}
		bool resolve_query(list<int> & word, answer & acceptance)
		// returns true if known
		{{{
			node * current;

			current = root->find_child(word.begin(), word.end());

			if(current != NULL && current->is_answered()) {
				acceptance = current->get_answer();
				if(stat)
					stat->query_count.membership++;
				return true;
			} else {
				return false;
			}
		}}}
		bool resolve_or_add_query(list<int> & word, answer & acceptance)
		// returns true if known.
		// otherwise marks knowledge as to-be-acquired and returns false.
		{{{
			node * current;

			current = root->find_or_create_child(word.begin(), word.end());

			if(current->is_answered()) {
				acceptance = current->get_answer();
				if(stat)
					stat->query_count.membership++;
				return true;
			} else {
				current->mark_required();
				return false;
			}
		}}}

		iterator begin()
		// begin() always begins at root node (epsilon)!
		{{{
			iterator it(false, required.end(), root, this);
			return it;
		}}}
		iterator end()
		{{{
			iterator it;
			return it;
		}}}

		iterator qbegin()
		{{{
			if(required.empty()) {
				return qend();
			} else {
				iterator it(true, required.begin(), required.front(), this);
				return it;
			}
		}}}
		iterator qend()
		{{{
			iterator it;
			return it;
		}}}

};

}; // end of namespace libalf

#endif // __libalf_knowledgebase_h__

