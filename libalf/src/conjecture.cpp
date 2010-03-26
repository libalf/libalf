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
 * (c) 2008,2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *                and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
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
	state_count = 0;
	omega = false;

}}}
void finite_state_machine::clear()
{{{
	conjecture::clear();
	is_deterministic = false;
	input_alphabet_size = 0;
	state_count = 0;
	omega = false;

	initial_states.clear();
}}}
bool finite_state_machine::calc_validity()
{{{
	if(!conjecture::calc_validity())
		return false;

	set<int>::iterator si;

	if(input_alphabet_size<=0 || state_count<=0 || initial_states.empty()) goto invalid;

	for(si = initial_states.begin(); si != initial_states.end(); ++si)
		if(*si < 0 || *si >= state_count)
			goto invalid;

	return true;

invalid:
	clear();
	return false;
}}}




simple_output_finite_state_machine::simple_output_finite_state_machine()
{ };
void simple_output_finite_state_machine::clear()
{{{
	finite_state_machine::clear();
	final_states.clear();
}}}
bool simple_output_finite_state_machine::calc_validity()
{{{
	if(!finite_state_machine::calc_validity())
		return false;

	set<int>::iterator si;

	for(si = final_states.begin(); si != final_states.end(); ++si)
		if(*si < 0 || *si >= this->state_count) {
			clear();
			return false;
		}

	return true;
}}}




enhanced_output_finite_state_machine::enhanced_output_finite_state_machine()
{{{
	output_alphabet_size = 0;
	final_output = true;
}}}
void enhanced_output_finite_state_machine::clear()
{{{
	finite_state_machine::clear();
	output_alphabet_size = 0;
	final_output = true;
}}}
bool enhanced_output_finite_state_machine::calc_validity()
{{{
	if(!finite_state_machine::calc_validity())
		return false;

	if(output_alphabet_size <= 0) {
		clear();
		return false;
	} else {
		return true;
	}
}}}




