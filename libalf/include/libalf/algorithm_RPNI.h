/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *    and Daniel Neider,    i7 Informatik RWTH-Aachen
 *        <neider@automata.rwth-aachen.de>
 *
 * see LICENSE file for licensing information.
 */


/*
 * RPNI (regular positive, negative inference) is an offline learning algorithm,
 * described in e.g.
 *	* P. Graćıa and J. Oncina: "Inferring regular languages in polynomial update time"
 *	* D. Neider: "Learning Automata for Streaming XML Documents" [1]
 */


#ifndef __libalf_algorithm_rpni_h__
# define __libalf_algorithm_rpni_h__

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
class RPNI : public learning_algorithm<answer> {
	public:	// types
		class equivalence_relation {
			public: // types
				typedef typename knowledgebase<answer>::node node;
				typedef pair<node*, node*> nodeppair;
			public: // data
				knowledgebase<answer> * base;
				typename knowledgebase<answer>::equivalence_relation equivalences;
			protected:
				set<nodeppair> candidates;

			public: // member functions
				equivalence_relation(knowledgebase<answer> * my_knowledge)
				{{{
					this->base = my_knowledge;

					// add all trivial equivalences
					kIterator_lex_graded<answer> kit(base->get_rootptr());
					while(!kit.end()) {
						equivalences.insert(pair<node*, node*>(&*kit, &*kit));
						kit++;
					}
				}}}
				~equivalence_relation()
				{ }

				bool add_if_possible(node * a, node * b)
				{{{
					bool ok;

					ok = add(a, b);

					if(ok) {
						typename set<nodeppair>::iterator ci;
						for(ci = candidates.begin(); ci != candidates.end(); ci++)
							equivalences.insert(pair<node*, node*>(ci->first, ci->second));
					}

					candidates.clear();
					return ok;
				}}}

				set<node*> get_equivalence_class(node * n)
				{{{
					return equivalences.get_equivalence_class(n);
				}}}

				bool are_equivalent(node * a, node * b)
				{{{
					return equivalences.are_equivalent(a,b);
				}}}

				bool is_representative_graded_lex(node * n)
				{{{
					return equivalences.representative_graded_lex(n);
				}}}
			protected:
				set<node*> get_equivalence_class_candidate(node * n)
				{{{
					set<node*> ret;
					typename set<nodeppair>::iterator eqi;

					ret = equivalences.get_equivalence_class(n);
					for(eqi = candidates.begin(); eqi != candidates.end(); eqi++)
						if(n == eqi->first)
							ret.insert(eqi->second);

					return ret;
				}}}
				bool are_candidate_equivalent(node * a, node * b)
				{{{
					if(equivalences.are_equivalent(a,b)) {
						return true;
					} else {
						pair<node*, node*> p;
						p.first = a;
						p.second = b;

						return candidates.find(p) != candidates.end();
					}
				}}}

				bool add(node * a, node * b)
				{{{
					if(!are_candidate_equivalent(a, b)) {
#ifdef RPNI_DEBUG_EQ_CLASSES
						list<int> w1,w2;
						w1 = a->get_word(); w2 = b->get_word();
						printf("\t[ %s , %s ]", word2string(w1).c_str(), word2string(w2).c_str());
#endif
						set<nodeppair> pending_equivalences;
						typename set<nodeppair>::iterator pi;

						set<node*> ca, cb;

						ca = get_equivalence_class_candidate(a);
						cb = get_equivalence_class_candidate(b);

						typename set<node*>::iterator cai, cbi;

						for(cai = ca.begin(); cai != ca.end(); cai++) {
							for(cbi = cb.begin(); cbi != cb.end(); cbi++) {
								if((*cai)->is_answered() && (*cbi)->is_answered()) {
									if((*cai)->get_answer() != (*cbi)->get_answer()) {
										// consistency failure. the requested equivalence is not possible with this sample set.
#ifdef RPNI_DEBUG_EQ_CLASSES
										printf(" bad!\n");
#endif
										return false;
									}
								}

								candidates.insert(nodeppair(*cai, *cbi));
								candidates.insert(nodeppair(*cbi, *cai));

								for(int sigma = 0; sigma < (*cai)->max_child_count() && sigma < (*cbi)->max_child_count(); sigma++) {
									node *r, *s;
									r = (*cai)->find_child(sigma);
									if(r != NULL) {
										s = (*cbi)->find_child(sigma);
										if(s != NULL) {
											pending_equivalences.insert(nodeppair(r, s));
										}
									}
								}
							}
						}
#ifdef RPNI_DEBUG_EQ_CLASSES
						printf("\n");
#endif
						for(pi = pending_equivalences.begin(); pi != pending_equivalences.end(); pi++)
							if(!add(pi->first, pi->second))
								return false;
					}

					return true;
				}}}
		}; // end of equivalence_relation

	protected: // data

