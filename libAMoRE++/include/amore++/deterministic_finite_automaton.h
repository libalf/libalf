/* $Id$
 * vim: fdm=marker
 *
 * amore++
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __amore_deterministic_automaton_h__
# define __amore_deterministic_automaton_h__

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
 * instead of numbers, used within libalf to represent characters,
 * the corresponding chars from itoc[] are used, as declared e.g. in <amore/vars.h>. it is possible
 * to declare your own itoc. please note that, if not including <amore/vars.h>, you will have to
 * define
 *	jmp_buf _jmp;
 *	char dummy[7];
 *	char itoc[28] = "@abcdefghijklmnopqrstuvwxyz";
 * or alike, as libAMoRE requires these symbols.
 * (watch out for linker errors with missing symbols like these)


 * attention: stupid amore headers typedef string to be char*
 * thus we have to use "std::string"...
 */

using namespace std;

class deterministic_finite_automaton : public finite_automaton {
	private:
		dfa dfa_p;

	public:
		deterministic_finite_automaton();
		deterministic_finite_automaton(dfa a);
		virtual ~deterministic_finite_automaton();

		virtual deterministic_finite_automaton * clone();

		virtual int get_state_count();
		virtual int get_alphabet_size();
		virtual std::set<int> get_initial_states();
		virtual std::set<int> get_final_states();
		virtual void set_initial_states(std::set<int> &states);
		virtual void set_final_states(std::set<int> &states);
		virtual list<int> shortest_run(set<int> from, set<int> &to, bool &reachable);
		virtual bool is_reachable(set<int> &from, set<int> &to);
		virtual list<int> get_sample_word(bool & is_empty);
		virtual bool is_empty();
		virtual bool operator==(finite_automaton &other);
		virtual bool lang_subset_of(finite_automaton &other);
		virtual bool lang_disjoint_to(finite_automaton &other);
		virtual std::set<int> transition(std::set<int> from, int label);
		virtual bool contains(list<int> &word);
		virtual void minimize();
		virtual void lang_complement();
		virtual finite_automaton * lang_union(finite_automaton &other);
		virtual finite_automaton * lang_intersect(finite_automaton &other);
		virtual deterministic_finite_automaton * lang_difference(finite_automaton &other);
		virtual finite_automaton * lang_symmetric_difference(finite_automaton &other);
		virtual finite_automaton * lang_concat(finite_automaton &other);

		virtual bool is_deterministic();
		virtual finite_automaton * nondeterminize();
		virtual deterministic_finite_automaton * determinize();

		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

		virtual bool construct(int alphabet_size, int state_count, std::set<int> &initial, std::set<int> &final, multimap<pair<int,int>, int> &transitions);

	// new
		virtual void set_dfa(dfa a);
		virtual dfa get_dfa();
};

}; // end namespace amore

#ifndef LIBAMORE_LIBRARY_COMPILATION
# undef nfa
# undef dfa
#endif

#endif

