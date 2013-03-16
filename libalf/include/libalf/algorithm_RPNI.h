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
 * (c) 2008,2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *                and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *     and Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */


/*
 * RPNI (regular positive, negative inference) is an offline learning algorithm,
 * described in e.g.
 *	* P. Graćıa and J. Oncina: "Inferring regular languages in polynomial update time"
 *	* D. Neider: "Learning Automata for Streaming XML Documents" [1]
 *
 * NOTE: this version does only support bool as <answer>.
 */


#ifndef __libalf_algorithm_rpni_h__
# define __libalf_algorithm_rpni_h__

#include <stdio.h>

#include <set>
#include <list>
#include <string>
#include <ostream>
#include <fstream>

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

template <class answer>
class RPNI : public learning_algorithm<answer> {
	public:	// types
		class equivalence_relation {
			public: // types
				typedef typename knowledgebase<answer>::node node;
				typedef std::pair<node*, node*> nodeppair;
			public: // data
				knowledgebase<answer> * base;
				typename knowledgebase<answer>::equivalence_relation equivalences;
			protected:
				std::set<nodeppair> candidates;

			public: // member functions
				equivalence_relation(knowledgebase<answer> * my_knowledge)
				{{{
					this->base = my_knowledge;

					// add all trivial equivalences
					kIterator_lex_graded<answer> kit(base->get_rootptr());
					while(!kit.end()) {
						equivalences.insert(std::pair<node*, node*>(&*kit, &*kit));
						kit++;
					}
				}}}
				~equivalence_relation()
				{ }

				bool add_if_possible(node * a, node * b, RPNI * callback)
				{{{
					bool ok;

					ok = add(a, b);

					if(ok) 
						ok = callback->consistency_check(equivalences, candidates);

					candidates.clear();
					return ok;
				}}}

				std::set<node*> get_equivalence_class(node * n)
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
				std::set<node*> get_equivalence_class_candidate(node * n)
				{{{
					std::set<node*> ret;
					typename std::set<nodeppair>::iterator eqi;

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
						std::pair<node*, node*> p;
						p.first = a;
						p.second = b;

						return candidates.find(p) != candidates.end();
					}
				}}}

				bool add(node * a, node * b)
				{{{
					if(!are_candidate_equivalent(a, b)) {
#ifdef RPNI_DEBUG_EQ_CLASSES
						std::list<int> w1,w2;
						w1 = a->get_word(); w2 = b->get_word();
						printf("\t[ %s , %s ]", word2string(w1).c_str(), word2string(w2).c_str());
#endif
						std::set<nodeppair> pending_equivalences;
						typename std::set<nodeppair>::iterator pi;

						std::set<node*> ca, cb;

						ca = get_equivalence_class_candidate(a);
						cb = get_equivalence_class_candidate(b);

						typename std::set<node*>::iterator cai, cbi;

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


		/** standard RPNI does only check that the equivalence class is consistent.
		 * if your instance requires specific extra checks, overload this function.
		 * if the candidates are ok, you MUST then merge the candidates manually into the equivalences,
		 * as seen in this function, and return true.
		 * otherwise return false.
		 *
		 * if you want to do that, use the RPNI_extended_consistencies class below!
		 */
		virtual bool consistency_check(typename knowledgebase<answer>::equivalence_relation & equivalences, const std::set<typename equivalence_relation::nodeppair> & candidates)
		{
			typename std::set<typename equivalence_relation::nodeppair>::iterator ci;

			for(ci = candidates.begin(); ci != candidates.end(); ci++)
				equivalences.insert(std::pair<typename equivalence_relation::node*, typename equivalence_relation::node*>(ci->first, ci->second));

			return true;
		}

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

		virtual enum learning_algorithm_type get_type() const
		{ return ALG_RPNI; };

		virtual enum learning_algorithm_type get_basic_compatible_type() const
		{ return ALG_RIVEST_SCHAPIRE; };

		virtual void increase_alphabet_size(int new_asize)
		{{{
			this->set_alphabet_size(new_asize);
		}}}

		virtual void generate_statistics(void)
		{
			// FIXME
			
		}

		virtual bool sync_to_knowledgebase()
		{{{
			return true;
		}}}

		virtual bool supports_sync() const
		{{{
			return true;
		}}}

		virtual std::basic_string<int32_t> serialize() const
		{{{
			std::basic_string<int32_t> ret;

			// we don't have any internal, persistent data
			ret += ::serialize(1); // size
			ret += ::serialize(ALG_RPNI);

			return ret;
		}}}
		virtual bool deserialize(serial_stretch & serial)
		{{{
			int s;

			if(!::deserialize(s, serial)) return false;
			if(s != 1) return false;
			if(!::deserialize(s, serial)) return false;

			return (s == ALG_RPNI);
		}}}

		virtual void print(std::ostream &os) const
		{{{
			os << "RPNI does not have any persistent data.\n";
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
				std::list<int> sample;
				if(leaf_is_non_accepting(this->my_knowledge->get_rootptr(), sample)) {
					(*this->my_logger)(LOGGER_INFO, "RPNI expects a sample-set consisting of pref(S+). "
							"but this sampleset has a non-accepting leaf \"%s\". "
							"you may try anyway, this is just a sidenote.\n", word2string(sample).c_str());
				}

				if(this->get_alphabet_size() != this->my_knowledge->get_largest_symbol())
					(*this->my_logger)(LOGGER_WARN, "RPNI: differing alphabet size between this (%d) and knowledgebase (%d)!\n",
							this->get_alphabet_size(), this->my_knowledge->get_largest_symbol());

				return true;
			} else {
				return false;
			}
		}}}

		// stubs for counterexample will throw a warning to the logger
		virtual bool add_counterexample(std::list<int>)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "RPNI does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
			return false;
		}}}

