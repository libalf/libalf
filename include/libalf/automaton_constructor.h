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

#ifndef __libalf_automaton_constructor_h__
# define __libalf_automaton_constructor_h__

#include <list>
#include <set>
#include <string>

namespace libalf {

using namespace std;


class transition {
	public:
		class comparator { public: bool operator() (transition t1, transition t2) {return t1 < t2;} };
	public:
		int source;		// source node
		int label;		// attribute of transition (epsilon == -1)
		int destination;	// destination node

		transition() {
			source = -1;
			label = -1;
			destination = -1;
		}

		bool operator<(transition & t)
		{{{
			if(this->source != t.source)   return (this->source < t.source);
			if(this->destination != t.destination)   return (this->destination < t.destination);
			return (this->label < t.label);
		}}}

		bool operator==(transition &other)
		{{{
			return ((source == other.source) && (label == other.label) && (destination == other.destination));
		}}}
};

inline bool deterministic_transitions(const transition & t1, const transition & t2)
{ return (t1.source != t2.source) || (t1.label != t2.label) || (t1.destination == t2.destination); }

typedef set<transition, transition::comparator> transition_set;

class automaton_constructor {
	public:
		virtual ~automaton_constructor()
		{ };
		virtual bool can_construct_NFA() = 0;
		virtual bool can_construct_DFA() = 0;

		virtual bool construct(bool is_dfa, int alphabet_size, int state_count, set<int> &start, set<int> &final, transition_set &transitions) = 0;
};


class basic_automaton_holder : public automaton_constructor {
	public:
		bool is_dfa;
		int alphabet_size;
		int state_count;
		set<int> start;
		set<int> final;
		transition_set transitions;

	public:
		basic_automaton_holder();
		virtual ~basic_automaton_holder()
		{ };

		virtual bool can_construct_NFA()
		{ return true; };
		virtual bool can_construct_DFA()
		{ return true; };

		virtual void clear();

		virtual bool construct(bool is_dfa, int alphabet_size, int state_count, set<int> &start, set<int> &final, transition_set &transitions);

		virtual bool empty()
		{ return (state_count == 0); };

		virtual basic_string<int32_t> serialize();

		virtual string generate_dotfile();
};


}; // end namespace libalf

#endif

