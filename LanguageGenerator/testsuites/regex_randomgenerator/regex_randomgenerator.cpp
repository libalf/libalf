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

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <LanguageGenerator/regex_randomgenerator.h>

using namespace LanguageGenerator;
using namespace std;

int main(int argc, char**argv)
{
	int alphabet_size;
	int num_op;

	if(argc != 3) {
		cout << "please give <alphabet size> and <operand count> as parameters\n";
		return 1;
	}

	alphabet_size = atoi(argv[1]);
	num_op = atoi(argv[2]);

	string regex;
	regex_randomgenerator rrg;

	float p_sigma[alphabet_size];
	float peps, pcon, puni, pstar;
	for(int i = 0; i < alphabet_size; i++)
		p_sigma[i] = 0.2;
	peps = 0;
	pcon = alphabet_size;
	puni = alphabet_size;
	pstar = alphabet_size;

	regex = rrg.generate(num_op, alphabet_size, p_sigma, peps, pcon, puni, pstar);

	cout << regex << "\n";

	return 0;
}

