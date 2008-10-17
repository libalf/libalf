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

namespace libalf {

#include <libalf/automata.h>
// at some point we need to call amore::initbuf() before using any amore stuff
// and amore::freebuf() at the end.

// so we don't have to include <amore/ext.h> :
#ifndef __libalf_library_compilation__
typedef dfa void*;
typedef nfa void*;
#endif

class automata_amore : public automata {
	private:
		dfa dfa_p;
		nfa nfa_p;

	public:
		automata_amore();

		automata_amore(enum automata_type type);

		virtual ~automata();

		virtual set_nfa(nfa a);

		virtual set_dfa(dfa a);

		virtual enum automata_type get_type();

		virtual automata* clone();

		virtual bool is_empty();

		virtual list<int> get_sample_word();

		// == will also nfa2dfa both automatas
		virtual bool operator==(automata &other);

		virtual bool includes(automata &subautomata);

		virtual bool is_subset_of(automata &superautomata);

		virtual bool contains(list<int>);

		virtual void make_deterministic();

	protected:
		virtual void make_undeterministic();

	public:
		virtual void minimize();

		virtual void lang_complement();

		virtual automata* lang_union(automata &other);

		virtual automata* lang_intersect(automata &other);

		virtual automata* lang_difference(automata &other);

		virtual automata* lang_without(automata &other);

		virtual automata* lang_concat(automata &other);

	protected:
		virtual void clear_automatas();
}


}; // end namespace libalf

#endif

