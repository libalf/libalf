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
#include <libalf/serialize.h>

namespace libalf {

using namespace std;



// helper-functions for human-readable parser:
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




conjecture::conjecture()
{ valid = false; };
bool conjecture::is_valid()
{ return valid; };
void conjecture::clear()
{ valid = false; };
bool conjecture::calc_validity()
{ return valid; };


finite_state_machine::finite_state_machine()
{{{
	is_deterministic = false;
	input_alphabet_size = 0;
	output_alphabet_size = 0;
	state_count = 0;
	final_output = false;
	omega = false;

}}}
void finite_state_machine::clear()
{{{
	conjecture::clear();
	is_deterministic = false;
	input_alphabet_size = 0;
	output_alphabet_size = 0;
	state_count = 0;
	final_output = false;
	omega = false;

	initial_states.clear();
}}}
bool finite_state_machine::calc_validity()
{{{
	if(!conjecture::calc_validity())
		return false;

	if(input_alphabet_size<=0 || output_alphabet_size<=0 || state_count<=0 || initial_states.empty()) {
		valid = false;
		return false;
	}

	return true;
}}}



moore_machine::moore_machine()
{ }
void moore_machine::clear()
{{{
	finite_state_machine::clear();

	output_mapping.clear();
	transitions.clear();
}}}
bool moore_machine::calc_validity()
{{{
	if(!finite_state_machine::calc_validity())
		return false;

	if(output_mapping.size() != this->state_count) {
		valid = false;
		return false;
	};

	vector<int>::iterator vi;
	for(vi = output_mapping.begin(); vi != output_mapping.end(); ++vi) {
		if(*vi < 0 || *vi >= this->output_alphabet_size) {
			valid = false;
			return false;
		}
	}

	multimap<pair<int, int>, int>::iterator tri;
	for(tri = transitions.begin(); tri != transitions.end(); ++tri) {
		if(   tri->first.first < 0 || tri->first.first >= this->state_count
		   || tri->first.second < -1 || tri->first.second >= this->input_alphabet_size // allow epsilon-transitions (label = -1)
		   || tri->second < 0 || tri->second >= this->state_count) {
			valid = false;
			return false;
		}
	}

	return true;
}}}
bool moore_machine::calc_determinism()
{{{
	// check transitions for epsilon and double transitions
	multimap<pair<int, int>, int>::iterator tri;

	if(this->initial_states.size() > 1) {
		is_deterministic = false;
		return false;
	}

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
basic_string<int32_t> moore_machine::serialize()
{{{
	basic_string<int32_t> ret;

	if(this->valid) {
		ret += 0; // length, filled in later;
		ret += ::serialize(this->is_deterministic);
		ret += ::serialize(this->input_alphabet_size);
		ret += ::serialize(this->output_alphabet_size);
		ret += ::serialize(this->state_count);
		ret += ::serialize(this->initial_states);
		ret += ::serialize(this->output_mapping);
		ret += ::serialize(this->transitions);
		ret[0] = htonl(ret.length - 1);
	}

	return ret;
}}}
bool moore_machine::deserialize(serial_stretch & serial)
{{{
	clear();
	int size;
	if(!::deserialize(size, serial)) goto failed;
	if(!::deserialize(is_deterministic, serial)) goto failed;
	if(!::deserialize(input_alphabet_size, serial)) goto failed;
	if(!::deserialize(output_alphabet_size, serial)) goto failed;
	if(!::deserialize(state_count, serial)) goto failed;
	if(!::deserialize(initial_states, serial)) goto failed;
	if(!::deserialize(output_mapping, serial)) goto failed;
	if(!::deserialize(transitions, serial)) goto failed;

	this->valid = true;
	return true;
failed:
	clear();
	return false;
}}}
string moore_machine::write()
{ }
bool moore_machine::read(string input)
{ }
string moore_machine::visualize()
{ }
bool moore_machine::parse_transition(string single)
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

	this->transitions.insert(tr);

	return true;
}}}


simple_moore_machine::simple_moore_machine()
{{{
	this->output_alphabet_size = 2;
}}}
void simple_moore_machine::clear()
{{{
	moore_machine::clear();

	this->output_alphabet_size = 2;
}}}
bool simple_moore_machine::calc_validity()
{{{
	if(!moore_machine::calc_validity())
		return false;

	this->valid = (this->output_alphabet_size == 2);

	return this->valid;
}}}
basic_string<int32_t> simple_moore_machine::serialize()
{{{
	basic_string<int32_t> ret;

	if(this->valid) {
		ret += 0; // length, filled in later.
		ret += ::serialize(this->is_deterministic);
		ret += ::serialize(this->input_alphabet_size);
		ret += ::serialize(this->state_count);
		ret += ::serialize(this->initial_states);
		set<int> final = get_final_states();
		ret += ::serialize(this->final_states);
		ret += ::serialize(this->transitions);
		ret[0] = htonl(ret.length() - 1);
	}

	return ret;
}}}
bool simple_moore_machine::deserialize(serial_stretch & serial)
{{{
	clear();
	int size;
	if(!::deserialize(size, serial)) goto failed;
	if(!::deserialize(this->is_deterministic, serial)) goto failed;
	if(!::deserialize(this->input_alphabet_size, serial)) goto failed;
	this->output_alphabet_size = 2;
	if(!::deserialize(this->state_count, serial)) goto failed;
	output_mapping.resize(this->state_count, 0);
	if(!::deserialize(this->initial_states, serial)) goto failed;
	set<int> final;
	if(!::deserialize(final, serial)) goto failed;
	for(set<int>::iterator si = final.begin(); si != final.end(); ++si)
		output_mapping[*si] = 1;
	if(!::deserialize(this->transitions, serial)) goto failed;

	this->valid = true;
	return true;
failed:
	clear;
	return false;
}}}
string simple_moore_machine::write()
{
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
}
bool simple_moore_machine::read(string input)
{
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
}
string simple_moore_machine::visualize()
{
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
		if((int)final.size() < state_count) {
			ret += "\tnode [shape=circle, style=\"\", color=black];";
			for(int s = 0; s < state_count; s++) {
				if(final.find(s) == final.end()) {
					snprintf(buf, 128, " q%d", s);
					ret += buf;
				}
			}
			ret += ";\n";
		}

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
}
set<int> simple_moore_machine::get_final_states()
{{{
	set<int> s;
	for(int i = 0; i < this->state_count; ++i)
		if(output_mapping[i] == 1)
			s.insert(i);
	return s;
}}}


mealy_machine::mealy_machine()
{ };
void mealy_machine::clear()
{{{
	finite_state_machine::clear();

	transitions.clear();
}}}
bool mealy_machine::calc_validity()
{ }
bool mealy_machine::calc_determinism()
{ }
basic_string<int32_t> mealy_machine::serialize()
{ }
bool mealy_machine::deserialize(serial_stretch & serial)
{ }
string mealy_machine::write()
{ }
bool mealy_machine::read(string input)
{ }
string mealy_machine::visualize()
{ }


bounded_simple_mVCA::bounded_simple_mVCA()
{{{
	m_bound = -1;
}}}
void bounded_simple_mVCA::clear()
{{{
	simple_moore_machine::clear();

	m_bound = -1;
}}}
bool bounded_simple_mVCA::calc_validity()
{{{
	if(!simple_moore_machine::calc_validity())
		return false;
	this->valid = (m_bound >= 0);
	return this->valid;
}}}
bool bounded_simple_mVCA::calc_determinism()
{ }
basic_string<int32_t> bounded_simple_mVCA::serialize()
{ }
bool bounded_simple_mVCA::deserialize(serial_stretch & serial)
{ }
string bounded_simple_mVCA::write()
{ }
bool bounded_simple_mVCA::read(string input)
{ }
string bounded_simple_mVCA::visualize()
{ }


simple_mVCA::simple_mVCA()
{{{
	this->output_alphabet_size = 2;
	m_bound = -1;
}}}
void simple_mVCA::clear()
{{{
	finite_state_machine::clear();

	alphabet_directions.clear();
	m_bound = -1;
	final_states.clear();
	transitions.clear();
	this->output_alphabet_size = 2;
}}}
bool simple_mVCA::calc_validity()
{ }
bool simple_mVCA::calc_determinism()
{ }
basic_string<int32_t> simple_mVCA::serialize()
// FIXME: libmVCA expects another alphabet definition
{{{
	basic_string<int32_t> ret;

	if(!valid) return ret;

	ret += 0; // size, filled in later.

	ret += htonl(is_deterministic ? 1 : 2);
	ret += ::serialize(state_count);
	// alphabet
	ret += ::serialize(alphabet_size);
	ret += ::serialize(alphabet_directions);
	// end of alphabet
	ret += ::serialize(initial_state);
	ret += ::serialize(final_states);
	ret += ::serialize(m_bound);
	// transition function
	if(is_deterministic) {
		map<int, map<int, map<int, int> > > deterministic_transitions;
		map<int, map<int, map<int, set<int> > > >::iterator mmmsi;
		map<int, map<int, set<int> > >::iterator mmsi;
		map<int, set<int> >::iterator msi;
		set<int>::iterator si;
		for(mmmsi = transitions.begin(); mmmsi != transitions.end(); ++mmmsi)
			for(mmsi = mmmsi->second.begin(); mmsi != mmmsi->second.end(); ++mmsi)
				for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
					bool found_one = false;
					for(si = msi->second.begin(); si != msi->second.end(); ++si) {
						if(found_one) {
							is_deterministic = false;
							return serialize();
						} else {
							found_one = true;
							deterministic_transitions[mmmsi->first][mmsi->first][msi->first] = *si;
						}
					}
				}
		ret += ::serialize(deterministic_transitions);
	} else {
		ret += ::serialize(transitions);
	}

	ret[0] = htonl(ret.length() - 1);

	return ret;
}}}
bool simple_mVCA::deserialize(serial_stretch & serial)
// FIXME: libmVCA gives another alphabet definition
{{{
	int size;
	int type;
	if(!::deserialize(size, serial)) goto fail;
	if(size < 1) goto fail;
	if(!::deserialize(type, serial)) goto fail;
	switch(type) {
		case 1:
			is_deterministic = true;
			break;
		case 2:
			is_deterministic = false;
			break;
		default:
			goto fail;
	}
	if(!::deserialize(state_count, serial)) goto fail;
	if(!::deserialize(alphabet_size, serial)) goto fail;
	if(!::deserialize(alphabet_directions, serial)) goto fail;
	if(!::deserialize(initial_state, serial)) goto fail;
	if(!::deserialize(final_states, serial)) goto fail;
	if(!::deserialize(m_bound, serial)) goto fail;

	if(is_deterministic) {
		transitions.clear();

		map<int, map<int, map<int, int> > > deterministic_transitions;
		if(!::deserialize(deterministic_transitions, serial)) goto fail;

		map<int, map<int, map<int, int > > >::iterator mmmi;
		map<int, map<int, int > >::iterator mmi;
		map<int, int >::iterator mi;

		for(mmmi = deterministic_transitions.begin(); mmmi != deterministic_transitions.end(); ++mmmi)
			for(mmi = mmmi->second.begin(); mmi != mmmi->second.end(); ++mmi)
				for(mi = mmi->second.begin(); mi != mmi->second.end(); ++mi)
					transitions[mmmi->first][mmi->first][mi->first].insert(mi->second);
	} else {
		if(!::deserialize(transitions, serial)) goto fail;
	}

	return true;
fail:
	clear();
	return false;
}}}
string simple_mVCA::write()
{ }
bool simple_mVCA::read(string input)
{ }
string simple_mVCA::visualize()
{ }



} // enf of namespace libalf.

