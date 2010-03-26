/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 * http://libalf.informatik.rwth-aachen.de/
 *
 * Stamina Interface for libalf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 */

#include <iostream>

#include "stamina.h"

#include <libalf/alf.h>
#include <libalf/algorithm_biermann_original.h>
#include <libalf/algorithm_RPNI.h>

using namespace std;
using namespace libalf;

int main(int argc, const char* argv[]) {

	stamina_set *s_set = NULL;

	/*
	 * Create stamina set depending on the command line args
	 */
	// One argument, supposed to be an integer
	if(argc == 2) {

		int problem_number = atoi(argv[1]);

		if(problem_number <= 0) {
			cout << "stamina: invalid problem number specified" << endl;
			exit(-1);
		}

		s_set = stamina_set::create_stamina_set(problem_number);

	}

	// Two arguments, interpreted as training and test set filenames
	else if (argc == 3) {
		s_set = stamina_set::create_stamina_set(argv[1], argv[2]);
	}

	// Invalid arguments
	else {
		cout << "stamina: no problem specified" << endl;
		cout << "stamina: use \"stamina <problem_number>\" or \"stamina <training_set_file> <test_set_file>\"" << endl;
		exit(-1);
	}


	// Create libalf
	knowledgebase<bool> base;
	//original_biermann<bool> algorithm = original_biermann<bool>(&base, NULL, s_set->get_alphabet_size(), 4);
	RPNI<bool> algorithm = RPNI<bool>(&base, NULL, s_set->get_alphabet_size());

	// Obtain data
	map<list<int>, bool> training_set = s_set->get_training_set();
	vector<list<int> > test_set = s_set->get_test_set();

#ifdef VERBOSE
	cout << "Size of training set = " << training_set.size() << endl;
	cout << "Alphabet size is " << s_set->get_alphabet_size() << endl;
#endif

	// Add training set to knowledgebase
	map<list<int>, bool>::iterator training_it;
	for(training_it = training_set.begin(); training_it != training_set.end(); training_it++) {
		bool classification = (*training_it).second;
		list<int> word = (*training_it).first;

		base.add_knowledge(word, classification);

	}

	// Compute conjecture
	conjecture *cj = algorithm.advance();
	simple_moore_machine *result = dynamic_cast<simple_moore_machine*>(cj);

#ifdef VERBOSE
	cout << result->visualize() << endl;
#endif

	// Classify test set
	stringstream submit;
	vector<list<int> >::iterator test_it;
	for(test_it = test_set.begin(); test_it != test_set.end(); test_it++) {

		list<int> word = *test_it;

		bool classification = run(result, word);

		if(classification)
			submit << "1";
		else
			submit << "0";
	}

	cout << submit.str() << endl;

	// Clean memory
	delete result;
	delete s_set;

	return 0;
}

