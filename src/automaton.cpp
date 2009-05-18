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

bool is_deterministic(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
{{{
	// check transitions for epsilon and double transitions
	multimap<pair<int, int>, int>::iterator tri;

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
	for(ti = transitions.begin(); ti != transitions.end(); ti++) {
		// skip double entries
		if(transitions.count(ti->first) > 2) {
			tj = ti;
			tj++;
			if(ti->second == tj->second)
				continue;
		}

		snprintf(buf, 64, "\tq%d -> q%d [ label = \"%d\" ];\n", ti->first.first, ti->first.second, ti->second);
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
			   "\tis dfa = %s\n"
			   "\talphabet size = %d\n"
			   "\tnumber of states = %d\n"
			   "[initial states]\n",
			is_dfa ? "true" : "false", alphabet_size, state_count);
	ret += buf;

	first_komma = true;
	for(si = initial.begin(); si != initial.end(); si++) {
		snprintf(buf, 256, "%s%d", first_komma ? "\t" : ", ", *si);
		first_komma = false;
		ret += buf;
	}

	ret += "\n[final states]\n";

	first_komma = true;
	for(si = final.begin(); si != final.end(); si++) {
		snprintf(buf, 256, "%s%d", first_komma ? "\t" : ", ", *si);
		first_komma = false;
		ret += buf;
	}

	ret += "\n[transitions]\n";

	for(tri = transitions.begin(); tri != transitions.end(); tri++) {
		snprintf(buf, 256, "\t%d, %d, %d\n", tri->first.first, tri->first.second, tri->second);
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

}; // end of namespace libalf