simple_moore_machine::simple_moore_machine()
{{{
	this->omega = false;
}}}
void simple_moore_machine::clear()
{{{
	simple_output_finite_state_machine::clear();

	final_states.clear();
	transitions.clear();

	this->omega = false;
}}}
bool simple_moore_machine::calc_validity()
{{{
	set<int>::iterator si;
	multimap<pair<int, int>, int>::iterator tri;

	if(!simple_output_finite_state_machine::calc_validity())
		return false;

	if(this->omega)
		goto invalid;

	for(si = final_states.begin(); si != final_states.end(); ++si)
		if(*si < 0 || *si >= this->state_count)
			goto invalid;

	for(tri = transitions.begin(); tri != transitions.end(); ++tri) {
		if(   tri->first.first < 0 || tri->first.first >= this->state_count
		   || tri->first.second < -1 || tri->first.second >= this->input_alphabet_size // allow epsilon-transitions (label = -1)
		   || tri->second < 0 || tri->second >= this->state_count)
			goto invalid;
	}

	return true;
invalid:
	clear();
	return false;
}}}
bool simple_moore_machine::calc_determinism()
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
basic_string<int32_t> simple_moore_machine::serialize()
{{{
	basic_string<int32_t> ret;

	if(this->valid) {
		ret += 0; // length, filled in later.
		ret += ::serialize(this->is_deterministic);
		ret += ::serialize(this->input_alphabet_size);
		ret += ::serialize(this->state_count);
		ret += ::serialize(this->initial_states);
		ret += ::serialize(this->final_states);
		ret += ::serialize(this->transitions);
		ret[0] = htonl(ret.length() - 1);
	}

	return ret;
}}}
bool simple_moore_machine::deserialize(serial_stretch & serial)
{{{
	int size;

	clear();

	if(!::deserialize(size, serial)) goto failed;
	if(!::deserialize(this->is_deterministic, serial)) goto failed;
	if(!::deserialize(this->input_alphabet_size, serial)) goto failed;
	if(!::deserialize(this->state_count, serial)) goto failed;
	if(!::deserialize(this->initial_states, serial)) goto failed;
	if(!::deserialize(this->final_states, serial)) goto failed;
	if(!::deserialize(this->transitions, serial)) goto failed;

	this->omega = false;
	this->valid = true;
	return true;
failed:
	clear();
	return false;
}}}
string simple_moore_machine::write()
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
				this->is_deterministic ? "true" : "false", this->input_alphabet_size, this->state_count);
		ret += buf;

		first_komma = true;
		for(si = this->initial_states.begin(); si != this->initial_states.end(); si++) {
			snprintf(buf, 256, "%s%d", first_komma ? "\t" : ", ", *si);
			first_komma = false;
			ret += buf;
		}
		if(!first_komma)
			ret += ";";

		ret += "\n[final states]\n";

		first_komma = true;
		for(si = this->final_states.begin(); si != this->final_states.end(); ++si) {
			snprintf(buf, 256, "%s%d", first_komma ? "\t" : ", ", *si);
			first_komma = false;
			ret += buf;
		}
		if(!first_komma)
			ret += ";";

		ret += "\n[transitions]\n";

		for(tri = this->transitions.begin(); tri != this->transitions.end(); ++tri) {
			snprintf(buf, 256, "\t%d, %d, %d;\n", tri->first.first, tri->first.second, tri->second);
			ret += buf;
		}
	}

	return ret;
}}}
bool simple_moore_machine::read(string input)
{{{
	bool set_is_det = false, set_alphabet_size = false, set_state_count = false;
	enum section {
		section_none,
		section_general,
		section_initial,
		section_final,
		section_transitions
	} current_section = section_none;

	clear();

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
						this->valid = false;
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
									this->valid = false;
									goto end;
								}
				} else {
					switch (current_section) {
						case section_none:
							this->valid = false;
							goto end;
						case section_general:
							{
								string lval, rval;
								char *endptr;
								size_t p = single.find_first_of('=');
								if(p == string::npos) {
									this->valid = false;
									goto end;
								}
								lval = single.substr(0,p);
								rval = single.substr(p+1, string::npos);
								if(lval == "is dfa") {
									if(rval == "true" || rval == "yes") {
										this->is_deterministic = true;
										set_is_det = true;
									} else {
										if(rval == "false" || rval == "no") {
											this->is_deterministic = false;
											set_is_det = true;
										} else {
											this->valid = false;
											goto end;
										}
									}
								} else {
									if(lval == "alphabet size") {
										set_alphabet_size = true;
										this->input_alphabet_size = strtol(rval.c_str(), &endptr, 10);
										if(*endptr) {
											this->valid = false;
											goto end;
										}
									} else {
										if(lval == "number of states") {
											set_state_count = true;
											this->state_count = strtol(rval.c_str(), &endptr, 10);
											if(*endptr) {
												this->valid = false;
												goto end;
											}
										} else {
											this->valid = false;
											goto end;
										}
									}
								}
							}
							break;
						case section_initial:
							if(!parser_get_set(single, this->initial_states)) {
								this->valid = false;
								goto end;
							}
							break;
						case section_final:
							{
								set<int> final;
								set<int>::iterator si;

								if(!parser_get_set(single, final)) {
									this->valid = false;
									goto end;
								}
								for(si = final.begin(); si != final.end(); ++si)
									final_states.insert(*si);
							}
							break;
						case section_transitions:
							if(!parse_transition(single)) {
								this->valid = false;
								goto end;
							}
							break;
					}
				}
			}
		}
	}

	this->valid = set_is_det && set_alphabet_size && set_state_count;

