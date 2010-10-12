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
 * (c) 2010 David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#define ANGLUIN_GENERIC_HYPOTHESIS
#include <libalf/alf.h>
#include <libalf/algorithm_angluin.h>


#include "my_answer.h"


using namespace std;
using namespace libalf;


string set2string(const set<int> & s)
{{{
	stringstream str;
	set<int>::const_iterator si;

	str << "{ ";
	si = s.begin();
	if(si != s.end()) {
		str <<  *si;
		++si;
		while(si != s.end()) {
			str << ", " << *si;
			++si;
		};
	}
	str << " }";

	return str.str();
}}}

string conjecture2string(const moore_machine<my_answer> & cj)
{{{
	stringstream str;

	str << "{\n";

	str << "\tvalid: " << (cj.valid ? "true" : "false") << "\n";

	str << "\tdeterministic: " << (cj.is_deterministic ? "true" : "false") << "\n";

	str << "\tinput alphabet size: " << (cj.input_alphabet_size) << "\n";

	str << "\tstate count: " << (cj.state_count) << "\n";

	str << "\tinitial states: " << (set2string(cj.initial_states)) << "\n";

	str << "\tomega: " << (cj.omega ? "true" : "false") << "\n";

	str << "\toutput mapping:\n";
	map<int, my_answer>::const_iterator oi;
	for(oi = cj.output_mapping.begin(); oi != cj.output_mapping.end(); ++oi)
		str << "\t\t" << oi->first << " -> " << oi->second << "\n";
	str << "\n";

	str << "\ttransitions:\n";
	map<int, map<int, set<int> > >::const_iterator tri;
	map<int, set<int> >::const_iterator mi;
	for(tri = cj.transitions.begin(); tri != cj.transitions.end(); ++tri)
		for(mi = tri->second.begin(); mi != tri->second.end(); ++mi)
			str << "\t\t(" << tri->first << ", " << mi->first << ") -> " << set2string(mi->second) << "\n";
	str << "\n";

	str << "}\n";

	return str.str();
}}};

// by user-interaction, get a valid integer
int user_get_int(string msg, bool must_be_positive)
{{{
	char * line;
	char * end;
	bool ok = false;

	int ret;

	while(!ok) {
		line = readline(msg.c_str());

		ret = strtoll(line, &end, 10);
		if(*end == '\0')
			ok = !must_be_positive || (ret >= 0);

		free(line);
	};

	return ret;
}}};

// by user-interaction, get a char in given bounds
char user_get_char(string msg, char lower_limit, char upper_limit)
{{{
	char * line;
	bool ok = false;

	char ret;

	while(!ok) {
		line = readline(msg.c_str());

		if(strlen(line) == 1) {
			ret = line[0];
			ok = (ret >= lower_limit && ret <= upper_limit);
		}

		free(line);
	};

	return ret;
}}};

// by user-interaction, get a boolean
bool user_get_bool(string msg)
{{{
	char * line;
	bool ok = false;

	bool ret;
	char c;

	while(!ok) {
		line = readline(msg.c_str());

		if(strlen(line) == 1) {
			c = line[0];
			if(c == 'y' || c == 'Y' || c == 't' || c == 'T' || c == '+') {
				ret = true;
				ok = true;
			}
			if(c == 'n' || c == 'N' || c == 'f' || c == 'F' || c == '-') {
				ret = false;
				ok = true;
			}
		}

		free(line);
	};

	return ret;
}}};

list<int> user_get_word(string msg)
{{{
	stringstream str (stringstream::in | stringstream::out);
	int i;
	list<int> ret;

	cout << msg << "\nplease give one int per line, using anything >= " << my_answer::get_alphabet_size() << " as end-of-word marker.\n";

	while( (i = user_get_int("next letter ", true)) <= my_answer::get_alphabet_size())
		ret.push_back(i);

	return ret;
}}};

int main(int argc, char**argv)
{
	ostream_logger log(&cout, LOGGER_DEBUG);
	knowledgebase<my_answer> knowledge;

	int alphabet_size = user_get_int("please enter alphabet size. ", true);

	int membership_queries = 0;
	int equivalence_queries = 0;

	bool success = false;


	angluin_simple_table<my_answer> ot(&knowledge, &log, alphabet_size);
	conjecture * cj = NULL;
	moore_machine<my_answer> * mm = NULL;

	while(!success) {
		if(cj) {
			delete cj;
			cj = NULL;
		};

		while( NULL == (cj = ot.advance()) ) {
			// resolve missing knowledge
			knowledgebase<my_answer>::iterator ki;
			while( (ki = knowledge.qbegin()) != knowledge.qend() ) {
				++membership_queries;
				list<int> qw;
				my_answer a;
				qw = ki->get_word();
				a = user_get_char("classify " + word2string(qw) + " ", 'a', 'a' + my_answer::get_alphabet_size() - 1);
				ki->set_answer(a);
			}
		}

		// do equivalence query
		++equivalence_queries;
		mm = dynamic_cast<moore_machine<my_answer>* >(cj);
		if(!mm) {{{ // debug
			cerr << "dynamic cast of conjecture failed. aborting.\n";
			return 1;
		}}}
		cout << conjecture2string(*mm);
		success = user_get_bool("is this the searched automaton? ");
		if(!success) {
			list<int> counterexample;
			counterexample = user_get_word("please give counterexample. ");
			ot.add_counterexample(counterexample);
		}
	};

	cout << "\n\nnumber of uniq membership queries: " << membership_queries << "\n";
	cout << "number of equivalence queries: " << equivalence_queries << "\n";
	cout << "final automaton:\n" << conjecture2string(*mm) << "\n";

	delete cj;

	return 0;
}

