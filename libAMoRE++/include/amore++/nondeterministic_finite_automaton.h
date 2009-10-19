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
 * you will need a standard itoc[] for this constructor to work (see next paragraph)


 * instead of numbers, used within libalf to represent characters,
 * the corresponding chars from itoc[] are used, as declared e.g. in <amore/vars.h>. it is possible
 * to declare your own itoc. please note that, if not including <amore/vars.h>, you will have to
 * define
 *	jmp_buf _jmp;
 *	char dummy[7];
 *	char itoc[28] = "@abcdefghijklmnopqrstuvwxyz";
 * or alike, as libAMoRE requires these symbols.
 * (watch out for linker errors with missing symbols like these)
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
		virtual set<int> predecessor_states(set<int> states);
		virtual list<int> shortest_run(set<int> from, set<int> &to, bool &reachable);
		virtual bool is_reachable(set<int> &from, set<int> &to);
		virtual list<int> get_sample_word(bool & is_empty);
		virtual bool is_empty();
		virtual bool is_universal();
		virtual bool operator==(finite_automaton &other);
		virtual bool lang_subset_of(finite_automaton &other);
		virtual bool lang_disjoint_to(finite_automaton &other);
		virtual set<int> transition(set<int> from, int label);
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
		virtual void set_nfa(nfa a);
		virtual nfa get_nfa();
		virtual string to_regex();
		virtual void epsilon_closure(set<int> & states);
		virtual void inverted_epsilon_closure(set<int> & states);

};


}; // end namespace amore

#ifndef LIBAMORE_LIBRARY_COMPILATION
# undef nfa
# undef dfa
#endif

#endif

