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
#include <map>

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

class nondeterministic_finite_automaton : public finite_automaton {
	private:
		nfa nfa_p;

	public:
		nondeterministic_finite_automaton();
		nondeterministic_finite_automaton(nfa a);
		nondeterministic_finite_automaton(const char *regex, bool &success);
		nondeterministic_finite_automaton(int alphabet_size, const char *regex, bool &success);

		virtual ~nondeterministic_finite_automaton();

		virtual nondeterministic_finite_automaton * clone() const;

		virtual unsigned int get_state_count() const;
		virtual unsigned int get_alphabet_size() const;
		virtual std::set<int> get_initial_states() const;
		virtual std::set<int> get_final_states() const;
		virtual void set_initial_states(std::set<int> &states);
		virtual void set_final_states(std::set<int> &states);
		virtual bool contains_initial_states(std::set<int> states) const;
		virtual bool contains_final_states(std::set<int> states) const;
		virtual std::set<int> successor_states(std::set<int> states) const;
		virtual std::set<int> successor_states(std::set<int> states, int label) const;
		virtual std::set<int> predecessor_states(std::set<int> states) const;
		virtual std::set<int> predecessor_states(std::set<int> states, int label) const;
		virtual void epsilon_closure(std::set<int> & states) const;
		virtual void inverted_epsilon_closure(std::set<int> & states) const;
		virtual std::list<int> shortest_run(std::set<int> from, std::set<int> &to, bool &reachable) const;
		virtual bool is_reachable(std::set<int> &from, std::set<int> &to) const;
		virtual std::list<int> get_sample_word(bool & is_empty) const;
		virtual bool is_empty() const;
		virtual bool is_universal() const;
		virtual bool operator==(const finite_automaton &other) const;
		virtual bool lang_subset_of(const finite_automaton &other) const;
		virtual bool lang_disjoint_to(const finite_automaton &other) const;
		virtual void minimize();
		virtual void lang_complement();
		virtual nondeterministic_finite_automaton * reverse_language() const;
		virtual nondeterministic_finite_automaton * lang_union(const finite_automaton &other) const;
		virtual finite_automaton * lang_intersect(const finite_automaton &other) const;
		virtual finite_automaton * lang_difference(const finite_automaton &other) const;
		virtual finite_automaton * lang_symmetric_difference(const finite_automaton &other) const;
		virtual nondeterministic_finite_automaton * lang_concat(const finite_automaton &other) const;

		virtual bool is_deterministic() const;
		virtual nondeterministic_finite_automaton * nondeterminize() const;
		virtual finite_automaton * determinize() const;

		virtual std::basic_string<int32_t> serialize() const;
		virtual bool deserialize(std::basic_string<int32_t>::const_iterator &it, std::basic_string<int32_t>::const_iterator limit);

		// overloading
		virtual bool contains(std::list<int> &word) const;

		// new
		virtual void set_nfa(nfa a);
		virtual nfa get_nfa();

		// get regex for automaton
		virtual std::string to_regex() const;

	protected:
//		std::set<std::set<int> > antichain_universality_cpre(std::set<int>);
		std::multimap< int, std::set<int> > antichain_subset_cpre(std::multimap< int, std::set<int> > &stateset, nondeterministic_finite_automaton &other) const;
		void antichain_subset_cpreN(std::pair<int, std::set<int> > &f1, std::multimap< int, std::set<int> > &Fn, nondeterministic_finite_automaton &other) const;

};


}; // end namespace amore

#ifndef LIBAMORE_LIBRARY_COMPILATION
# undef nfa
# undef dfa
#endif

#endif