end:
	if(!valid)
		clear();

	return valid;
}}}
string simple_moore_machine::visualize()
{{{
	string ret;

	if(valid) {
		char buf[64];

		set<int>::iterator sti;
		bool header_written;

		// head
		ret = "digraph simple_moore_machine {\n"
			"\tgraph[fontsize=8]\n"
			"\trankdir=LR;\n"
			"\tsize=8;\n\n";

		// mark final states
		header_written = false;

		for(sti = this->final_states.begin(); sti != this->final_states.end(); ++sti) {
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
		if((int)final_states.size() < state_count) {
			ret += "\tnode [shape=circle, style=\"\", color=black];";
			for(int s = 0; s < state_count; s++) {
				if(final_states.find(s) == final_states.end()) {
					snprintf(buf, 128, " q%d", s);
					ret += buf;
				}
			}
			ret += ";\n";
		}

		// add non-visible states for arrows to initial states
		header_written = false;
		for(sti = this->initial_states.begin(); sti != this->initial_states.end(); ++sti) {
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
		for(sti = this->initial_states.begin(); sti != this->initial_states.end(); ++sti) {
			snprintf(buf, 128, "\tiq%d -> q%d [ color = blue ];\n", *sti, *sti);
			ret += buf;
		}

		// transitions
		multimap<pair<int, int>, int>::iterator ti, tj;
		int s=-1,l=-2,d=-1;
		for(ti = this->transitions.begin(); ti != this->transitions.end(); ti++) {
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
bool simple_moore_machine::parse_transition(string single)
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

	if(m_bound < 0) {
		clear();
		return false;
	} else {
		return true;
	}
}}}
basic_string<int32_t> bounded_simple_mVCA::serialize()
{{{
	basic_string<int32_t> ret;

	if(valid) {
		ret = simple_moore_machine::serialize();
		ret[0] = htonl( 1 + ntohl(ret[0]) ); // fix size:
		ret += ::serialize(m_bound); // appending
	}

	return ret;
}}}
bool bounded_simple_mVCA::deserialize(serial_stretch & serial)
{{{
	if(!simple_moore_machine::deserialize(serial))
		return false;
	if(!::deserialize(m_bound, serial)) {
		clear();
		return false;
	} else {
		return true;
	}
}}}
string bounded_simple_mVCA::write()
{
	// FIXME
	return "";
}
bool bounded_simple_mVCA::read(string input)
{
	// FIXME
	return false;
}
string bounded_simple_mVCA::visualize()
{
	// FIXME: add a label showing the m_bound
	return simple_moore_machine::visualize();
}




simple_mVCA::simple_mVCA()
{{{
	m_bound = -1;
}}}
void simple_mVCA::clear()
{{{
	simple_output_finite_state_machine::clear();

	alphabet_directions.clear();
	m_bound = -1;
	final_states.clear();
	transitions.clear();
}}}
bool simple_mVCA::calc_validity()
{{{
	set<int>::iterator si;
	vector<int>::iterator vi;
	map<int, map<int, map<int, set<int> > > >::iterator mmmsi;
	map<int, map<int, set<int> > >::iterator mmsi;
	map<int, set<int> >::iterator msi;

	if(!simple_output_finite_state_machine::calc_validity())
		return false;

	if(omega) goto invalid;

	if(m_bound < 0) goto invalid;

	if((int)alphabet_directions.size() != this->input_alphabet_size) goto invalid;
	for(vi = alphabet_directions.begin(); vi != alphabet_directions.end(); ++vi)
		if((*vi < -1 || *vi > 1) && (*vi != -100)) goto invalid;

	for(si = final_states.begin(); si != final_states.end(); ++si)
		if(*si < 0 || *si >= this->state_count) goto invalid;

	for(mmmsi = transitions.begin(); mmmsi != transitions.end(); ++mmmsi) {
		if(mmmsi->first < 0 || mmmsi->first > m_bound) goto invalid;
		for(mmsi = mmmsi->second.begin(); mmsi != mmmsi->second.end(); ++mmsi) {
			if(mmsi->first < 0 || mmsi->first >= this->state_count) goto invalid;
			for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
				if(msi->first < -1 || msi->first >= this->input_alphabet_size) goto invalid;
				for(si = msi->second.begin(); si != msi->second.end(); ++si)
					if(*si < 0 || *si >= this->state_count) goto invalid;
			}
		}
	}

	return true;
invalid:
	clear();
	return false;
}}}
bool simple_mVCA::calc_determinism()
{{{
	map<int, map<int, map<int, set<int> > > >::iterator mmmsi;
	map<int, map<int, set<int> > >::iterator mmsi;
	map<int, set<int> >::iterator msi;
	set<int>::iterator si;

	for(mmmsi = transitions.begin(); mmmsi != transitions.end(); ++mmmsi)
		for(mmsi = mmmsi->second.begin(); mmsi != mmmsi->second.end(); ++mmsi)
			for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi)
				if(msi->second.size() > 1) {
					this->is_deterministic = false;
					return false;
				}
	this->is_deterministic = true;
	return true;
}}}
basic_string<int32_t> simple_mVCA::serialize()
{{{
	basic_string<int32_t> ret;

	if(!valid) return ret;

	ret += 0; // size, filled in later.

	ret += htonl(is_deterministic ? 1 : 2);
	ret += ::serialize(this->state_count);
	// alphabet
	ret += ::serialize(this->input_alphabet_size);
	ret += ::serialize(alphabet_directions);
	// end of alphabet
	ret += ::serialize(this->initial_states);
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
	if(!::deserialize(this->state_count, serial)) goto fail;
	if(!::deserialize(this->input_alphabet_size, serial)) goto fail;
	if(!::deserialize(alphabet_directions, serial)) goto fail;
	if(!::deserialize(this->initial_states, serial)) goto fail;
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
{
	// FIXME
	return "";
}
bool simple_mVCA::read(string input)
{
	// FIXME
	return false;
}
string simple_mVCA::visualize()
{
	// FIXME: copy code from libmVCA
	return "";
}




moore_machine::moore_machine()
{ };
void moore_machine::clear()
{{{
	enhanced_output_finite_state_machine::clear();

	output_mapping.clear();
	transitions.clear();
}}}
bool moore_machine::calc_validity()
{{{
	if(!enhanced_output_finite_state_machine::calc_validity())
		return false;

	map<int, int>::iterator omi;
	multimap<pair<int, int>, int>::iterator tri;

	for(omi = output_mapping.begin(); omi != output_mapping.end(); ++omi) {
		if(omi->first < 0 || omi->first >= this->state_count || omi->second < 0 || omi->second >= this->output_alphabet_size)
			goto invalid;
	}

	for(tri = transitions.begin(); tri != transitions.end(); ++tri) {
		if(   tri->first.first < 0 || tri->first.first >= this->state_count
		   || tri->first.second < -1 || tri->first.second >= this->input_alphabet_size // allow epsilon-transitions (label = -1)
		   || tri->second < 0 || tri->second >= this->state_count)
			goto invalid;
	}

	return true;
invalid:
	clear();
	return false;
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
		ret += ::serialize(this->final_output);
		ret += ::serialize(this->omega);
		ret += ::serialize(output_mapping);
		ret += ::serialize(transitions);
		ret[0] = htonl(ret.length() - 1);
	}

	return ret;
}}}
bool moore_machine::deserialize(serial_stretch & serial)
{{{
	clear();
	int size;
	if(!::deserialize(size, serial)) goto failed;
	if(!::deserialize(this->is_deterministic, serial)) goto failed;
	if(!::deserialize(this->input_alphabet_size, serial)) goto failed;
	if(!::deserialize(this->output_alphabet_size, serial)) goto failed;
	if(!::deserialize(this->state_count, serial)) goto failed;
	if(!::deserialize(this->initial_states, serial)) goto failed;
	if(!::deserialize(this->final_output, serial)) goto failed;
	if(!::deserialize(this->omega, serial)) goto failed;
	if(!::deserialize(output_mapping, serial)) goto failed;
	if(!::deserialize(transitions, serial)) goto failed;

	this->valid = true;
	return true;
failed:
	clear();
	return false;
}}}
string moore_machine::write()
{
	// FIXME
	return "";
}
bool moore_machine::read(string input)
{
	// FIXME
	return false;
}
string moore_machine::visualize()
{{{
	string ret;

	if(valid) {
		char buf[64];

		set<int>::iterator sti;
		bool header_written;

		// head
		ret = "digraph moore_machine {\n"
			"\tgraph[fontsize=8]\n"
			"\trankdir=LR;\n"
			"\tsize=8;\n\n";

		// states
		for(int s = 0; s < state_count; s++) {
			ret += "\tnode [shape=circle, style=\"\", color=black, label=";
			snprintf(buf, 128, "\"q%d (%d)\"]; q%d;\n", s, output_mapping[s], s);
			ret += buf;
		}

		// add non-visible states for arrows to initial states
		header_written = false;
		for(sti = this->initial_states.begin(); sti != this->initial_states.end(); ++sti) {
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
		for(sti = this->initial_states.begin(); sti != this->initial_states.end(); ++sti) {
			snprintf(buf, 128, "\tiq%d -> q%d [ color = blue ];\n", *sti, *sti);
			ret += buf;
		}

		// transitions
		multimap<pair<int, int>, int>::iterator ti, tj;
		int s=-1,l=-2,d=-1;
		for(ti = this->transitions.begin(); ti != this->transitions.end(); ti++) {
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




mealy_machine::mealy_machine()
{ };
void mealy_machine::clear()
{{{
	enhanced_output_finite_state_machine::clear();

	transitions.clear();
}}}
bool mealy_machine::calc_validity()
{{{
	if(!enhanced_output_finite_state_machine::calc_validity())
		return false;

	map<int, map<int, set<pair<int, int> > > >::iterator mmspi;
	map<int, set<pair<int, int> > >::iterator mspi;
	set<pair<int, int> >::iterator spi;

	for(mmspi = transitions.begin(); mmspi != transitions.end(); ++mmspi) {
		if(mmspi->first < 0 || mmspi->first >= this->state_count) goto invalid;
		for(mspi = mmspi->second.begin(); mspi != mmspi->second.end(); ++mspi) {
			if(mspi->first < -1 || mspi->first >= this->input_alphabet_size) goto invalid;
			for(spi = mspi->second.begin(); spi != mspi->second.end(); ++spi) {
				if(spi->first < 0 || spi->first >= this->state_count) goto invalid;
				if(spi->second < 0 || spi->second >= this->output_alphabet_size) goto invalid;
			}
		}
	}

	return true;
invalid:
	clear();
	return false;
}}}
bool mealy_machine::calc_determinism()
{{{
	map<int, map<int, set<pair<int, int> > > >::iterator mmspi;
	map<int, set<pair<int, int> > >::iterator mspi;
	set<pair<int, int> >::iterator spi;

	for(mmspi = transitions.begin(); mmspi != transitions.end(); ++mmspi)
		for(mspi = mmspi->second.begin(); mspi != mmspi->second.end(); ++mspi)
			if(mspi->second.size() > 1) {
				is_deterministic = false;
				return false;
			}

	is_deterministic = true;
	return true;


}}}
basic_string<int32_t> mealy_machine::serialize()
{{{
	basic_string<int32_t> ret;

	if(this->valid) {
		ret += 0; // length, filled in later.
		ret += ::serialize(this->is_deterministic);
		ret += ::serialize(this->input_alphabet_size);
		ret += ::serialize(this->output_alphabet_size);
		ret += ::serialize(this->state_count);
		ret += ::serialize(this->initial_states);
		ret += ::serialize(this->transitions);
		ret += ::serialize(this->final_output);
		ret += ::serialize(this->omega);
		ret += ::serialize(transitions);
		ret[0] = htonl(ret.length() - 1);
	}

	return ret;
}}}
bool mealy_machine::deserialize(serial_stretch & serial)
{{{
	int size;
	if(!::deserialize(size, serial)) goto failed;
	if(size < 1) goto failed;
	if(!::deserialize(this->is_deterministic, serial)) goto failed;
	if(!::deserialize(this->input_alphabet_size, serial)) goto failed;
	if(!::deserialize(this->output_alphabet_size, serial)) goto failed;
	if(!::deserialize(this->state_count, serial)) goto failed;
	if(!::deserialize(this->initial_states, serial)) goto failed;
	if(!::deserialize(this->transitions, serial)) goto failed;
	if(!::deserialize(this->final_output, serial)) goto failed;
	if(!::deserialize(this->omega, serial)) goto failed;
	if(!::deserialize(transitions, serial)) goto failed;

	this->valid = true;
	return true;
failed:
	clear();
	return false;
}}}
string mealy_machine::write()
{
	// FIXME
	return "";
}
bool mealy_machine::read(string input)
{
	// FIXME
	return false;
}
string mealy_machine::visualize()
{{{
	string ret;

	if(valid) {
		char buf[64];

		set<int>::iterator sti;
		bool header_written;

		// head
		ret = "digraph mealy_machine {\n"
			"\tgraph[fontsize=8]\n"
			"\trankdir=LR;\n"
			"\tsize=8;\n\n";

		// default
		ret += "\tnode [shape=circle, style=\"\", color=black];";
		for(int s = 0; s < state_count; s++) {
			snprintf(buf, 128, " q%d", s);
			ret += buf;
		}
		ret += ";\n";

		// add non-visible states for arrows to initial states
		header_written = false;
		for(sti = this->initial_states.begin(); sti != this->initial_states.end(); ++sti) {
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
		for(sti = this->initial_states.begin(); sti != this->initial_states.end(); ++sti) {
			snprintf(buf, 128, "\tiq%d -> q%d [ color = blue ];\n", *sti, *sti);
			ret += buf;
		}

		// transitions
		map<int, map<int, set<pair<int, int> > > >::iterator mmspi;
		map<int, set<pair<int, int> > >::iterator mspi;
		set<pair<int, int> >::iterator spi;

		for(mmspi = transitions.begin(); mmspi != transitions.end(); ++mmspi) {
			int src = mmspi->first;
			for(mspi = mmspi->second.begin(); mspi != mmspi->second.end(); ++mspi) {
				int sigma = mspi->first;
				set<pair<int, int> >::iterator spi;
				for(spi = mspi->second.begin(); spi != mspi->second.end(); ++spi) {
					int dst = spi->first;
					int osigma = spi->second;
					snprintf(buf, 64, "\tq%d -> q%d [ label = \"%d/%d\" ];\n",
							src, dst, sigma, osigma);
					ret += buf;
				}
			}
		}
		// end
		ret += "};\n";
	}

	return ret;
}}}




} // enf of namespace libalf.

