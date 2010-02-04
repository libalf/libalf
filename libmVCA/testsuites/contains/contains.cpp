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
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <ostream>
#include <fstream>
#include <iterator>
#include <list>

#include <stdlib.h>

#include <libmVCA/mVCA.h>
#include <libmVCA/deterministic_mVCA.h>
#include <libmVCA/pushdown.h>

using namespace std;
using namespace libmVCA;


void print_word(list<int> &word)
{{{
	printf(".");
	for(list<int>::iterator l = word.begin(); l != word.end(); l++)
		printf("%d.", *l);
}}}


int main(int argc, char**argv)
{
	set<int> up, stay, down;
	set<int> final;
	mVCA * m;
	map<int, map<int, map<int, set<int> > > > transitions;
	list<int> word;

	up.insert(0);
	stay.insert(1);
	down.insert(2);

	final.insert(1);

	// transitions: m -> state -> sigma -> states
	transitions[0][0][0].insert(0);

	transitions[1][0][0].insert(0);
	transitions[1][1][2].insert(1);

	transitions[2][0][0].insert(0);
	transitions[2][1][2].insert(1);
	transitions[2][0][1].insert(1);

	m = construct_mVCA(2, 3, up, stay, down, 0, final, 2, transitions);

	cout << "this test defines a simple mVCA that should accept the language a^n b c^n with n >= 2.\n\n";

	cout << "derivate type: " << (int)m->get_derivate_id() << "\n";

	cout << "checking words:\n\n";

	print_word(word); printf(" : %c\n", m->contains(word) ? '+' : '-');

	word.push_front(1);
	print_word(word); printf(" : %c\n", m->contains(word) ? '+' : '-');

	word.push_front(0);
	print_word(word); printf(" : %c\n", m->contains(word) ? '+' : '-');

	word.push_back(2);
	print_word(word); printf(" : %c\n", m->contains(word) ? '+' : '-');

	word.push_front(0);
	print_word(word); printf(" : %c\n", m->contains(word) ? '+' : '-');

	word.push_back(2);
	print_word(word); printf(" : %c\n", m->contains(word) ? '+' : '-');

	word.push_front(0);
	print_word(word); printf(" : %c\n", m->contains(word) ? '+' : '-');

	word.push_back(2);
	print_word(word); printf(" : %c\n", m->contains(word) ? '+' : '-');
}
