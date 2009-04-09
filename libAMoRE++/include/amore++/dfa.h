/* $Id$
 * vim: fdm=marker
 *
 * libAMoRE C++ interface
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __libAMoRE_automata_amore_h__
# define __libAMoRE_automata_amore_h__

#include <list>
#include <string>
#include <set>

#include <amore++/finite_automaton.h>

#ifdef AMORE_LIBRARY_COMPILATION
# include <amore/dfa.h>
#else
# define dfa void*
#endif

namespace amore {

/*
 * at some point you need to call amore::initbuf() before using any amore stuff
 * and amore::freebuf() at the end.


 * typically, an amore automaton is created from a regular expression (using amore::rexFromString).
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


 * attention: stupid amore headers typedef string to be char*
 * thus we have to use "std::string"...
 */

using namespace std;

class dfa : public finite_automaton {
	private:
		dfa dfa_p;

	public:
		dfa();
		dfa(dfa a);
		virtual ~dfa();

		virtual enum automaton_type get_type() {
			return DETERMINISTIC_FINITE_AUTOMATON;
		}
		virtual enum automaton_implementation get_implementation() {
			return IMP_AMORE;
		}

	// from finite_automaton
		virtual dfa * clone();

	// from finite_automaton
		virtual int get_state_count();
		virtual int get_alphabet_size();
		virtual list<int> get_sample_word(bool & is_empty);
		virtual bool is_empty();
		virtual bool operator==(finite_automaton &other);
		virtual bool lang_subset_of(finite_automaton &other);
		virtual bool lang_disjoint_to(finite_automaton &other);
		virtual bool contains(list<int> &word);
		virtual void minimize();
		virtual void lang_complement();
		virtual nfa * lang_union(finite_automaton &other);
		virtual finite_automaton * lang_intersect(finite_automaton &other);
		virtual dfa * lang_difference(finite_automaton &other);
		virtual nfa * lang_symmetric_difference(finite_automaton &other);
		virtual nfa * lang_concat(finite_automaton &other);

		virtual nfa * nondeterminize();
		virtual dfa * determinize();

		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

		virtual bool construct(int alphabet_size, int state_count, list<int> &start, list<int> &final, list<transition> &transitions);

	// new
		virtual void set_dfa(dfa a);
		virtual dfa get_dfa();

	protected:
		virtual bool accepts_suffix(int starting_state, list<int>::iterator suffix_begin, list<int>::iterator suffix_end);
};

}; // end namespace amore

#ifndef AMORE_LIBRARY_COMPILATION
# undef dfa
#endif

#endif

