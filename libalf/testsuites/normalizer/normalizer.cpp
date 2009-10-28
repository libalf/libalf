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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <iterator>
#include <fstream>

#include <libalf/alf.h>
#include <libalf/normalizer_msc.h>

using namespace std;
using namespace libalf;


normalizer * create_normalizer()
{{{
// NOTE: this is the OLD normalizer that is commented out.
	// alphabet:
	//
	// /message type	/letter
	//
	// a->b(0)		0,1
	// a->b(1)		2,3
	// a->c(0)		4,5
	// a->c(1)		6,7
	//
	// b->a(0)		8,9
	// b->a(1)		10,11
	// b->c(0)		12,13
	// b->c(1)		14,15
	//
	// c->a(0)		16,17
	// c->a(1)		18,19
	// c->b(0)		20,21
	// c->b(1)		22,23

	unsigned int i;

	vector<int> total_order;
	vector<int> process_match;
	vector<int> buffer_match;
	int max_buffer_length;

	// total order
	printf("total order:   ");
//	int v_total_order[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
	int v_total_order[] = {0,1,2,3,4,5};
	for(i = 0; i < sizeof(v_total_order)/sizeof(int); i++) {
		printf("%d ", v_total_order[i]);
		total_order.push_back(v_total_order[i]);
	}
	printf(";\n");

	// process match
	printf("process match: ");
//	int v_process_match[] = {0,1, 0,1, 0,2, 0,2, 1,0, 1,0, 1,2, 1,2, 2,0, 2,0, 2,1, 2,1};
	int v_process_match[] = {0,1,1,0,0,1};
	for(i = 0; i < sizeof(v_process_match)/sizeof(int); i++) {
		printf("%d ", v_process_match[i]);
		process_match.push_back(v_process_match[i]);
	}
	printf(";\n");

	// buffer match
//	int v_buffer_match[] = {0,0, 0,0, 1,1, 1,1, 2,2, 2,2, 3,3, 3,3, 4,4, 4,4, 5,5, 5,5};
	int v_buffer_match[] = {0,0,1,1,0,0};
	printf("buffer match:  ");
	for(i = 0; i < sizeof(v_buffer_match)/sizeof(int); i++) {
		printf("%d ",v_buffer_match[i]);
		buffer_match.push_back(v_buffer_match[i]);
	}
	printf(";\n");

	max_buffer_length = 1;
	printf("buffer length: %d\n\n\n", max_buffer_length);

	return new normalizer_msc(total_order, process_match, buffer_match, max_buffer_length);
}}}

list<int> create_word()
{{{
	list<int> ret;
	unsigned int i;
	int v_word[] = {0, 1, 2, 3, 4, 5, 2, 4, 3, 5};

	for(i = 0; i < sizeof(v_word)/sizeof(int); i++)
		ret.push_back(v_word[i]);

	return ret;
}}}

list<int> read_word(int argc, char**argv)
{{{
	list<int> ret;

	for(int i = 1; i < argc; i++)
		ret.push_back(atoi(argv[i]));

	return ret;
}}}

int main(int argc, char**argv)
{{{
	normalizer * norm;
	list<int> word, pnf, snf;
	bool pnf_bottom, snf_bottom;

	norm = create_normalizer();

	word = read_word(argc, argv);

	cout << "word: ";
	print_word(cout, word);

	cout << "\n PNF: ";
	pnf = norm->prefix_normal_form(word, pnf_bottom);
	print_word(cout, pnf);
	if(pnf_bottom)
		cout << " (bottom)";

	cout << "\n SNF: ";
	snf = norm->suffix_normal_form(word, snf_bottom);
	print_word(cout, snf);
	if(snf_bottom)
		cout << " (bottom)";
	cout << "\n";

	delete norm;

	return 0;
}}}


