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

#include <set>
#include <map>
#include <list>
#include <queue>
#include <string>

#include <arpa/inet.h>

#include <libalf/automaton.h>

namespace libalf {

using namespace std;

string automaton2dotfile(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
{{{
	string ret;
	char buf[64];

	set<int>::iterator sti;
	bool header_written;

	// head
	ret = "digraph automaton {\n"
		"\tgraph[fontsize=8]\n"
		"\trankdir=LR;\n"
		"\tsize=8;\n\n";

	// mark final states
	header_written = false;
	for(sti = final.begin(); sti != final.end(); ++sti) {
		if(!header_written) {
			ret += "\tnode [shape=doublecircle, style=\"\", color=black];";
			header_written = true;
		}
		snprintf(buf, 128, " q%d", *sti);
		ret += buf;
	}
	if(header_written)
		ret += ";\n";

	// default
	ret += "\tnode [shape=circle, style=\"\", color=black];";
	for(int s = 0; s < state_count; s++) {
		if(final.find(s) == final.end()) {
			snprintf(buf, 128, " q%d", s);
			ret += buf;
		}
	}
	ret += ";\n";

	// add non-visible states for arrows to initial states
	header_written = false;
	for(sti = initial.begin(); sti != initial.end(); ++sti) {
		if(!header_written) {
			ret += "\tnode [shape=plaintext, label=\"\", style=\"\"];";
			header_written = true;
		}
		snprintf(buf, 128, " iq%d", *sti);
		ret += buf;
	}
	if(header_written)
		ret += ";\n";

	// and arrows to mark initial states
	for(sti = initial.begin(); sti != initial.end(); ++sti) {
		snprintf(buf, 128, "\tiq%d -> q%d [ color = blue ];\n", *sti, *sti);
		ret += buf;
	}

	// transitions
	multimap<pair<int, int>, int>::iterator ti, tj;
	int s=-1,l=-2,d=-1;
	for(ti = transitions.begin(); ti != transitions.end(); ti++) {
		// skip double entries
		if(s == ti->first.first && l == ti->first.second && d == ti->second)
			continue;
		s = ti->first.first;
		l = ti->first.second;
		d = ti->second;

		snprintf(buf, 64, "\tq%d -> q%d [ label = \"%d\" ];\n", s,d,l);
		buf[63] = 0;
		ret += buf;
	}

	// end
	ret += "};\n";

	return ret;
}}}



bool read_automaton(string input, bool is_dfa, int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
{
	
}

string write_automaton(bool is_dfa, int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
{{{
	string ret;
	char buf[256];
	set<int>::iterator si;
	multimap<pair<int, int>, int>::iterator tri;
	bool first_komma;

	snprintf(buf, 256, "[general]\n"
			   "\tis dfa = %s;\n"
			   "\talphabet size = %d;\n"
			   "\tnumber of states = %d;\n"
			   "[initial states]\n",
			is_dfa ? "true" : "false", alphabet_size, state_count);
	ret += buf;

	first_komma = true;
	for(si = initial.begin(); si != initial.end(); si++) {
		snprintf(buf, 256, "%s%d", first_komma ? "\t" : ", ", *si);
		first_komma = false;
		ret += buf;
	}
	if(first_komma)
		ret += ";";

	ret += "\n[final states]\n";

	first_komma = true;
	for(si = final.begin(); si != final.end(); si++) {
		snprintf(buf, 256, "%s%d", first_komma ? "\t" : ", ", *si);
		first_komma = false;
		ret += buf;
	}
	if(first_komma)
		ret += ";";

	ret += "\n[transitions]\n";

	for(tri = transitions.begin(); tri != transitions.end(); tri++) {
		snprintf(buf, 256, "\t%d, %d, %d;\n", tri->first.first, tri->first.second, tri->second);
		ret += buf;
	}

	ret += buf;

	return ret;
}}}



basic_string<int32_t> serialize_automaton(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
{{{
	basic_string<int32_t> ret;
	set<int>::iterator sit;
	multimap<pair<int, int>, int>::iterator tit;

	ret += 0; // length, filled in later.
	ret += htonl(alphabet_size);
	ret += htonl(state_count);
	ret += htonl(initial.size());
	for(sit = initial.begin(); sit != initial.end(); sit++)
		ret += htonl(*sit);
	ret += htonl(final.size());
	for(sit = final.begin(); sit != final.end(); sit++)
		ret += htonl(*sit);
	ret += htonl(transitions.size());
	for(tit = transitions.begin(); tit != transitions.end(); tit++) {
		ret += htonl(tit->first.first);
		ret += htonl(tit->first.second);
		ret += htonl(tit->second);
	}
	ret[0] = htonl(ret.length() - 1);

	return ret;
}}}

bool deserialize_automaton(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit,
			int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
{{{
	int size;
	int count;
	int s,q;

	initial.clear();
	final.clear();
	transitions.clear();

	if(limit == it)
		goto deserialization_failed_fast;

	size = ntohl(*it);

	// alphabet size
	it++; if(size <= 0 || limit == it) goto deserialization_failed_fast;
	alphabet_size = ntohl(*it);

	// state count
	size--, it++; if(size <= 0 || limit == it) goto deserialization_failed_fast;
	state_count = ntohl(*it);

	// initial states
	size--, it++; if(size <= 0 || limit == it) goto deserialization_failed_fast;
	count = ntohl(*it);

	for(s = 0; s < count; s++) {
		size--, it++; if(size <= 0 || limit == it) goto deserialization_failed;
		q = ntohl(*it);
		if(q >= state_count)
			goto deserialization_failed;
		initial.insert(q);
	}

	// final states
	size--, it++; if(size <= 0 || limit == it) goto deserialization_failed;
	count = ntohl(*it);

	for(s = 0; s < count; s++) {
		size--, it++; if(size <= 0 || limit == it) goto deserialization_failed;
		q = ntohl(*it);
		if(q >= state_count)
			goto deserialization_failed;
		final.insert(q);
	}

	// transitions
	size--, it++; if(size <= 0 || limit == it) goto deserialization_failed;
	count = ntohl(*it);

	for(s = 0; s < count; s++) {
		int32_t src,label,dst;

		size--, it++; if(size <= 0 || limit == it) goto deserialization_failed;
		src = ntohl(*it);
		size--, it++; if(size <= 0 || limit == it) goto deserialization_failed;
		label = ntohl(*it);
		size--, it++; if(size <= 0 || limit == it) goto deserialization_failed;
		dst = ntohl(*it);

		if( (label < -1) || (label >= alphabet_size) || (src < 0) || (src >= state_count) || (dst < 0) || (dst >= state_count) )
			goto deserialization_failed;

		pair<int, int> trid;
		trid.first = src;
		trid.second = label;
		transitions.insert( pair<pair<int, int>, int>( trid, dst ) );
	}

	size--, it++;

	if(size != 0)
		goto deserialization_failed;

	return true;

deserialization_failed:
	initial.clear();
	final.clear();
	transitions.clear();
deserialization_failed_fast:
	alphabet_size = 0;
	state_count = 0;
	return false;
}}}



bool automaton_is_deterministic(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
{{{
	// check transitions for epsilon and double transitions
	multimap<pair<int, int>, int>::iterator tri;

	if(initial.size() > 1)
		return false;

	pair<int, int> former;
	former.first = -1;
	former.second = -1;
	for(tri = transitions.begin(); tri != transitions.end(); ++tri) {
		// two transitions with same (source,label) ?
		if(former == tri->first)
			return false;

		// epsilon transition?
		if(tri->first.second == -1)
			return false;

		former = tri->first;
	}

	return true;
}}}

static bool intersection_empty(set<int> & set1, set<int> & set2)
{{{
	set<int>::iterator s1, s2;
	for(s1 = set1.begin(), s2 = set2.begin(); s1 != set1.end() && s2 != set2.end(); /* nothing */) {
		if(*s1 == *s2)
			return false;
		if(*s1 < *s2)
			s1++;
		else
			s2++;
	}

	return true;
}}}

void epsilon_closure(set<int> & states, multimap<pair<int, int>, int> & transitions)
{{{
	set<int>::iterator sti;
	multimap<pair<int, int>, int>::iterator tri;
	queue<int> new_states;
	int current;

	for(sti = states.begin(); sti != states.end(); ++sti)
		new_states.push(*sti);

	while(!new_states.empty()) {
		current = new_states.front();
		new_states.pop();

		for(tri = transitions.begin(); tri != transitions.end(); ++tri)
			if(tri->first.first == current && tri->first.second == -1)
				if(states.find(tri->second) == states.end()) {
					states.insert(tri->second);
					new_states.push(tri->second);
				}
	}
}}}

bool simulate_automaton(set<int> & current, int label,   set<int> & final, multimap<pair<int, int>, int> & transitions, bool check_epsilon_transitions)
// changes <current> to states reached after run. returns true iff accepting state was reached.
// this version is not very efficient! if you need something efficient, use libAMoRE++.
{{{
	set<int> new_states;

	set<int>::iterator si;
	multimap<pair<int, int>, int>::iterator tri;

	if(check_epsilon_transitions)
		epsilon_closure(current, transitions);

	for(si = current.begin(); si != current.end(); ++si) {
		pair<int, int> sl(*si, label);
		tri = transitions.find(sl);
		if(tri != transitions.end())
			new_states.insert(tri->second);
	}

	if(check_epsilon_transitions)
		epsilon_closure(current, transitions);

	// check if final state was reached.
	return !intersection_empty(current, final);
}}}

bool simulate_automaton(set<int> & current, list<int>::iterator word_begin, list<int>::iterator word_end,   set<int> & final, multimap<pair<int, int>, int> & transitions)
// changes <current> to states reached after run. returns true iff accepting state was reached.
{{{
	bool ret;

	while(word_begin != word_end) {
		epsilon_closure(current, transitions);
		ret = simulate_automaton(current, *word_begin, final, transitions, false);
	}

	epsilon_closure(current, transitions);

	return !intersection_empty(current, final);
}}}

}; // end of namespace libalf

