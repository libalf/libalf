/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * DFArandomgenerator: testsuite for DFA random generator.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>
#include <fstream>

#include <gmpxx.h>

#include <LanguageGenerator/DFArandomgenerator.h>
#include <LanguageGenerator/automaton_constructor.h>

using namespace LanguageGenerator;
using namespace std;

int main(int argc, char**argv)
{
	DFArandomgenerator rag;

	int alphabet_size;
	int state_count;

	mpz_class c;

	list<int> K;
	list<int>::iterator ki;

	if(argc != 3) {
		cout << "please give <alphabet size> and <state count> as parameters\n";
		return 1;
	}

	alphabet_size = atoi(argv[1]);
	state_count = atoi(argv[2]);
	cout << "alphabet size: " << alphabet_size << "\nstate count: " << state_count << "\n\n";

	// print C[m,t,p] for given data
	c = rag.getTableContent(alphabet_size, state_count*(alphabet_size-1), state_count);
	cout << "C[" << alphabet_size << "," << state_count*(alphabet_size-1) << "," << state_count
		<< "] = " << c << "\n";

/*
	// get random element of K and print it
	for(int i = 0; i < 5; i++) {
		K = rag.randomElementOfK(alphabet_size, state_count*(alphabet_size-1), state_count);

		cout << "K: ( ";
		for(ki = K.begin(); ki != K.end(); ++ki)
			cout << (*ki) << " ";
		cout << ")\n";
	}
*/

	cout << "\n";

	// generate random automaton and store it
	basic_automaton_holder automaton;

	rag.generate(alphabet_size, state_count, automaton);

	ofstream file;

	file.open("random.dot");
	file << automaton.generate_dotfile();
	file.close();

	return 0;
}

