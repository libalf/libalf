/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libAMoRE++
 *
 * libAMoRE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libAMoRE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libAMoRE++.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __amore_nondeterministic_automaton_h__
# define __amore_nondeterministic_automaton_h__

#include <list>
#include <string>
#include <set>

#include <amore++/finite_automaton.h>

#ifdef LIBAMORE_LIBRARY_COMPILATION
# include <amore/nfa.h>
# include <amore/dfa.h>
#else
# define nfa void*
# define dfa void*
#endif

namespace amore {

/*
 * typically, an NFA is created from a regular expression (using amore::rexFromString).
 * the following syntax is required for this function:
 *
 * *  kleene star
 * +  kleene plus
 * ~  unary prefix complement
 * .  concat (may be omitted)
 * &  intersection
 * -  set difference A\B
 * U  union
 *
 * spaces in regex are ignored. for more information, please refer to AMoRE documentation, e.g.
 * ftp://ftp.informatik.uni-kiel.de/pub/kiel/amore/amore.ps.gz
 *
 * the constructor generating an nfa from a regex will "insanitize" your regular expression
 * on the fly, i.e. it will transform any occurence of the commonly used '|' to 'U'
 *
 * the alphabet is mapped from [a..] to [0..] ([1..] in amore-internal representation)
 */

using namespace std;

class nondeterministic_finite_automaton : public finite_automaton {
	private:
		nfa nfa_p;

	public:
		nondeterministic_finite_automaton();
		nondeterministic_finite_automaton(nfa a);
		nondeterministic_finite_automaton(const char *regex, bool &success);
		nondeterministic_finite_automaton(int alphabet_size, const char *regex, bool &success);

		virtual ~nondeterministic_finite_automaton();

		virtual nondeterministic_finite_automaton * clone();

		virtual unsigned int get_state_count();
		virtual unsigned int get_alphabet_size();
		virtual set<int> get_initial_states();
		virtual set<int> get_final_states();
		virtual void set_initial_states(set<int> &states);
		virtual void set_final_states(set<int> &states);
		virtual bool contains_initial_states(set<int> states);
		virtual bool contains_final_states(set<int> states);
		virtual set<int> successor_states(set<int> states);
		virtual set<int> successor_states(set<int> states, int label);
		virtual set<int> predecessor_states(set<int> states);
		virtual set<int> predecessor_states(set<int> states, int label);
		virtual list<int> shortest_run(set<int> from, set<int> &to, bool &reachable);
		virtual bool is_reachable(set<int> &from, set<int> &to);
		virtual list<int> get_sample_word(bool & is_empty);
		virtual bool is_empty();
		virtual bool is_universal();
		virtual bool operator==(finite_automaton &other);
		virtual bool lang_subset_of(finite_automaton &other);
		virtual bool lang_disjoint_to(finite_automaton &other);
		virtual void minimize();
		virtual void lang_complement();
		virtual nondeterministic_finite_automaton * reverse_language();
		virtual nondeterministic_finite_automaton * lang_union(finite_automaton &other);
		virtual finite_automaton * lang_intersect(finite_automaton &other);
		virtual finite_automaton * lang_difference(finite_automaton &other);
		virtual finite_automaton * lang_symmetric_difference(finite_automaton &other);
		virtual nondeterministic_finite_automaton * lang_concat(finite_automaton &other);

		virtual bool is_deterministic();
		virtual nondeterministic_finite_automaton * nondeterminize();
		virtual finite_automaton * determinize();

		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

		// overloading
		virtual bool contains(list<int> &word);


		// new
	public:
		virtual void set_nfa(nfa a);
		virtual nfa get_nfa();

		// get regex for automaton
		virtual string to_regex();

		// apply epsilon-closure to state-set (i.e. include states that may be reached from those via epsilon)
		virtual void epsilon_closure(set<int> & states);
		// apply inverted epsilon-closure to state-set (i.e. include states from whom these states may be reached via epsilon)
		virtual void inverted_epsilon_closure(set<int> & states);

		// rather efficient algorithm for specific language tests
//		virtual bool antichain_universality_test(list<int> counterexample);
		virtual bool antichain_equivalence_test(nondeterministic_finite_automaton &other, list<int> counterexample);
		virtual bool antichain_subset_test(nondeterministic_finite_automaton &other, list<int> counterexample);


	protected:
//		set<set<int> > antichain_universality_cpre(set<int>);
		multimap< int, set<int> > antichain_subset_cpre(multimap< int, set<int> > &stateset, nondeterministic_finite_automaton &other);
		void antichain_subset_cpreN(pair<int, set<int> > &f1, multimap< int, set<int> > &Fn, nondeterministic_finite_automaton &other);

};


}; // end namespace amore

#ifndef LIBAMORE_LIBRARY_COMPILATION
# undef nfa
# undef dfa
#endif

#endif

