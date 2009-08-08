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

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <stdlib.h>

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


static string parser_split_line(string & blob)
{{{
	int p;
	string tmp, ret;

	// split first line off.
	p = blob.find_first_of('\n');
	tmp = blob.substr(0, p);
	blob = blob.substr(p+1, string::npos);

	// remove comments
	tmp = tmp.substr(0, tmp.find_first_of('#'));

	// strip superficial whitespace et al
	bool front=true;
	bool white=false;
	for(size_t i = 0; i < tmp.size(); i++) {
		switch (tmp[i]) {
			case ' ':
			case '\f':
			case '\n':
			case '\r':
			case '\t':
			case '\v':
				if(!front && !white) {
					ret.push_back(' ');
					white = true;
				}
				break;
			case ';':
			case '=':
			case ',':
			case '[':
			case ']':
				if(front) {
					ret.push_back(tmp[i]);
				} else {
					if(white) {
						ret[ret.size()-1] = tmp[i];
					} else {
						ret.push_back(tmp[i]);
					}
				}
				front = true;
				white = false;
				break;
			default:
				if(isupper(tmp[i]))
					ret.push_back(tmp[i] - 'A' + 'a');
				else
					ret.push_back(tmp[i]);
				front = false;
				white = false;
				break;
		}
	}

	// remove white-space at EOL
	if(white)
		ret = ret.substr(0, ret.size()-1);

	// if this line is empty, try next line
	if(ret.empty() && !blob.empty())
		return parser_split_line(blob);
	else
		return ret;
}}}
static bool parser_get_set(string single, set<int>& s)
{{{
	while(1) {
		string num;
		size_t p;
		p = single.find_first_of(',');
		num = single.substr(0, p);
		if(p != string::npos) {
			single = single.substr(p+1, string::npos);
		} else {
			single="";
		}
		if(num == "") {
			break;
		} else {
			int n;
			char * endptr;
			n = strtol(num.c_str(), &endptr, 10);
			if(*endptr)
				return false;
			s.insert(n);
		}
	}

	return true;
}}}
static bool parser_get_transition(string single, multimap<pair<int, int>, int> & transitions)
{{{
	string tok;
	size_t p;
	char *endptr;
	pair<pair<int, int>, int> tr;

	// FROM
	p = single.find_first_of(',');
	tok = single.substr(0,p);
	if(p == string::npos)
		return false;
	single = single.substr(p+1, string::npos);

	tr.first.first = strtol(tok.c_str(), &endptr, 10);
	if(*endptr)
		return false;

	// LABEL
	p = single.find_first_of(',');
	tok = single.substr(0,p);
	if(p == string::npos)
		return false;
	single = single.substr(p+1, string::npos);

	tr.first.second = strtol(tok.c_str(), &endptr, 10);
	if(*endptr)
		return false;

	// TO
	tok = single;

	tr.second = strtol(tok.c_str(), &endptr, 10);
	if(*endptr)
		return false;

	transitions.insert(tr);

	return true;
}}}

bool read_automaton(string input, bool & is_dfa, int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
{{{
	bool set_is_dfa = false, set_alphabet_size = false, set_state_count = false;
	enum section {
		section_none,
		section_general,
		section_initial,
		section_final,
		section_transitions
	} current_section = section_none;

	// split lines
	string line, single, token;
	while(1) {
		line = parser_split_line(input);
		if(input.empty())
			break;

		// split semi-colons
		while(1) {
			size_t p;
			p = line.find_first_of(';');
			single = line.substr(0, p);
			line = (p == string::npos) ? "" : line = line.substr(p+1, string::npos);

			if(single.empty()) {
				if(line.empty())
					break;
			} else {
//				printf("new single >>%s<<\n", single.c_str());
				if(single[0] == '[') {
					if(single[single.size()-1] != ']')
						return false;

					single = single.substr(1, single.size()-2);
					if(single == "general")
						current_section = section_general;
					else
						if(single == "initial states")
							current_section = section_initial;
						else
							if(single == "final states")
								current_section = section_final;
							else
								if(single == "transitions")
									current_section = section_transitions;
								else
									return false;
				} else {
					switch (current_section) {
						case section_none:
							return false;
						case section_general:
							{
								string lval, rval;
								char *endptr;
								size_t p = single.find_first_of('=');
								if(p == string::npos)
									return false;
								lval = single.substr(0,p);
								rval = single.substr(p+1, string::npos);
								if(lval == "is dfa") {
									if(rval == "true" || rval == "yes") {
										is_dfa = true;
										set_is_dfa = true;
									} else {
										if(rval == "false" || rval == "no") {
											is_dfa = false;
											set_is_dfa = true;
										} else {
											return false;
										}
									}
								} else {
									if(lval == "alphabet size") {
										set_alphabet_size = true;
										alphabet_size = strtol(rval.c_str(), &endptr, 10);
										if(*endptr)
											return false;
									} else {
										if(lval == "number of states") {
											set_state_count = true;
											state_count = strtol(rval.c_str(), &endptr, 10);
											if(*endptr)
												return false;
										} else {
											return false;
										}
									}
								}
							}
							break;
						case section_initial:
							if(!parser_get_set(single, initial))
								return false;
							break;
						case section_final:
							if(!parser_get_set(single, final))
								return false;
							break;
						case section_transitions:
							if(!parser_get_transition(single, transitions))
								return false;
							break;
					}
				}
			}
		}
	}

	return (set_is_dfa && set_alphabet_size && set_state_count);
}}}

string write_automaton(bool is_dfa, int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
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
	if(!first_komma)
		ret += ";";

	ret += "\n[final states]\n";

	first_komma = true;
	for(si = final.begin(); si != final.end(); si++) {
		snprintf(buf, 256, "%s%d", first_komma ? "\t" : ", ", *si);
		first_komma = false;
		ret += buf;
	}
	if(!first_komma)
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

/*
static bool intersection_empty(set<int> & set1, set<int> & set2)
{{{
	set<int>::iterator s1, s2;
	for(s1 = set1.begin(), s2 = set2.begin(); s1 != set1.end() && s2 != set2.end(); ;;;) {
		if(*s1 == *s2)
			return false;
		if(*s1 < *s2)
			s1++;
		else
			s2++;
	}

	return true;
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
*/

}; // end of namespace libalf

