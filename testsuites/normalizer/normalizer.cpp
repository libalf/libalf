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

#include <iostream>
#include <iterator>
#include <fstream>

#include <libalf/alf.h>
#include <libalf/normalizer_msc.h>

#include <amore/vars.h>

using namespace std;
using namespace libalf;

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

normalizer * create_normalizer()
{{{
	int i;

	vector<int> total_order;
	vector<int> process_match;
	vector<int> buffer_match;
	int max_buffer_length;

	// total order
	int v_total_order[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
	for(i = 0; i < 24; i++)
		total_order.push_back(v_total_order[i]);

	// process match
	int v_process_match[] = {0,1, 0,1, 0,2, 0,2, 1,0, 1,0, 1,2, 1,2, 2,0, 2,0, 2,1, 2,1};
	for(i = 0; i < 24; i++)
		process_match.push_back(v_process_match[i]);

	// buffer match
	int v_buffer_match[] = {0,0, 0,0, 1,1, 1,1, 2,2, 2,2, 3,3, 3,3, 4,4, 4,4, 5,5, 5,5};
	for(i = 0; i < 24; i++)
		buffer_match.push_back(v_buffer_match[i]);

	max_buffer_length = 1;

	return new normalizer_msc(total_order, process_match, buffer_match, max_buffer_length);
}}}

list<int> create_word()
{{{
	list<int> ret;
	unsigned int i;
	int v_word[] = {0,2,4,4,5,0,1,3,1,5};

	for(i = 0; i < sizeof(v_word)/sizeof(int); i++)
		ret.push_back(v_word[i]);

	return ret;
}}}

int main()
{{{
	normalizer * norm;
	list<int> word, pnf, snf;
	bool pnf_bottom, snf_bottom;

	norm = create_normalizer();

	word = create_word();

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