	public: // methods
		RPNI(knowledgebase<answer> * base, logger * log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(base);
		}}}
		virtual ~RPNI()
		{{{
			// nothing
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			this->set_alphabet_size(new_asize);
		}}}

		virtual memory_statistics get_memory_statistics()
		{{{
			memory_statistics ret;
			// FIXME
			
			return ret;
		}}}

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
			ret += htonl(learning_algorithm<answer>::ALG_RPNI);

			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			if(it == limit) return false;
			if(ntohl(*it) != 1)
				return false;

			it++; if(it == limit) return false;
			if(ntohl(*it) != learning_algorithm<answer>::ALG_RPNI)
				return false;

			it++;

			return true;
		}}}

		virtual void print(ostream &os)
		{{{
			os << tostring();
		}}}
		virtual string tostring()
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
					(*this->my_logger)(LOGGER_INFO, "RPNI expects a sample-set consisting of pref(S+). "
							"but this sampleset has a non-accepting leaf \"%s\". "
							"you may try anyway, this is just a sidenote.\n", s.c_str());
				}

				if(this->get_alphabet_size() != this->my_knowledge->get_alphabet_size())
					(*this->my_logger)(LOGGER_WARN, "RPNI: differing alphabet size between this (%d) and knowledgebase (%d)!\n",
							this->get_alphabet_size(), this->my_knowledge->get_alphabet_size());

				return true;
			} else {
				return false;
			}
		}}}

		// stubs for counterexample will throw a warning to the logger
		virtual void add_counterexample(list<int>)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "RPNI does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		}}}

	protected:
		bool leaf_is_non_accepting(typename knowledgebase<answer>::node* n, list<int> & sample, bool prefix_accepting = false)
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
			typename knowledgebase<answer>::node * c;

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
			return true;
		}}}
		// derive an automaton from data structure
		virtual bool derive_automaton(bool & t_is_dfa, int & t_alphabet_size, int & t_state_count, set<int> & t_initial, set<int> & t_final, multimap<pair<int, int>, int> & t_transitions)
		{{{
			if(this->get_alphabet_size() != this->my_knowledge->get_alphabet_size())
				(*this->my_logger)(LOGGER_WARN, "RPNI: differing alphabet size between this (%d) and knowledgebase (%d)!\n",
						this->get_alphabet_size(), this->my_knowledge->get_alphabet_size());
			bool ok;
			equivalence_relation eq(this->my_knowledge);

			merge_states(eq);
			(*this->my_logger)(LOGGER_INFO, "RPNI: states merged. constructing automaton...\n");
			ok = this->my_knowledge->equivalence_relation2automaton(eq.equivalences, false,
					t_is_dfa, t_alphabet_size, t_state_count,
					t_initial, t_final, t_transitions);

			t_is_dfa = true;

			return ok;
		}}}
		virtual void merge_states(equivalence_relation & eq)
		{{{
			kIterator_lex_graded<answer> lgo(this->my_knowledge->get_rootptr());
			int lgo_index = 0;

			lgo++;
			lgo_index++;

#ifdef RPNI_DEBUG_EQ_CLASSES
			int iteration = 0;
#endif

			while( ! lgo.end()) {

				kIterator_lex_graded<answer> lgo2(this->my_knowledge->get_rootptr());

				// check that current state is not equivalent to some smaller
				bool skip = false;
				while((&*lgo) != (&*lgo2)) {
					if(eq.are_equivalent(&*lgo, &*lgo2)){
						skip = true;
						break;
					}
					lgo2++;
				}

				if(!skip) {
					// check for all smaller states, if one can be joined with this one
					lgo2.set_root(this->my_knowledge->get_rootptr());
					while(&*lgo != &*lgo2) {
						if(eq.is_representative_graded_lex(&*lgo2)) {
							list<int> w1, w2;
							w1 = lgo->get_word();
							w2 = lgo2->get_word();
#ifdef RPNI_DEBUG_EQ_CLASSES
							printf("\n\n");
#endif
							if(eq.add_if_possible(&*lgo, &*lgo2)) {
								(*this->my_logger)(LOGGER_DEBUG, "RPNI: merge ok:  ( %s , %s )\n",
									word2string(w1).c_str(), word2string(w2).c_str());
#ifdef RPNI_DEBUG_EQ_CLASSES
								char filename[128];
								ofstream file;
								snprintf(filename, 128, "eq-classes-%02d.dot", iteration);
								file.open(filename);
								file << this->my_knowledge->generate_dotfile(eq.equivalences);
								file.close();
								iteration++;
#endif
								break;
							} else {
								(*this->my_logger)(LOGGER_DEBUG, "RPNI: merge bad: ( %s , %s )\n",
									word2string(w1).c_str(), word2string(w2).c_str());
							}

						}
						lgo2++;
					}
				}

				lgo++;
				lgo_index++;
			}
#ifdef RPNI_DEBUG_EQ_CLASSES
			printf("\n");
			set<typename knowledgebase<answer>::node*> representatives, eq_class;
			typename set<typename knowledgebase<answer>::node*>::iterator ri, eqi;
			representatives = eq.equivalences.representatives_graded_lex();
			for(ri = representatives.begin(); ri != representatives.end(); ri++) {
				list<int> word = (*ri)->get_word();
				printf("\tclass [%s]: { ", word2string(word).c_str());
				eq_class = eq.equivalences.get_equivalence_class(*ri);
				for(eqi = eq_class.begin(); eqi != eq_class.end(); eqi++) {
					word = (*eqi)->get_word();
					printf("%s, ", word2string(word).c_str());
				}
				printf("};\n");
			}
			printf("\n");
#endif
		}}}

};


}; // end namespace libalf

#endif // __libalf_algorithm_rpni_h__