	protected:
		bool leaf_is_non_accepting(typename knowledgebase<answer>::node* n, std::list<int> & sample, bool prefix_accepting = false)
		// check if all leafs (i.e. states that have no suffixes that either accept or reject) accept
		{{{
			std::list<int> w;
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
		// derive an automaton and return it
		virtual conjecture * derive_conjecture()
		{{{
			finite_automaton * ret = new finite_automaton;

			if(this->my_knowledge->count_answers() == 0) {
				(*this->my_logger)(LOGGER_WARN, "RPNI: you started an offline-algorithm with an empty knowledgebase. that does not make very much sense, does it?\n");
				// return automaton for empty language
				/* Florian's fix*/
				ret->input_alphabet_size = this->get_alphabet_size();
				//ret->input_alphabet_size = 1;
				/* End Florian's fix*/
				ret->state_count = 1;
				ret->set_all_non_accepting();
				ret->initial_states.insert(0);
				ret->is_deterministic = true;
				ret->valid = true;
				return ret;
			}
			
			if(this->get_alphabet_size() != this->my_knowledge->get_largest_symbol())
				(*this->my_logger)(LOGGER_WARN, "RPNI: differing alphabet size between this (%d) and knowledgebase (%d)!\n",
						this->get_alphabet_size(), this->my_knowledge->get_largest_symbol());
			bool ok;
			equivalence_relation eq(this->my_knowledge);

			merge_states(eq);
			(*this->my_logger)(LOGGER_INFO, "RPNI: states merged. constructing automaton...\n");
			ok = this->my_knowledge->equivalence_relation2automaton(eq.equivalences, false, *ret);

			ret->is_deterministic = true;

			/* 
			 * Florian's fix
			 * Set alphabet size the desired alphabet size even if the sample
			 * contains only symbols smaller than the desired one.
			 */
			if(ret->input_alphabet_size < this->get_alphabet_size()) {
				ret->input_alphabet_size = this->get_alphabet_size();
			}
			// End Florian's fix
	
			if(!ok) {
				delete ret;
				ret = NULL;
			} else {
				ret->valid = true;
			}
			
			return ret;
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
							std::list<int> w1, w2;
							w1 = lgo->get_word();
							w2 = lgo2->get_word();
#ifdef RPNI_DEBUG_EQ_CLASSES
							printf("\n\n");
#endif
							if(eq.add_if_possible(&*lgo, &*lgo2, this)) {
								(*this->my_logger)(LOGGER_DEBUG, "RPNI: merge ok:  ( %s , %s )\n",
									word2string(w1).c_str(), word2string(w2).c_str());
#ifdef RPNI_DEBUG_EQ_CLASSES
								char filename[128];
								std::ofstream file;
								snprintf(filename, 128, "eq-classes-%02d.dot", iteration);
								file.open(filename);
								file << this->my_knowledge->visualize(eq.equivalences);
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
			std::set<typename knowledgebase<answer>::node*> representatives, eq_class;
			typename std::set<typename knowledgebase<answer>::node*>::iterator ri, eqi;
			representatives = eq.equivalences.representatives_graded_lex();
			for(ri = representatives.begin(); ri != representatives.end(); ri++) {
				std::list<int> word = (*ri)->get_word();
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


/* implementation of RPNI with extra consistency check in recursive merging of states */
template <class answer>
class RPNI_extended_consistencies : public RPNI<answer>
{
public:
	virtual bool do_extended_consistency_check(typename knowledgebase<answer>::equivalence_relation & equivalences) = 0;
	// TODO: do your specific check here.

	virtual bool consistency_check(typename knowledgebase<answer>::equivalence_relation & equivalences, const std::set<typename RPNI<answer>::equivalence_relation::nodeppair> & candidates)
	{
		typename std::set<typename RPNI<answer>::equivalence_relation::nodeppair>::iterator ci;
		typename knowledgebase<answer>::equivalence_relation tmp_equivalences = equivalences;

		for(ci = candidates.begin(); ci != candidates.end(); ci++)
			tmp_equivalences.insert(std::pair<typename RPNI<answer>::node*, typename RPNI<answer>::node*>(ci->first, ci->second));

		if(do_extended_consistency_check(tmp_equivalences)) {
			equivalences.swap(tmp_equivalences);
			return true;
		} else {
			return false;
		}
	}
};

}; // end namespace libalf

#endif // __libalf_algorithm_rpni_h__

