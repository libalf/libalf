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

#include <libalf/alphabet.h>

using namespace std;
using namespace libalf;

int main()
{
	list<int> word;

	for(int i = 0; i < 20; i++) {
		cout << "(" << word2string(word) << ")++\n";
		inc_graded_lex(word, 3);
	}

	for(int i = 0; i < 20; i++) {
		dec_graded_lex(word, 3);
		cout << "--(" << word2string(word) << ")\n";
	}

	return 0;
}

