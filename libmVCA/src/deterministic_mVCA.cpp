/* vim: fdm=syntax foldlevel=1 foldnestmax=2
 * $Id$
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
 *           and David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <libmVCA/mVCA.h>
#include <libmVCA/deterministic_mVCA.h>

#include <libmVCA/serialize.h>

#ifdef _WIN32
# include <stdio.h>
# include <winsock.h>
#else
# include <arpa/inet.h>
#endif

namespace libmVCA {

using namespace std;

deterministic_mVCA::~deterministic_mVCA()
{ };

set<int> deterministic_mVCA::transition(const set<int> & from, int & m, int label) const
{
	enum pushdown_direction dir;
	set<int> ret;

	dir = this->alphabet.get_direction(label);

	if(dir != DIR_INDEFINITE && m >= 0) {
		const_iterator dti;
		dti = transition_function.find( (m<=this->m_bound)?m:m_bound );
		if(dti != transition_function.end())
			ret = dti->second.transmute(from, label);
		m += dir;
	} else {
		m = -1;
	}

	return ret;
}

bool deterministic_mVCA::endo_transition(set<int> & states, int & m, int label) const
{
	enum pushdown_direction dir;

	dir = this->alphabet.get_direction(label);

	if(dir != DIR_INDEFINITE && m >= 0) {
		const_iterator dti;
		dti = transition_function.find( (m<=this->m_bound)?m:m_bound );
		if(dti != transition_function.end())
			dti->second.endo_transmute(states, label);
		else
			states.clear();
		m += dir;
		return true;
	} else {
		states.clear();
		m = -1;
		return false;
	}
}

basic_string<int32_t> deterministic_mVCA::serialize_derivate() const
{
	return ::serialize(transition_function);
}

bool deterministic_mVCA::deserialize_derivate(serial_stretch & serial)
{
	return ::deserialize(transition_function, serial);
}

void deterministic_mVCA::get_transition_map(map<int, map<int, map<int, set<int> > > > & postmap) const
{
	// create mappings with:
	// map[m][current_state][label] = { successor-states }
	postmap.clear();

	const_iterator tfi;
	map<int, map<int, int> >::const_iterator tfii;
	map<int, int>::const_iterator tfiii;

	for(tfi = transition_function.begin(); tfi != transition_function.end(); ++tfi)	// m
		for(tfii = tfi->second.transitions.begin(); tfii != tfi->second.transitions.end(); ++tfii) // current_state
			for(tfiii = tfii->second.begin(); tfiii != tfii->second.end(); ++tfiii) // label
				postmap[tfi->first][tfii->first][tfiii->first].insert(tfiii->second);
}

string deterministic_mVCA::get_transition_dotfile() const
{
	string ret;

	const_iterator tri;
	for(tri = transition_function.begin(); tri != transition_function.end(); ++tri)
		ret += tri->second.get_transition_dotfile(tri->first, this->m_bound);

	return ret;
}

void deterministic_mVCA::add_transition(int m, int src, int label, int dst)
{ transition_function[m].transitions[src][label] = dst; }

} // end of namespace libmVCA

