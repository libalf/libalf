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
#include <libmVCA/p-automaton.h>

using namespace std;
using namespace libmVCA;


void print_word(list<int> &word)
{{{
	printf(".");
	for(list<int>::iterator l = word.begin(); l != word.end(); l++)
		printf("%d.", *l);
}}}

pushdown_alphabet get_alphabet()
{{{
	pushdown_alphabet ret(3);
	ret.set_direction(0, DIR_UP);
	ret.set_direction(1, DIR_STAY);
	ret.set_direction(2, DIR_DOWN);
	return ret;
}}}

mVCA * get_aNbcN1()
{{{
	map<int, map<int, map<int, set<int> > > > transitions;

	pushdown_alphabet al = get_alphabet();

	set<int> final;
	final.insert(1);


	transitions[0][0][0].insert(0);

	transitions[1][0][0].insert(0);
	transitions[1][1][2].insert(1);
	transitions[1][0][1].insert(1);

	return construct_mVCA(/*state-count*/ 2, al, /*initial state*/ 0, final, /*m_bound*/ 1, transitions);
}}};

mVCA * get_aNbcN3()
{{{
	map<int, map<int, map<int, set<int> > > > transitions;

	pushdown_alphabet al = get_alphabet();

	set<int> final;
	final.insert(1);

	transitions[0][0][0].insert(0);

	transitions[1][0][0].insert(0);
	transitions[1][1][2].insert(1);

	transitions[2][0][0].insert(0);
	transitions[2][1][2].insert(1);

	transitions[3][0][0].insert(0);
	transitions[3][1][2].insert(1);
	transitions[3][0][1].insert(1);

	return construct_mVCA(/*state-count*/ 2, al, /*initial state*/ 0, final, /*m_bound*/ 3, transitions);
}}};

int main(int argc, char**argv)
{
	mVCA *aNbcN1, *aNbcN3, *intersect;
	list<int> word;
	bool is_empty;

	aNbcN1 = get_aNbcN1();
	aNbcN3 = get_aNbcN3();
	intersect = aNbcN1->lang_intersect(*aNbcN3);

	printf("sample word of intersect: ");
	word = intersect->get_sample_word(is_empty);
	if(is_empty) {
		printf("none (intersect empty)\n");
	} else {
		print_word(word);
		printf("\n");
	}

	ofstream file;
	file.open("aNbcN1.dot"); file << aNbcN1->generate_dotfile(); file.close();
	file.open("aNbcN3.dot"); file << aNbcN3->generate_dotfile(); file.close();
	file.open("intersect.dot"); file << intersect->generate_dotfile(); file.close();

	list<int> counterexample;
	if(aNbcN1->lang_subset_of(*aNbcN3, counterexample)) {
		printf("m1 subset of m3\n");
	} else {
		printf("m1 NOT subset of m3 (cex: ");
		print_word(counterexample);
		printf(")\n");
	}

	printf("\n");

	if(aNbcN3->lang_subset_of(*aNbcN1, counterexample)) {
		printf("m3 subset of m1\n");
	} else {
		printf("m3 NOT subset of m1 (cex: ");
		print_word(counterexample);
		printf(")\n");
	}

	printf("\n");

	if(aNbcN3->lang_disjoint_to(*aNbcN1, counterexample)) {
		printf("m3 is disjoint to m1\n");
	} else {
		printf("m3 NOT disjoint to m1 (cex: ");
		print_word(counterexample);
		printf(")\n");
	}
}

