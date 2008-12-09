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

#include <amore/vars.h>


using namespace std;
using namespace libalf;

void prepare_sqt(structured_query_tree<extended_bool> & sqt)
{{{
	structured_query_tree<extended_bool>::iterator qi;
	list<int> word;
	list<extended_bool> acceptance;
	extended_bool acc;

	word.push_back(0);
	word.push_back(0);
	word.push_back(3);
	word.push_back(1);
	word.push_back(2);

	sqt.add_query(word, 3); // .0.0. & .0.0.3. & .0.0.3.1. & .0.0.3.1.2.

	word.clear();

	word.push_back(0);
	word.push_back(1);
	word.push_back(2);
	word.push_back(0);
	word.push_back(3);

	sqt.add_query(word, 5); // . & .0. & .0.1. & .0.1.2. & .0.1.2.0. & .0.1.2.0.3.

	qi = sqt.begin();
	acc.value = extended_bool::EBOOL_FALSE; acceptance.push_back(acc);
	acc.value = extended_bool::EBOOL_FALSE; acceptance.push_back(acc);
	acc.value = extended_bool::EBOOL_UNKNOWN; acceptance.push_back(acc);
	acc.value = extended_bool::EBOOL_TRUE; acceptance.push_back(acc);
	qi->set_answers(acceptance);

	qi++;
	acceptance.clear();
	acc.value = extended_bool::EBOOL_FALSE; acceptance.push_back(acc);
	acc.value = extended_bool::EBOOL_TRUE; acceptance.push_back(acc);
	acc.value = extended_bool::EBOOL_TRUE; acceptance.push_back(acc);
	acc.value = extended_bool::EBOOL_UNKNOWN; acceptance.push_back(acc);
	acc.value = extended_bool::EBOOL_FALSE; acceptance.push_back(acc);
	acc.value = extended_bool::EBOOL_TRUE; acceptance.push_back(acc);
	qi->set_answers(acceptance);
}}}

int main()
{
	structured_query_tree<extended_bool> sqt;
	structured_query_tree<extended_bool> new_sqt;
	basic_string<int32_t> serialized;
	basic_string<int32_t>::iterator si;

	prepare_sqt(sqt);

	cout << "initial SQT: ---------\n";
	sqt.print(cout);
	cout << "----------------------\n";

	serialized = sqt.serialize();

	si = serialized.begin();

	if( ! new_sqt.deserialize(si, serialized.end()) ) {
		cout << "deserialization failed!\n";
	}

	if( si != serialized.end() ) {
		cout << "ERROR: not at end of serialized data!\n";
	}

	cout << "\nserialized data: ";
	print_basic_string_2hl(serialized, cout);

	cout << "\n\n";
	cout << "final SQT: -----------\n";
	new_sqt.print(cout);
	cout << "----------------------\n";
}


