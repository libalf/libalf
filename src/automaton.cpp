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
		snprintf(buf, 128, " S%d", *sti);
		ret += buf;
	}
	if(header_written)
		ret += "\n";

	// default
	ret += "\tnode [shape=circle, style=\"\", color=black];\n";

	// transitions
	multimap<pair<int, int>, int>::iterator ti, tj;
	for(ti = transitions.begin(); ti != transitions.end(); ti++) {
		// skip double entries
		if(transitions.count(ti->first) > 2) {
			tj = ti;
			tj++;
			if(ti->second == tj->second)
				continue;
		}

		snprintf(buf, 64, "\tS%d -> S%d [ label = \"%d\" ];\n", ti->first.first, ti->first.second, ti->second);
		buf[63] = 0;
		ret += buf;
	}

	// add non-visible states for arrows to initial states
	header_written = false;
	for(sti = initial.begin(); sti != initial.end(); ++sti) {
		if(!header_written) {
			ret += "\n\tnode [shape=plaintext, label=\"\", style=\"\"];";
			header_written = true;
		}
		snprintf(buf, 128, " iS%d", *sti);
		ret += buf;
	}
	if(header_written)
		ret += "\n";

	// and arrows to mark initial states
	for(sti = initial.begin(); sti != initial.end(); ++sti) {
		snprintf(buf, 128, "\tiS%d -> S%d [ color = blue ];\n", *sti, *sti);
		ret += buf;
	}

	// end
	ret += "};\n";

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

}; // end of namespace libalf

