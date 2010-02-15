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
#include <libmVCA/deterministic_mVCA.h>

#ifdef _WIN32
# include <stdio.h>
# include <winsock.h>
#else
# include <arpa/inet.h>
#endif

namespace libmVCA {

using namespace std;


set<int> deterministic_mVCA::transition(const set<int> & from, int & m, int label)
{{{
	enum pushdown_direction dir;
	set<int> ret;

	dir = this->alphabet.get_direction(label);

	if(dir != DIR_INDEFINITE && m >= 0) {
		ret = transition_function[ (m<=this->m_bound)?m:m_bound ].transmute(from, label);
		m += dir;
	} else {
		m = -1;
	}

	return ret;
}}}

bool deterministic_mVCA::endo_transition(set<int> & states, int & m, int label)
{{{
	enum pushdown_direction dir;

	dir = this->alphabet.get_direction(label);

	if(dir != DIR_INDEFINITE && m >= 0) {
		transition_function[ (m<=this->m_bound)?m:m_bound ].endo_transmute(states, label);
		m += dir;
		return true;
	} else {
		states.clear();
		m = -1;
		return false;
	}
}}}

basic_string<int32_t> deterministic_mVCA::serialize_derivate()
{{{
	basic_string<int32_t> ret;

	ret += htonl(this->m_bound + 1);

	for(int sigma = 0; sigma <= this->m_bound; ++sigma)
		ret += transition_function[sigma].serialize();

	return ret;
}}}

bool deterministic_mVCA::deserialize_derivate(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit, int & progress)
{
	
}

string deterministic_mVCA::get_transition_dotfile()
{
	
}

void deterministic_mVCA::get_transition_map(map<int, map<int, map<int, set<int> > > > & postmap)
{{{
	// create mappings with:
	// map[m][current_state][label] = { successor-states }
	postmap.clear();

	map<int, deterministic_transition_function>::iterator tfi;
	map<int, map<int, int> >::iterator tfii;
	map<int, int>::iterator tfiii;

	for(tfi = transition_function.begin(); tfi != transition_function.end(); ++tfi)	// m
		for(tfii = tfi->second.transitions.begin(); tfii != tfi->second.transitions.end(); ++tfii) // current_state
			for(tfiii = tfii->second.begin(); tfiii != tfii->second.end(); ++tfiii) // label
				postmap[tfi->first][tfii->first][tfiii->first].insert(tfiii->second);
}}}

} // end of namespace libmVCA

