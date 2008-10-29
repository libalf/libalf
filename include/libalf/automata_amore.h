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

#ifndef __libalf_automata_amore_h__
# define __libalf_automata_amore_h__

#include <list>
#include <string>

namespace libalf {

#include <libalf/automata.h>
// at some point we need to call amore::initbuf() before using any amore stuff
// and amore::freebuf() at the end.

// so we don't have to include amore headers here:
#ifndef __libalf_so_compilation__
typedef void *dfa;
typedef void *nfa;
#else
// AMoRE includes
# include "amore/nfa.h"
# include "amore/dfa.h"
# include "amore/nfa2dfa.h"
# include "amore/dfa2nfa.h"
# include "amore/dfamdfa.h"
# include "amore/testBinary.h"
# include "amore/unaryB.h"
# include "amore/binary.h"
#endif

// attention: stupid amore headers typedef string to be char*
// thus we have to use "std::string"...

using namespace std;

class deterministic_finite_amore_automaton : public deterministic_finite_automaton {
	private:
		dfa dfa_p;

	public:
		deterministic_finite_amore_automaton();
		deterministic_finite_amore_automaton(dfa a);
		virtual ~deterministic_finite_amore_automaton();

		virtual enum automaton_type get_type() {
			return DETERMINISTIC_FINITE_AUTOMATON;
		}
		virtual enum automaton_implementation get_implementation() {
			return IMP_AMORE;
		}

	// from finite_automaton
		virtual deterministic_finite_amore_automaton * clone();
		virtual std::string generate_dotfile();

	// from finite_language_automaton
		virtual bool is_empty();
		virtual list<int> get_sample_word();
		virtual bool operator==(finite_language_automaton &other);
		virtual bool includes(finite_language_automaton &subautomaton);
		virtual bool contains(list<int>);
		virtual void minimize();
		virtual void lang_complement();
		virtual finite_language_automaton * lang_union(finite_language_automaton &other);
		virtual finite_language_automaton * lang_intersect(finite_language_automaton &other);
		virtual finite_language_automaton * lang_difference(finite_language_automaton &other);
		virtual deterministic_finite_amore_automaton * lang_without(finite_language_automaton &other);
		virtual finite_language_automaton * lang_concat(finite_language_automaton &other);

		virtual deterministic_finite_amore_automaton * construct(int alphabet_size, int state_count, list<int> start, list<int> final, list< pair< pair<int, int>, int> > transitions);

	// from deterministic_finite_automaton
		virtual nondeterministic_finite_automaton * nondeterminize();
	// new
		virtual void set_dfa(dfa a);
		virtual dfa get_dfa();
};

class nondeterministic_finite_amore_automaton : public nondeterministic_finite_automaton {
	private:
		nfa nfa_p;

	public:
		nondeterministic_finite_amore_automaton();
		nondeterministic_finite_amore_automaton(nfa a);
		virtual ~nondeterministic_finite_amore_automaton();

		virtual enum automaton_type get_type() {
			return DETERMINISTIC_FINITE_AUTOMATON;
		}
		virtual enum automaton_implementation get_implementation() {
			return IMP_AMORE;
		}

	// from finite_automaton
		virtual nondeterministic_finite_amore_automaton * clone();
		virtual std::string generate_dotfile();

	// from finite_language_automaton
		virtual bool is_empty();
		virtual list<int> get_sample_word();
		virtual bool operator==(finite_language_automaton &other);
		virtual bool includes(finite_language_automaton &subautomaton);
		virtual bool contains(list<int>);
		virtual void minimize();
		virtual void lang_complement();
		virtual finite_language_automaton * lang_union(finite_language_automaton &other);
		virtual finite_language_automaton * lang_intersect(finite_language_automaton &other);
		virtual finite_language_automaton * lang_difference(finite_language_automaton &other);
		virtual deterministic_finite_amore_automaton * lang_without(finite_language_automaton &other);
		virtual finite_language_automaton * lang_concat(finite_language_automaton &other);

		virtual nondeterministic_finite_amore_automaton * construct(int alphabet_size, int state_count, list<int> start, list<int> final, list< pair< pair<int, int>, int> > transitions);

	// from deterministic_finite_automaton
		virtual deterministic_finite_automaton * determinize();
	// new
		virtual void set_nfa(nfa a);
		virtual nfa get_nfa();
};


}; // end namespace libalf

#endif

