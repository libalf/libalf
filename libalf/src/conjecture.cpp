/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <set>
#include <map>
#include <list>
#include <queue>
#include <string>

#ifdef _WIN32
#include <winsock.h>
#include <stdint.h>
#include <stdio.h>
#else
#include <arpa/inet.h>
#endif

#include <stdlib.h>

#include <libalf/conjecture.h>

namespace libalf {

using namespace std;




// for human-readable automaton parser:
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
				ret.push_back(tolower(tmp[i]));
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









simple_automaton::simple_automaton()
{ clear(); }

simple_automaton::~simple_automaton()
{ };

void simple_automaton::clear()
{{{
	valid = false;
	is_deterministic = false;
	alphabet_size = 0;
	state_count = 0;
	initial.clear();
	final.clear();
	transitions.clear();
}}}

bool simple_automaton::is_valid()
{ return valid; }

basic_string<int32_t> simple_automaton::serialize()
{{{
	basic_string<int32_t> ret;

	if(valid) {
		set<int>::iterator sit;
		multimap<pair<int, int>, int>::iterator tit;

		ret += 0; // length, filled in later.
		ret += htonl( is_deterministic ? 1 : 0 );
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
	}

	return ret;
}}}

bool simple_automaton::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
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

	// deterministic?
	it++; if(size <= 0 || limit == it) goto deserialization_failed_fast;
	s = ntohl(*it);
	if(s != 0 && s != 1) goto deserialization_failed_fast;
	is_deterministic = s;

	// alphabet size
	size--, it++; if(size <= 0 || limit == it) goto deserialization_failed_fast;
	alphabet_size = ntohl(*it);
	if(alphabet_size < 1)
		return false;

	// state count
	size--, it++; if(size <= 0 || limit == it) goto deserialization_failed_fast;
	state_count = ntohl(*it);
	if(state_count < 1)
		return false;

	// initial states
	size--, it++; if(size <= 0 || limit == it) goto deserialization_failed_fast;
	count = ntohl(*it);
	if(count < 0)
		return false;

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
	if(count < 0)
		return false;

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
	if(count < 0)
		return false;

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

	valid = true;
	return true;

deserialization_failed:
	initial.clear();
	final.clear();
	transitions.clear();
deserialization_failed_fast:
	alphabet_size = 0;
	state_count = 0;
	valid = false;
	return false;
}}}

string simple_automaton::write()
{{{
	string ret;

	if(valid) {
		char buf[256];
		set<int>::iterator si;
		multimap<pair<int, int>, int>::iterator tri;
		bool first_komma;

		snprintf(buf, 256, "[general]\n"
				   "\tis dfa = %s;\n"
				   "\talphabet size = %d;\n"
				   "\tnumber of states = %d;\n"
				   "[initial states]\n",
				is_deterministic ? "true" : "false", alphabet_size, state_count);
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
	}

	return ret;
}}}

bool simple_automaton::read(string input)
{{{
	bool set_is_det = false, set_alphabet_size = false, set_state_count = false;
	enum section {
		section_none,
		section_general,
		section_initial,
		section_final,
		section_transitions
	} current_section = section_none;

	initial.clear();
	final.clear();
	transitions.clear();

	// split lines
	string line, single, token;
	while(1) {
		line = parser_split_line(input);
		if(input.empty() && line.empty())
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
//				fprintf(stderr, "new single >>%s<<\n", single.c_str());
				if(single[0] == '[') {
					if(single[single.size()-1] != ']') {
						valid = false;
						goto end;
					}

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
								else {
									valid = false;
									goto end;
								}
				} else {
					switch (current_section) {
						case section_none:
							valid = false;
							goto end;
						case section_general:
							{
								string lval, rval;
								char *endptr;
								size_t p = single.find_first_of('=');
								if(p == string::npos) {
									valid = false;
									goto end;
								}
								lval = single.substr(0,p);
								rval = single.substr(p+1, string::npos);
								if(lval == "is dfa") {
									if(rval == "true" || rval == "yes") {
										is_deterministic = true;
										set_is_det = true;
									} else {
										if(rval == "false" || rval == "no") {
											is_deterministic = false;
											set_is_det = true;
										} else {
											valid = false;
											goto end;
										}
									}
								} else {
									if(lval == "alphabet size") {
										set_alphabet_size = true;
										alphabet_size = strtol(rval.c_str(), &endptr, 10);
										if(*endptr) {
											valid = false;
											goto end;
										}
									} else {
										if(lval == "number of states") {
											set_state_count = true;
											state_count = strtol(rval.c_str(), &endptr, 10);
											if(*endptr) {
												valid = false;
												goto end;
											}
										} else {
											valid = false;
											goto end;
										}
									}
								}
							}
							break;
						case section_initial:
							if(!parser_get_set(single, initial)) {
								valid = false;
								goto end;
							}
							break;
						case section_final:
							if(!parser_get_set(single, final)) {
								valid = false;
								goto end;
							}
							break;
						case section_transitions:
							if(!parser_get_transition(single, transitions)) {
								valid = false;
								goto end;
							}
							break;
					}
				}
			}
		}
	}

	valid = set_is_det && set_alphabet_size && set_state_count;

end:
	if(!valid)
		clear();

	return valid;
}}}


string simple_automaton::visualize()
{{{
	string ret;

	if(valid) {
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
	}

	return ret;
}}}

bool simple_automaton::calculate_determinism()
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
		if(former == tri->first) {
			is_deterministic = false;
			return false;
		}

		// epsilon transition?
		if(tri->first.second == -1) {
			is_deterministic = false;
			return false;
		}

		former = tri->first;
	}

	is_deterministic = true;
	return true;
}}}

} // enf of namespace libalf.

