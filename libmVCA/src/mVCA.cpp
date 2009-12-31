/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libmVCA.
 *
 * libmVCA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libmVCA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libmVCA.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <libmVCA/mVCA.h>
// for libmVCA::deserialize :
#include <libmVCA/deterministic_mVCA.h>
#include <libmVCA/nondeterministic_mVCA.h>

namespace libmVCA {

using namespace std;

static const char* libmVCA_version_string = "libmVCA version " VERSION;

const char* libmVCA_version()
{{{
	return libmVCA_version_string;
}}}



void mVCA::set_alphabet(pushdown_alphabet & alphabet)
{{{
	this->alphabet = alphabet;
}}}
void mVCA::unset_alphabet()
{{{
	this->alphabet.clear();
}}}
pushdown_alphabet mVCA::get_alphabet()
{{{
	return this->alphabet;
}}}
int mVCA::get_alphabet_size()
{{{
	return this->alphabet.get_alphabet_size();
}}}

int mVCA::get_initial_state()
{{{
	return initial_state;
}}}
set<int> mVCA::get_initial_states()
{{{
	set<int> ret;
	ret.insert(initial_state);
	return ret;
}}}
set<int> mVCA::get_final_states()
{{{
	return final_states;
}}}
bool mVCA::set_initial_state(int state)
{{{
	if(state >= 0 && state < state_count) {
		this->initial_state = state;
		return true;
	} else {
		return false;
	}
}}}
bool mVCA::set_final_state(const set<int> & states)
{{{
	set<int>::iterator si;
	for(si = states.begin(); si != states.end(); ++si)
		if(*si < 0 || *si >= state_count)
			return false;
	final_states = states;
	return true;
}}}

bool mVCA::contains_initial_states(const set<int> & states)
{{{
	set<int>::iterator si;
	for(si = states.begin(); si != states.end(); ++si)
		if(*si == initial_state)
			return true;
	return false;
}}}
bool mVCA::contains_final_states(const set<int> & states)
{{{
	set<int>::iterator si;
	for(si = states.begin(); si != states.end(); ++si)
		if(final_states.find(*si) != final_states.end())
			return true;
	return false;
}}}


set<int> mVCA::run(const set<int> & from, int & m, list<int>::iterator word, list<int>::iterator word_limit)
{
	
}

list<int> mVCA::shortest_run(const set<int> & from, int & m, const set<int> & to, bool &reachable)
{
	
}

bool mVCA::contains(list<int> & word)
{{{
	return this->contains(word.begin(), word.end());
}}}
bool mVCA::contains(list<int>::iterator word, list<int>::iterator word_limit)
{{{
	set<int> s;
	set<int> ini;
	int m = 0;
	ini = this->get_initial_states();
	s = this->run(ini, m, word, word_limit);
	// FIXME: (m == 0) ?
	return contains_final_states(s);
}}}
// obtain shortest word in language resp. test if language is empty,
list<int> mVCA::get_sample_word(bool & is_empty)
{{{
	set<int> i = get_initial_states();
	set<int> f = get_final_states();
	bool reachable;
	list<int> w;
	int m = 0;
	w = shortest_run(i, m, f, reachable);
	// FIXME: (m == 0) ?
	is_empty = ! reachable;
	return w;
}}}
bool mVCA::is_empty()
{{{
	bool ret;
	get_sample_word(ret);
	return ret;
}}}

basic_string<int32_t> mVCA::serialize()
{
	
}
bool mVCA::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
	
}



mVCA * deserialize_mVCA(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
	
}


} // end of namespace libmVCA

