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

#include <string>
#include <stdio.h>
#include <set>

#include <arpa/inet.h>

#include "amore++/finite_automaton.h"
#include "amore++/deterministic_finite_automaton.h"
#include "amore++/nondeterministic_finite_automaton.h"

namespace amore {

using namespace std;


finite_automaton * construct_amore_automaton(bool is_dfa, int alphabet_size, int state_count, std::set<int> &initial, std::set<int> &final, multimap<pair<int,int>, int> &transitions)
{{{
	finite_automaton * ret;
	if(is_dfa)
		ret = new deterministic_finite_automaton;
	else
		ret = new nondeterministic_finite_automaton;

	if(ret->construct(alphabet_size, state_count, initial, final, transitions))
		return ret;

	delete ret;
	return NULL;
}}}




string finite_automaton::generate_dotfile()
{{{
	basic_string<int32_t> serialized;
	basic_string<int32_t>::iterator si;
	int n;

	std::set<int32_t> initial, final;
	std::set<int32_t>::iterator sti;

	int state_count;
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
	ret += "\tnode [shape=circle, style=\"\", color=black];";
	for(int s = 0; s < state_count; s++){
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
	ret += "}\n";

	return ret;
}}}

// inefficient (as it only wraps another interface), but it works for all automata implementations
// that implement serialize and deserialize. implementations may provide their own, more performant
// implementation of construct().
bool finite_automaton::construct(int alphabet_size, int state_count, std::set<int> &initial, std::set<int> &final, multimap<pair<int, int>, int> &transitions)
{{{
	basic_string<int32_t> ser;
	std::set<int>::iterator sit;
	multimap<pair<int, int>, int>::iterator tit;

	// serialize that data and call deserialize :)
	ser += 0;

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

