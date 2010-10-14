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




bool finite_automaton::calc_validity()
{{{
	if(!moore_machine<bool>::calc_validity())
		goto invalid;

	if(this->omega)
		goto invalid;

	return true;

invalid:
	this->valid = false;
	return false;
}}}
basic_string<int32_t> finite_automaton::serialize() const
{{{
	// CUSTOM FORMAT, to match libAMoRE++ serialization format.

	basic_string<int32_t> ret;

	if(this->valid) {
		ret += 0; // size, filled in later.
		ret += ::serialize(this->is_deterministic);
		ret += ::serialize(this->input_alphabet_size);
		ret += ::serialize(this->state_count);
		ret += ::serialize(this->initial_states);

		// get final states
		set<int> final;
		map<int, bool>::const_iterator mi;
		for(mi = this->output_mapping.begin(); mi != this->output_mapping.end(); ++mi)
			if(mi->second)
				final.insert(mi->first);
		ret += ::serialize(final);

		// get transitions
		int transition_count = 0;
		basic_string<int32_t> transition_ser;
		map<int, map<int, set<int> > >::const_iterator mmsi;
		map<int, set<int> >::const_iterator msi;
		set<int>::const_iterator si;

		for(mmsi = this->transitions.begin(); mmsi != this->transitions.end(); ++mmsi) {
			for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
				for(si = msi->second.begin(); si != msi->second.end(); ++si) {
					++transition_count;
					transition_ser += ::serialize(mmsi->first); // src
					transition_ser += ::serialize(msi->first); // label
					transition_ser += ::serialize(*si); // dst
				}
			}
		}
		ret += ::serialize(transition_count);
		ret += transition_ser;

		ret[0] = htonl(ret.length() - 1);
	}

	return ret;
}}}
bool finite_automaton::deserialize(serial_stretch & serial)
{{{
	int size;
	int transition_count;
	int i;
	int src, label, dst;

	set<int> final;
	set<int>::const_iterator si;

	clear();
	if(!::deserialize(size, serial)) goto invalid;
	if(!::deserialize(this->is_deterministic, serial)) goto invalid;
	if(!::deserialize(this->input_alphabet_size, serial)) goto invalid;
	if(!::deserialize(this->state_count, serial)) goto invalid;
	if(!::deserialize(this->initial_states, serial)) goto invalid;
	// final states
	if(!::deserialize(final, serial)) goto invalid;
	for(i = 0; i < this->state_count; ++i)
		output_mapping[i] = false;
	for(si = final.begin(); si != final.end(); ++si)
		output_mapping[*si] = true;
	// transitions
	if(!::deserialize(transition_count, serial)) goto invalid;
	for(i = 0; i < transition_count; ++i) {
		if(!::deserialize(src, serial)) goto invalid;
		if(!::deserialize(label, serial)) goto invalid;
		if(!::deserialize(dst, serial)) goto invalid;
		this->transitions[src][label].insert(dst);
	}

	this->valid = true;
	return true;
invalid:
	clear();
	return false;
}}}
string finite_automaton::write() const
{{{
	string ret;

	if(valid) {
		char buf[256];
		map<int, map<int, set<int> > >::const_iterator mmsi;
		map<int, set<int> >::const_iterator msi;
		set<int>::const_iterator si;
		map<int, bool>::const_iterator oi;
		bool first_komma;

		snprintf(buf, 256, "[general]\n"
				   "\tis dfa = %s;\n"
				   "\talphabet size = %d;\n"
				   "\tnumber of states = %d;\n"
				   "[initial states]\n",
				this->is_deterministic ? "true" : "false", this->input_alphabet_size, this->state_count);
		ret += buf;

		first_komma = true;
		for(si = this->initial_states.begin(); si != this->initial_states.end(); ++si) {
			snprintf(buf, 256, "%s%d", first_komma ? "\t" : ", ", *si);
			first_komma = false;
			ret += buf;
		}
		if(!first_komma)
			ret += ";";

		ret += "\n[final states]\n";

		first_komma = true;
		for(oi = this->output_mapping.begin(); oi != this->output_mapping.end(); ++oi) {
			if(oi->second) {
				snprintf(buf, 256, "%s%d", first_komma ? "\t" : ", ", oi->first);
				first_komma = false;
				ret += buf;
			}
		}
		if(!first_komma)
			ret += ";";

		ret += "\n[transitions]\n";

		for(mmsi = this->transitions.begin(); mmsi != this->transitions.end(); ++mmsi) {
			for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
				for(si = msi->second.begin(); si != msi->second.end(); ++si) {
					snprintf(buf, 256, "\t%d, %d, %d;\n", mmsi->first, msi->first, *si);
					ret += buf;
				}
			}
		}
	}

	return ret;
}}}
bool finite_automaton::read(string input)
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
											// set all states to be non-final
											for(int i = 0; i < this->state_count; ++i)
												this->output_mapping[i] = false;
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
									this->output_mapping[*si] = true;
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
string finite_automaton::visualize() const
{{{
	string ret;

	if(valid) {
		char buf[64];

		set<int>::iterator sti;
		bool header_written;

		// head
		ret = "digraph finite_automaton {\n"
			"\tgraph[fontsize=8]\n"
			"\trankdir=LR;\n"
			"\tsize=8;\n\n";

		// mark final states
		header_written = false;

		int final_state_count = 0;
		map<int, bool>::const_iterator oi;

		for(oi = output_mapping.begin(); oi != output_mapping.end(); ++oi) {
			if(oi->second) {
				++final_state_count;
				if(!header_written) {
					ret += "\tnode [shape=doublecircle, style=\"\", color=black];";
					header_written = true;
				}
				snprintf(buf, 128, " q%d", oi->first);
				ret += buf;
			}
		}
		if(header_written)
			ret += ";\n";

		// default
		if(final_state_count < state_count) {
			ret += "\tnode [shape=circle, style=\"\", color=black];";
			for(oi = output_mapping.begin(); oi != output_mapping.end(); ++oi) {
				if(!oi->second) {
					snprintf(buf, 128, " q%d", oi->first);
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
		map<int, map<int, set<int> > >::const_iterator mmsi;
		map<int, set<int> >::const_iterator msi;
		set<int>::const_iterator si;
		for(mmsi = this->transitions.begin(); mmsi != this->transitions.end(); ++mmsi) {
			for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
				for(si = msi->second.begin(); si != msi->second.end(); ++si) {
					snprintf(buf, 256, "\tq%d -> q%d [ label = \"%d\" ];\n", mmsi->first, msi->first, *si);
					ret += buf;
				}
			}
		}

		// end
		ret += "};\n";
	}

	return ret;
}}}

bool finite_automaton::contains(const list<int> & word) const
{{{
	if(!this->valid)
		return false;

	set<int> current_states;
	set<int>::const_iterator si;

	current_states = this->initial_states;
	run(current_states, word.begin(), word.end());

	// check if a final state was reached
	map<int, bool>::const_iterator fi;
	for(si = current_states.begin(); si != current_states.end(); ++si) {
		fi = this->output_mapping.find(*si);
		if(fi != this->output_mapping.end()) {
			if(fi->second)
				return true;
		}
	}

	return false;
}}}
void finite_automaton::set_all_non_accepting()
{{{
	this->output_mapping.clear();
	for(int i = 0; i < this->state_count; ++i)
		output_mapping[i] = false;
}}}
bool finite_automaton::parse_transition(string single)
{{{
	string tok;
	size_t p;
	char *endptr;

	int src, label, dst;

	// FROM
	p = single.find_first_of(',');
	tok = single.substr(0,p);
	if(p == string::npos)
		return false;
	single = single.substr(p+1, string::npos);

	src = strtol(tok.c_str(), &endptr, 10);
	if(*endptr)
		return false;

	// LABEL
	p = single.find_first_of(',');
	tok = single.substr(0,p);
	if(p == string::npos)
		return false;
	single = single.substr(p+1, string::npos);

	label = strtol(tok.c_str(), &endptr, 10);
	if(*endptr)
		return false;

	// TO
	tok = single;

	dst = strtol(tok.c_str(), &endptr, 10);
	if(*endptr)
		return false;

	this->transitions[src][label].insert(dst);

	return true;
}}}




basic_string<int32_t> simple_mVCA::serialize() const
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
	set<int> final_states;
	this->get_final_states(final_states);
	ret += ::serialize(final_states);
	ret += ::serialize(m_bound);
	// transition function
	if(is_deterministic) {
		map<int, map<int, map<int, int> > > deterministic_transitions;
		map<int, map<int, map<int, set<int> > > >::const_iterator mmmsi;
		map<int, map<int, set<int> > >::const_iterator mmsi;
		map<int, set<int> >::const_iterator msi;
		set<int>::const_iterator si;
		for(mmmsi = transitions.begin(); mmmsi != transitions.end(); ++mmmsi)
			for(mmsi = mmmsi->second.begin(); mmsi != mmmsi->second.end(); ++mmsi)
				for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
					bool found_one = false;
					for(si = msi->second.begin(); si != msi->second.end(); ++si) {
						if(found_one) {
							ret.clear();
							return ret;
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

	set<int> final_states;
	set<int>::const_iterator si;

	map<int, map<int, map<int, int> > > deterministic_transitions;
	map<int, map<int, map<int, int > > >::iterator mmmi;
	map<int, map<int, int > >::iterator mmi;
	map<int, int >::iterator mi;

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
	for(int i = 0; i < this->state_count; ++i)
		this->output_mapping[i] = false;
	if(!::deserialize(this->input_alphabet_size, serial)) goto fail;
	if(!::deserialize(alphabet_directions, serial)) goto fail;
	if(!::deserialize(this->initial_states, serial)) goto fail;
	if(!::deserialize(final_states, serial)) goto fail;
	for(si = final_states.begin(); si != final_states.end(); ++si)
		this->output_mapping[*si] = true;
	if(!::deserialize(m_bound, serial)) goto fail;

	if(is_deterministic) {
		transitions.clear();

		if(!::deserialize(deterministic_transitions, serial)) goto fail;

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
string simple_mVCA::write() const
{
	// TODO
	return "bounded_simple_mVCA::write(): TODO.";
}
bool simple_mVCA::read(string input)
{
	// TODO
	return false;
}
string simple_mVCA::visualize() const
{
	// TODO
	return "simple_mVCA::visualize(): TODO.\n";
}




void bounded_simple_mVCA::clear()
{{{
	finite_automaton::clear();
	bound = 0;
}}}
bool bounded_simple_mVCA::calc_validity()
{{{
	if(!finite_automaton::calc_validity())
		goto invalid;

	if(bound < 0)
		goto invalid;

	return true;

invalid:
	this->valid = false;
	return false;
}}}
basic_string<int32_t> bounded_simple_mVCA::serialize() const
{{{
	basic_string<int32_t> ret;

	if(valid) {
		ret += 0;
		ret += finite_automaton::serialize();
		ret += ::serialize(bound);
		ret[0] = htonl(ret.length() - 1);
	}

	return ret;
}}}
bool bounded_simple_mVCA::deserialize(serial_stretch & serial)
{{{
	int size;

	clear();
	if(!::deserialize(size, serial)) goto failed;
	if(!finite_automaton::deserialize(serial)) goto failed;
	if(!::deserialize(bound, serial)) goto failed;

	this->valid = true;
	return true;
failed:
	clear();
	return false;
}}}
string bounded_simple_mVCA::write() const
{
	// TODO
	return "bounded_simple_mVCA::write(): TODO.";
}
bool bounded_simple_mVCA::read(string input)
{
	// TODO
	return false;
}
string bounded_simple_mVCA::visualize() const
{
	// TODO: add label with bound.
	return finite_automaton::visualize();
}



} // enf of namespace libalf.

