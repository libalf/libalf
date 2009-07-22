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


/*
 * RPNI (regular positive, negative inference) is an offline learning algorithm,
 * described in e.g.
 *	* P. Graćıa and J. Oncina. Inferring regular languages in polynomial update time
 *	* D. Neider, Learning Automata for Streaming XML Documents
 */


#ifndef __libalf_algorithm_rpni_h__
# define __libalf_algorithm_rpni_h__

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
			public: // data
				knowledgebase<answer> * base;
				set<pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> > equivalences;
			protected:
				set<pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> > candidates;

			public: // member functions
				equivalence_relation(knowledgebase<answer> * my_knowledge)
				{{{
					knowledgebase<answer>::node * n;

					this->base = my_knowledge;

					// add all trivial equivalences
					queue<knowledgebase<answer>::node*> fifo;
					fifo.push(base->get_rootptr());
					while(!fifo.empty()) {
						pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> p;

						knowledgebase<answer>::node * n = fifo.front();
						fifo.pop();

						p.first = n;
						p.second = n;
						equivalences.insert(p);

						vector<knowledgebase<answer>::node*>::iterator ci;
						for(ci = n->children.begin(); ci != n->children.end(); ci++)
							if(*ci)
								fifo.push(*ci);
					}
				}}}
				equivalence_relation(equivalence_relation & copy_from)
				{ *this = copy_from; }
				~equivalence_relation()
				{ }

				equivalence_relation operator=(equivalence_relation & copy_from)
				{{{
					base = copy_from->base;
					equivalences = copy_from->equivalences;
					candidates = copy_from->candidates;
				}}}

				bool add_if_possible(knowledgebase<answer>::node * a, knowledgebase<answer>::node * b)
				{{{
					bool ok;

					ok = add(a, b);

					if(ok) {
						set<pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> >::iterator ci;
						for(ci = candidates.begin(); ci != candidates.end(); ci++)
							equivalences.insert(*ci);
					}

					candidates.clear();
					return ok;
				}}}

				set<knowledgebase<answer>::node*> get_equivalence_class(knowledgebase<answer>::node * n)
				{{{
					set<knowledgebase<answer>::node*> ret;
					set<PAIR<KNOWLEDGEBASE<ANSWER>::NODE*, KNOWLEDGEBASE<answer>::node*> >::iterator eqi;

					for(eqi = equivalences.begin(); eqi != equivalences.end(); eqi++)
						if(n == eqi->first)
							ret.insert(eqi->second);

					return ret;
				}}}

				bool are_equivalent(knowledgebase<answer>node * a, knowledgebase<answer>::node * b)
				{{{
					pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> p;
					p.first = a;
					p.second = b;

					return equivalences.find(p) != equivalences.end();
				}}}
			protected:
				set<knowledgebase<answer>::node*> get_equivalence_class_candidate(knowledgebase<answer>::node * n)
				{{{
					set<knowledgebase<answer>::node*> ret;
					set<pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> >::iterator eqi;

					for(eqi = equivalences.begin(); eqi != equivalences.end(); eqi++)
						if(n == eqi->first)
							ret.insert(eqi->second);
					for(eqi = candidates.begin(); eqi != candidates.end(); eqi++)
						if(n == eqi->first)
							ret.insert(eqi->second);

					return ret;
				}}}
				bool are_candidate_equivalent(knowledgebase<answer>node * a, knowledgebase<answer>::node * b)
				{{{
					pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> p;
					p.first = a;
					p.second = b;

					if(equivalences.find(p) != equivalences.end())
						return true;
					return candidates.find(p) != candidates.end();
				}}}

				bool add(knowledgebase<answer>::node * a, knowledgebase<answer>::node * b)
				{{{
					if(!are_candidate_equivalent(a, b)) {
						set< pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> > pending_equivalences;
						set< pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> >::iterator pi;


						set<knowledgebase<answer>::node*> ca, cb;

						ca = get_equivalence_class_candidate(a);
						cb = get_equivalence_class_candidate(b);

						set<knowledgebase<answer>::node*> cai, cbi;

						for(cai = ca.begin(); cai != ca.end(); cai++) {
							for(cbi = cb.begin(); cbi != cb.end(); cbi++) {
								pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> p;

								if((*cai)->is_answered() && (*cbi)->is_answered())
									if((*cai)->get_answer() != (*cbi)->get_answer()) {
										// consistency failure. the requested equivalence
										// is not possible with this sample set.
										return false;
									}

								p.first = *cai;
								p.second = *cbi;
								candidates.insert(p);

								p.first = *cbi;
								p.second = *cai;
								candidates.insert(p);

								for(int sigma = 0; sigma < base->get_alphabet_size(); sigma++) {
									knowledgebase<answer>::node * r,s;
									r = (*cai)->children[sigma];
									s = (*cbi)->children[sigma];
									if(r != NULL && s != NULL) {
										pair<knowledgebase<answer>::node*, knowledgebase<answer>::node*> q;
										q.first = r;
										q.second = s;
										pending_equivalences.insert(q);
									}
								}
							}
						}

						for(pi = pending_equivalences.begin(); pi != pending_equivalences.end(); pi++)
							if(!add(pi->first, pi->second))
								return false;
					}
				}}}

		};

	protected: // data

	public: // methods
		RPNI()
		{
			initial = NULL;
		}
		virtual ~RPNI()
		{
			discard_tree();
		}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			this->set_alphabet_size(new_asize);
		}}}

		virtual void get_memory_statistics(statistics & stats)
		{ /* FIXME: maybe keep some stats from last run? */ }

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
		{
		}
		virtual string tostring()
		{
		}

		// conjecture is always ready if there is a non-empty knowledgebase
		virtual bool conjecture_ready()
		{{{
			return ( (this->my_knowledge != NULL) && (this->my_knowledge->count_answers() > 0) );
		}}}

		// stubs for counterexample will throw a warning to the logger
		virtual void add_counterexample(list<int>)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "RPNI does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		}}}

	protected:
		virtual bool complete()
		{{{
			// we're offline.
			return true;
		}}}
		// derive an automaton from data structure
		virtual bool derive_automaton(bool & t_is_dfa, int & t_alphabet_size, int & t_state_count, set<int> & t_initial, set<int> & t_final, multimap<pair<int, int>, int> & t_transitions)
		{
			equivalence_relation eq(my_knowledge);

			if(!merge_states(eq))
				return false;

			// now construct automaton from that
			
			
			
		}
		virtual bool merge_states(equivalence_relation & eq)
		{{{
			kIterator_lex_graded lgo(my_knowledge->get_rootptr());
			int lgo_index = 0;

			lgo++;
			lgo_index++;

			while( ! lgo.end()) {

				kIterator_lex_graded lgo2(my_knowledge->get_rootptr());

				// check that current state is not equivalent to some smaller
				bool skip = false;
				while(*lgo != *lgo2) {
					if(eq.are_equivalent(*lgo, *lgo2)){
						skip = true;
						break;
					}
					lgo2++;
				}

				if(!skip) {
					// check for all smaller states, if one can be joined with this one
					lgo2.set_root(my_knowledge->get_rootptr());
					while(*lgo != *lgo2) {
						// FIXME: we should only do this once for each equivalence class,
						// not for each node.

						if(eq.add_if_possible(*lgo, *lgo2))
							break;

						lgo2++;
					}
				}

				lgo++;
				lgo_index++;
			}
		}}}

};


}; // end namespace libalf

#endif // __libalf_algorithm_rpni_h__

