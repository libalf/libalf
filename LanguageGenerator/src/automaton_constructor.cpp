/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * automaton_constructor: simple automaton holder passed to classes that
 * generate automata.
 * this is the same class as libalf::automaton_constructor.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <LanguageGenerator/automaton_constructor.h>
#include <string>
#include <set>
#include <arpa/inet.h>

namespace LanguageGenerator {

using namespace std;

basic_automaton_holder::basic_automaton_holder()
{{{
	clear();
}}}

void basic_automaton_holder::clear()
{{{
	alphabet_size = 0;
	state_count = 0;
	start.clear();
	final.clear();
	transitions.clear();
}}}

bool basic_automaton_holder::construct(bool is_dfa, int alphabet_size, int state_count, set<int> &start, set<int> &final, transition_set &transitions)
{{{
	clear();

	this->is_dfa = is_dfa;
	this->alphabet_size = alphabet_size;
	this->state_count = state_count;
	this->start = start;
	this->final = final;
	this->transitions = transitions;

	return true;
}}}

basic_string<int32_t> basic_automaton_holder::serialize()
{{{
	basic_string<int32_t> ret;
	set<int>::iterator sit;
	transition_set::iterator tit;

	ret += 0; // length, filled in later.
	ret += htonl(alphabet_size);
	ret += htonl(state_count);
	ret += htonl(start.size());
	for(sit = start.begin(); sit != start.end(); sit++)
		ret += htonl(*sit);
	ret += htonl(final.size());
	for(sit = final.begin(); sit != final.end(); sit++)
		ret += htonl(*sit);
	ret += htonl(transitions.size());
	for(tit = transitions.begin(); tit != transitions.end(); tit++) {
		ret += htonl(tit->source);
		ret += htonl(tit->label);
		ret += htonl(tit->destination);
	}
	ret[0] = htonl(ret.length() - 1);

	return ret;
}}}

string basic_automaton_holder::generate_dotfile()
{{{
	if(empty())
		return "digraph empty_automaton { }\n";

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
	transition_set::iterator ti;
	for(ti = transitions.begin(); ti != transitions.end(); ti++) {
		snprintf(buf, 64, "\tS%d -> S%d [ label = \"%d\" ];\n", ti->source, ti->destination, ti->label);
		buf[63] = 0;
		ret += buf;
	}

	// add non-visible states for arrows to initial states
	header_written = false;
	for(sti = start.begin(); sti != start.end(); ++sti) {
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
	for(sti = start.begin(); sti != start.end(); ++sti) {
		snprintf(buf, 128, "\tiS%d -> S%d [ color = blue ];\n", *sti, *sti);
		ret += buf;
	}

	// end
	ret += "};\n";

	return ret;
}}}

}; // end of namespace LanguageGenerator

