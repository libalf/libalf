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

#include "libalf/automata.h"


namespace libalf {

using namespace std;

string finite_language_automaton::generate_dotfile()
{{{
	basic_string<int32_t> serialized;
	basic_string<int32_t>::iterator si;
	int n;

	set<int32_t> initial, final;
	set<int32_t>::iterator sti;

	bool header_written;
	char buf[128];

	string ret;

	serialized = this->serialize();
	if(serialized.length() == 0)
		return ret; // empty

	si = serialized.begin();

	ret = "digraph finite_automaton {\n"
		"\trankdir=LR;\n"
		"\tsize=8;\n";

	si++; // skip length field

	si++; // skip alphabet size
	si++; // skip state count
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

	// initial states that are not final
	header_written = false;
	for(sti = initial.begin(); sti != initial.end(); sti++) {
		if(final.find(*sti) == final.end()) {
			if(!header_written) {
				ret += "\tnode [shape=circle, style=bold];";
				header_written = true;
			}
			snprintf(buf, 128, " S%d", *sti);
			ret += buf;
		}
	}
	if(header_written)
		ret += "\n";

	// initial states that are final
	header_written = false;
	for(sti = initial.begin(); sti != initial.end(); sti++) {
		if(final.find(*sti) != final.end()) {
			if(!header_written) {
				ret += "\tnode [shape=doublecircle, style=bold];";
				header_written = true;
			}
			snprintf(buf, 128, " S%d", *sti);
			ret += buf;
		}
	}
	if(header_written)
		ret += "\n";

	// final states that are not initial
	header_written = false;
	for(sti = final.begin(); sti != final.end(); sti++) {
		if(initial.find(*sti) == initial.end()) {
			if(!header_written) {
				ret += "\tnode [shape=doublecircle, style=\"\"];";
				header_written = true;
			}
			snprintf(buf, 128, " S%d", *sti);
			ret += buf;
		}
	}
	if(header_written)
		ret += "\n";


	// default
	ret += "\tnode [shape=circle, style=\"\"];\n";

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
			snprintf(buf, 128, "\tS%d -> S%d [ label = \"%d\" ];\n", src, dst, label);
		else
			snprintf(buf, 128, "\tS%d -> S%d;\n", src, dst);
		ret += buf;
	}

	// end
	ret += "}\n";

	return ret;
}}}

// inefficient (as it only wraps another interface), but it works for all automata implementations
// that implement serialize and deserialize. implementations may provide their own, more performant
// implementation of construct().
bool finite_language_automaton::construct(int alphabet_size, int state_count, list<int> &start, list<int> &final, list<transition> &transitions)
{{{
	basic_string<int32_t> ser;
	list<int>::iterator sit;
	list<transition>::iterator tit;

	// serialize that data and call deserialize :)
	ser += 0;

	ser += htonl(alphabet_size);

	ser += htonl(state_count);

	ser += htonl(start.size());
	for(sit = start.begin(); sit != start.end(); sit++)
		ser += htonl(*sit);

	ser += htonl(final.size());
	for(sit = final.begin(); sit != final.end(); sit++)
		ser += htonl(*sit);

	ser += htonl(transitions.size());
	for(tit = transitions.begin(); tit != transitions.end(); tit++) {
		ser += htonl(tit->source);
		ser += htonl(tit->label);
		ser += htonl(tit->destination);
	}

	ser[0] = htonl(ser.length() - 1);

	basic_string<int32_t>::iterator ser_begin = ser.begin();

	return this->deserialize(ser_begin, ser.end());
}}}

} // end namespace libalf

