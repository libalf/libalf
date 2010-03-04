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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */


/*
 * DeLeTe2 is an offline learning algorithm for RFSA,
 * described in
 *	* F. Denis, A. Lemay, A. Terlutte: "Learning regular languages using RFSAs" [1]
 *	  (Theoretical Computer Science 313, 2004, p. 267-294)
 */


#ifndef __libalf_algorithm_delete2_h__
# define __libalf_algorithm_delete2_h__

#include <stdio.h>

#include <string>
#include <list>
#include <map>
#include <queue>

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

template <class answer>
class DeLeTe2 : public learning_algorithm<answer> {
	public:	// types
		typedef typename knowledgebase<answer>::node node;
		typedef pair<node*, node*> nodeppair;

	protected: // data

	public: // methods
		DeLeTe2(knowledgebase<answer> * base, logger * log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(base);
		}}}
		virtual ~DeLeTe2()
		{{{
			// nothing
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			this->set_alphabet_size(new_asize);
		}}}

		virtual memory_statistics get_memory_statistics()
		{
			memory_statistics ret;
			// FIXME: maybe keep some stats from last run?

			return ret;
		}

		virtual void receive_generic_statistics(generic_statistics & stat)
		{
			// FIXME: maybe keep some stats from last run?
		}

		virtual bool sync_to_knowledgebase()
		{{{
			return true;
		}}}

		virtual bool supports_sync()
		{{{
			return true;
		}}}

		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;

			// we don't have any internal, persistent data
			ret += htonl(1);
			ret += htonl(learning_algorithm<answer>::ALG_DELETE2);

			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			if(it == limit) return false;
			if(ntohl(*it) != 1)
				return false;

			it++; if(it == limit) return false;
			if(ntohl(*it) != learning_algorithm<answer>::ALG_DELETE2)
				return false;

			it++;

			return true;
		}}}

		virtual void print(ostream &os)
		{{{
			os << to_string();
		}}}
		virtual string to_string()
		{{{
			string s;
			return s;
		}}}

		// conjecture is always ready if there is a non-empty knowledgebase
		virtual bool conjecture_ready()
		{{{
			// we're offline. just performe some tests and
			// give a warning if they fail.

			// according to [1], we generate a
			// prefix-automaton from S+, but here we just
			// work with the whole given tree. thus we throw
			// a warning if there are non-accepting leafs

			if((this->my_knowledge != NULL) && (this->my_knowledge->count_answers() > 0)) {
				list<int> sample;
				if(leaf_is_non_accepting(this->my_knowledge->get_rootptr(), sample)) {
					string s;
					s = word2string(sample);
					(*this->my_logger)(LOGGER_INFO, "DeLeTe2 expects a sample-set consisting of pref(S+). "
							"but this sampleset has a non-accepting leaf \"%s\". "
							"you may try anyway, this is just a sidenote.\n", s.c_str());
				}

				if(this->get_alphabet_size() != this->my_knowledge->get_largest_symbol())
					(*this->my_logger)(LOGGER_WARN, "DeLeTe2: differing alphabet size between this (%d) and knowledgebase (%d)!\n",
							this->get_alphabet_size(), this->my_knowledge->get_largest_symbol());

				return true;
			} else {
				return false;
			}
		}}}

		// stubs for counterexample will throw a warning to the logger
		virtual bool add_counterexample(list<int>)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "DeLeTe2 does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
			return false;
		}}}

	protected:
		bool leaf_is_non_accepting(node* n, list<int> & sample, bool prefix_accepting = false)
		// check if all leafs (i.e. states that have no suffixes that either accept or reject) accept
		{{{
			list<int> w;
			if(n->is_answered()) {
				if(n->get_answer() == true)
					prefix_accepting = true;
				else
					if(n->get_answer() == false)
						prefix_accepting = false;
			}

			int i;
			bool has_children = false;
			node * c;

			for(i = 0; i < n->max_child_count(); i++) {
				c = n->find_child(i);
				if(c) {
					has_children = true;
					if(leaf_is_non_accepting(c, sample, prefix_accepting))
						return true;
				}
			}
			if(!has_children && !prefix_accepting) {
				sample = n->get_word();
				return true;
			}

			return false;
		}}}

		virtual bool complete()
		{{{
			// we're offline.
			return true;
		}}}

		void generate_inclusion_relation(list<node*> & pref, set<nodeppair> & inclusions)
		{{{
			// calculate language inclusion:
			// a « b iff. there is no word w
			// so that a.w is in S+ but b.w is in S-.

			inclusions.clear();

			// we will generate an inclusion relation by
			// checking each field in the (Pref X Pref)
			// matrix. we start at the lover right corner
			// and try each diagonal until we arrive at the
			// higher left corner.
			//
			// pref MUST be sorted in ascending graded
			// lexicographic order

			typename list<node*>::iterator a, b, first, last;
			first = pref.begin();
			last = pref.end();
			last--;
			a = last;
			b = last;
			while(true) {
#ifdef DELETE2_DEBUG_INCLUSION_RELATION
				list<int> wa,wb;
				wa = (*a)->get_word();
				wb = (*b)->get_word();
#endif
				if(*a == *b) {
					inclusions.insert(nodeppair(*a,*b));
					if(*a == pref.front())
						break;
				} else {
					// ROFL
					// check if a « b
#ifdef DELETE2_DEBUG_INCLUSION_RELATION
					(*this->my_logger)(LOGGER_DEBUG, "a == %s « %s == b ?\n\t\t\t", word2string(wa).c_str(), word2string(wb).c_str());
#endif
					bool good = true;
					if((*a)->is_answered() && (*b)->is_answered())
						if((*a)->get_answer() == true && (*b)->get_answer() == false) {
#ifdef DELETE2_DEBUG_INCLUSION_RELATION
							(*this->my_logger)(LOGGER_DEBUG, ".a. in S+ BUT .b. in S-\n");
#endif
							good = false;
						}

					if(good) {
						// check inclusion among suffixes
						for(int i = 0; i < (*a)->max_child_count(); i++) {
							nodeppair children;
							children.first = (*a)->find_child(i);
							children.second = (*b)->find_child(i);

							if(children.first != NULL) {
								if(children.second != NULL) {
									if(inclusions.find(children) == inclusions.end()) {
#ifdef DELETE2_DEBUG_INCLUSION_RELATION
										(*this->my_logger)(LOGGER_DEBUG, ".a.%d !« .b.%d\n", i, i);
#endif
										good = false;
										break;
									}
								}
							}
						}
						if(/* still */ good) {
#ifdef DELETE2_DEBUG_INCLUSION_RELATION
							(*this->my_logger)(LOGGER_DEBUG, "ok\n");
#endif
							inclusions.insert(nodeppair(*a,*b));
						}
					}

				}
				// walk in diagonal up and right. if at
				// edge of matrix, walk next diagonal.
				if(a == last && b != first) {
					a = b;
					a--;
					b = last;
				} else {
					if(b == first) {
						b = a;
						b--;
						a = first;
					} else {
						a++;
						b--;
					}
				}
			}
		}}}

		// derive an automaton and return it
		virtual conjecture * derive_conjecture()
		{{{
			simple_automaton *ret = new simple_automaton;
			if(this->my_knowledge->count_answers() == 0) {
				(*this->my_logger)(LOGGER_WARN, "DeLeTe2: you started an offline-algorithm with an empty knowledgebase. that does not make very much sense, does it?\n");
				// return automaton for empty language
				ret->alphabet_size = 1;
				ret->state_count = 1;
				ret->initial.insert(0);
				ret->valid = true;
				return ret;
			}

			if(this->get_alphabet_size() != this->my_knowledge->get_largest_symbol())
				(*this->my_logger)(LOGGER_WARN, "DeLeTe2: differing alphabet size between this (%d) and knowledgebase (%d)!\n",
						this->get_alphabet_size(), this->my_knowledge->get_largest_symbol());
			// generate a graded-lex ordered list of words in the knowledgebase (i.e. pref(S+) )
			list<node*> pref;

			kIterator_lex_graded<answer> klg(this->my_knowledge->get_rootptr());
			while(!klg.end()) {
				list<int> w = klg->get_word();
				pref.push_back(&(*klg));
				klg++;
			}

			(*this->my_logger)(LOGGER_INFO, "DeLeTe2: calculating inclusion relation.\n");

			// generate inclusion relation
			set<nodeppair> inclusions;

			generate_inclusion_relation(pref, inclusions);

#ifdef DELETE2_DEBUG_INCLUSION_RELATION
			(*this->my_logger)(LOGGER_DEBUG, "\nDeLeTe2: Inclusion relation:\n");
			typename set<nodeppair>::iterator si;
			for(si = inclusions.begin(); si != inclusions.end(); ++si) {
				list<int> a,b;
				a = si->first->get_word();
				b = si->second->get_word();
				(*this->my_logger)(LOGGER_DEBUG, "%s  «  %s\n", word2string(a).c_str(), word2string(b).c_str());
			}
			(*this->my_logger)(LOGGER_DEBUG, "\n");
#endif

			(*this->my_logger)(LOGGER_INFO, "DeLeTe2: deriving automaton.\n");

			// run DeLeTe2 algorithm
			typename list<node*>::iterator pi;

			ret->is_deterministic = false;
			ret->alphabet_size = this->alphabet_size;
			ret->state_count = 0;

			list<node*> state_candidates;	// the position in the list gives the numerical state-id. thus, only append new states.
			int sid = 0;
			typename list<node*>::iterator sci;
			nodeppair r1,r2;

			for(pi = pref.begin(); pi != pref.end(); ++pi) {
				list<int> piw = (*pi)->get_word();
				r1.first = r2.second = *pi;
				bool equivalent_state_exists = false;
				for(sci = state_candidates.begin(); sci != state_candidates.end() && !equivalent_state_exists; ++sci) {
					r1.second = r2.first = *sci;
					if(inclusions.find(r1) != inclusions.end() && inclusions.find(r2) != inclusions.end()) {
						list<int> w = (*sci)->get_word();
						(*this->my_logger)(LOGGER_DEBUG, "DeLeTe2: removing suffixes(%s) ( = %s )\n", word2string(piw).c_str(), word2string(w).c_str());
						equivalent_state_exists = true;
					}
				}
				if(equivalent_state_exists) {
					// delete *pi and all suffixes from pref
					typename list<node*>::iterator prev, next;
					prev = pi;
					next = prev; next++;
					while(next != pref.end()) {
						if((*next)->is_suffix_of(*pi)) {
							pref.erase(next);
							next = prev;
							next++;
						} else {
							prev=next;
							next++;
						}
					}
					prev = pi;
					++pi;
					pref.erase(prev);
					--pi;
				} else {
					(*this->my_logger)(LOGGER_DEBUG, "DeLeTe2: %s is new state candidate q%d. ", word2string(piw).c_str(), sid);

					// check if initial
					r1.second = this->my_knowledge->get_rootptr();
					if(inclusions.find(r1) != inclusions.end()) {
						(*this->my_logger)(LOGGER_DEBUG, "is initial. ");
						ret->initial.insert(sid);
					}
					// check if final
					if((*pi)->is_answered() && (*pi)->get_answer() == true) {
						(*this->my_logger)(LOGGER_DEBUG, "is final.");
						ret->final.insert(sid);
					}

					(*this->my_logger)(LOGGER_DEBUG, "\n");

					// add candidate to list
					state_candidates.push_back(*pi);

					// add transitions:
					pair<int, int> trid;
					int i;
					// incoming
					r1.first = *pi;
					for(i=0,sci = state_candidates.begin(); sci != state_candidates.end(); ++sci,i++) {
						for(int sigma = 0; sigma < (*sci)->max_child_count(); sigma++) {
							r1.second = (*sci)->find_child(sigma);
							if(inclusions.find(r1) != inclusions.end()) {
								trid.first = i;
								trid.second = sigma;
								ret->transitions.insert(pair<pair<int, int>, int>(trid, sid));
								(*this->my_logger)(LOGGER_DEBUG, "DeLeTe2: incoming transition (q%d,%d,q%d)\n", i, sigma, sid);
							}
						}
					}
					// outgoing
					trid.first = sid;
					for(i=0,sci = state_candidates.begin(); sci != state_candidates.end(); ++sci,i++) {
						if(*sci == *pi)
							continue; // already done in incoming transitions
						r1.first = *sci;
						for(int sigma = 0; sigma < (*pi)->max_child_count(); sigma++) {
							r1.second = (*pi)->find_child(sigma);
							if(inclusions.find(r1) != inclusions.end()) {
								trid.first = sid;
								trid.second = sigma;
								ret->transitions.insert(pair<pair<int, int>, int>(trid, i));
								(*this->my_logger)(LOGGER_DEBUG, "DeLeTe2: outgoing transition (q%d,%d,q%d)\n", sid, sigma, i);
							}
						}
					}

					sid++;
				}
				ret->state_count = state_candidates.size();
			}

			ret->valid = true;
			return ret;
		}}}

};


}; // end namespace libalf

#endif // __libalf_algorithm_delete2_h__

