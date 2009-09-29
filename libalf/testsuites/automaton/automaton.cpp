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

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <map>

#include <libalf/alf.h>
#include <libalf/automaton.h>

#include <amore++/deterministic_finite_automaton.h>
#include <amore++/nondeterministic_finite_automaton.h>

#include <liblangen/dfa_randomgenerator.h>
#include <liblangen/nfa_randomgenerator.h>

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;
using namespace amore;
using namespace liblangen;

void test_dfa()
{{{
	bool f_is_dfa;
	int f_alphabet_size, f_state_count;
	set<int> f_initial, f_final;
	multimap<pair<int, int>, int> f_transitions;

	basic_string<int32_t> ser_dfa;
	basic_string<int32_t>::iterator si;

	deterministic_finite_automaton dfa1, dfa2, dfa3;

	// generate random DFA
	dfa_randomgenerator dfa_rg;
	dfa_rg.generate(4, 7, f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);

	cout << "-------\n" << write_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)
	     << "-------\n";

	// serialize raw version
	ser_dfa = serialize_automaton(f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
	// create amore automaton from raw version
	if(!dfa1.construct(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		cout << "construct DFA#1 failed\n";
	// create amore automaton from serialized raw data
	si = ser_dfa.begin();
	if(!dfa2.deserialize(si, ser_dfa.end()))
		cout << "deser DFA#2 failed\n";
	if(si != ser_dfa.end()) {
		cout << "DFA#2 remainder:\n";
		while(si != ser_dfa.end()) {
			printf("%d, ", ntohl(*si));
			si++;
		};
		printf("\n");
	};
	// deserialize serialized version and create amore automaton from this
	si = ser_dfa.begin();
	if(!deserialize_automaton(si, ser_dfa.end(), f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		cout << "deser DFA#3 failed\n";
	if(!dfa3.construct(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		cout << "construct DFA#3 failed\n";

	if(!(dfa1 == dfa2))
		cout << "DFA@1 != DFA#2\n";
	if(!(dfa1 == dfa3))
		cout << "DFA@1 != DFA#3\n";
	if(!(dfa2 == dfa3))
		cout << "DFA@2 != DFA#3\n";

	if(automaton_is_deterministic(f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		cout << "(is deterministic)\n";
	else
		cout << "(is NOT deterministic)\n";

	cout << "DFA test done.\n";
}}}

void test_nfa()
{{{
	bool f_is_dfa;
	int f_alphabet_size, f_state_count;
	set<int> f_initial, f_final;
	multimap<pair<int, int>, int> f_transitions;

	basic_string<int32_t> ser_nfa;
	basic_string<int32_t>::iterator si;

	nondeterministic_finite_automaton nfa1, nfa2, nfa3;

	// generate random NFA
	nfa_randomgenerator nfa_rg;
	nfa_rg.generate(4, 7, 3, 0.3, 0.3, f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);

	cout << "-------\n" << write_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)
	     << "-------\n";

	// serialize raw version
	ser_nfa = serialize_automaton(f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
	// create amore automaton from raw version
	if(!nfa1.construct(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		cout << "construct NFA#1 failed\n";
	// create amore automaton from serialized raw data
	si = ser_nfa.begin();
	if(!nfa2.deserialize(si, ser_nfa.end()))
		cout << "deser NFA#2 failed\n";
	if(si != ser_nfa.end())
		cout << "NFA#2 remainder\n";
	// deserialize serialized version and create amore automaton from this
	si = ser_nfa.begin();
	if(!deserialize_automaton(si, ser_nfa.end(), f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		cout << "deser NFA#3 failed\n";
	if(!nfa3.construct(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		cout << "construct NFA#3 failed\n";

	if(!(nfa1 == nfa2))
		cout << "NFA@1 != NFA#2\n";
	if(!(nfa1 == nfa3))
		cout << "NFA@1 != NFA#3\n";
	if(!(nfa2 == nfa3))
		cout << "NFA@2 != NFA#3\n";

	if(automaton_is_deterministic(f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		cout << "(is deterministic)\n";
	else
		cout << "(is NOT deterministic)\n";

	cout << "NFA test done.\n";
}}}

int main(int argc, char**argv)
{
	test_dfa();
	test_nfa();
}

