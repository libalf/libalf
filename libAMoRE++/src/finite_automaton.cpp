/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libAMoRE++
 *
 * libAMoRE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libAMoRE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libAMoRE++.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <string>
#include <stdio.h>
#include <set>

#include <arpa/inet.h>

#include "amore++/finite_automaton.h"
#include "amore++/deterministic_finite_automaton.h"
#include "amore++/nondeterministic_finite_automaton.h"

namespace amore {

using namespace std;


finite_automaton * construct_amore_automaton(bool is_dfa, int alphabet_size, int state_count, set<int> &initial, set<int> &final, multimap<pair<int,int>, int> &transitions)
{{{
	finite_automaton * ret;
	if(is_dfa)
		ret = new deterministic_finite_automaton;
	else
		ret = new nondeterministic_finite_automaton;

	if(ret->construct(is_dfa, alphabet_size, state_count, initial, final, transitions))
		return ret;

	delete ret;
	return NULL;
}}}

finite_automaton * deserialize_amore_automaton(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{{{
	finite_automaton * ret;

	basic_string<int32_t>::iterator si;
	int is_det;

	si = it;
	if(si == limit) return NULL;
	si++;
	if(si == limit) return NULL;
	is_det = ntohl(*si);
	if(is_det != 0 && is_det != 1) return NULL;

	if(is_det == 0)
		ret = new nondeterministic_finite_automaton;

	if(is_det == 1)
		ret = new deterministic_finite_automaton;

	ret->deserialize(it, limit);

	return ret;
}}}





finite_automaton::~finite_automaton()
{ };

set<int> finite_automaton::run(set<int> from, list<int>::iterator word, list<int>::iterator word_limit)
{{{
	while(word != word_limit) {
		from = this->transition(from, *word);
		word++;
	}
	return from;
}}}

bool finite_automaton::contains(list<int> & word)
{{{
	set<int> states, final_states;
	set<int>::iterator si;

	states = this->get_initial_states();
	states = this->run(states, word.begin(), word.end());

	final_states = this->get_final_states();
	for(si = states.begin(); si != states.end(); si++)
		if(final_states.find(*si) != final_states.end())
			return true;
	return false;
}}}

string finite_automaton::generate_dotfile()
{{{
	basic_string<int32_t> serialized;
	basic_string<int32_t>::iterator si;
	int n;

	set<int32_t> initial, final;
	set<int32_t>::iterator sti;

	unsigned int state_count;
	bool header_written;
	char buf[128];

	string ret;

	serialized = this->serialize();
	if(serialized.length() == 0)
		return ret; // empty

	si = serialized.begin();

	ret = "digraph automaton {\n"
		"\tgraph[fontsize=8]\n"
		"\trankdir=LR;\n"
		"\tsize=8;\n\n";

	si++; // skip length field

	si++; // skip deterministic-flag

	si++; // skip alphabet size

	// state count
	state_count = ntohl(*si);
	si++;

	// number of initial states
	n = ntohl(*si);
	si++;
	// initial states
	for(/* -- */; n > 0; n--) {
		initial.insert(ntohl(*si));
		si++;
	}
	// number of final states
	n = ntohl(*si);
	si++;
	// final states
	for(/* -- */; n > 0; n--) {
		final.insert(ntohl(*si));
		si++;
	}

	// skip number of transitions (assumed to be correct)
	si++;

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
	if(final.size() < state_count) {
		ret += "\tnode [shape=circle, style=\"\", color=black];";
		for(unsigned int s = 0; s < state_count; s++){
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
	while(si != serialized.end()) {
		int32_t src,label,dst;
		src = ntohl(*si);
		si++;
		label = ntohl(*si);
		si++;
		dst = ntohl(*si);
		si++;
		if(label != -1)
			snprintf(buf, 128, "\tq%d -> q%d [ label = \"%d\" ];\n", src, dst, label);
		else
			snprintf(buf, 128, "\tq%d -> q%d;\n", src, dst);
		ret += buf;
	}

	// end
	ret += "};\n";

	return ret;
}}}

finite_automaton *finite_automaton::co_determinize()
{
	finite_automaton *r, *rcod, *cod;
	r = this->reverse_language();
	rcod = r->determinize();
	cod = rcod->reverse_language();
	delete r;
	delete rcod;
	return cod;
}

// inefficient (as it only wraps another interface), but it works for all automata implementations
// that implement serialize and deserialize. implementations may provide their own, more performant
// implementation of construct().
bool finite_automaton::construct(bool is_dfa, int alphabet_size, int state_count, set<int> &initial, set<int> &final, multimap<pair<int, int>, int> &transitions)
{{{
	basic_string<int32_t> ser;
	set<int>::iterator sit;
	multimap<pair<int, int>, int>::iterator tit;

	// serialize that data and call deserialize :)
	ser += 0;

	ser += htonl( is_dfa ? 1 : 0 );

	ser += htonl(alphabet_size);

	ser += htonl(state_count);

	ser += htonl(initial.size());
	for(sit = initial.begin(); sit != initial.end(); sit++)
		ser += htonl(*sit);

	ser += htonl(final.size());
	for(sit = final.begin(); sit != final.end(); sit++)
		ser += htonl(*sit);

	ser += htonl(transitions.size());

	for(tit = transitions.begin(); tit != transitions.end(); tit++) {
		ser += htonl(tit->first.first);  // source
		ser += htonl(tit->first.second); // label
		ser += htonl(tit->second);       // desination
	}

	ser[0] = htonl(ser.length() - 1);

	basic_string<int32_t>::iterator ser_begin = ser.begin();

	return this->deserialize(ser_begin, ser.end());
}}}

} // end namespace amore

