/*
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
 * (c) 2009 Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 *
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

/*
 * This class demonstrates how to use online algorithms in libalf. In this
 * example, the user is queried for classified examples that are then used to
 * compute the final result.
 *
 * This program takes the alphabet size as the input and produces the conjecture
 * as the output by requesting the user to classify the queries/words computed
 * in the knowledgebase. As the first step, a knowledgebase is created.
 * The user is then required to provide information about the
 * alphabet size of the automaton. Once this information is obtained, a learning
 * algorithm is created by passing the type of algorithm, empty knowledgebase,
 * and the alphabet size as the parameters. The learning algorithm advances
 * iteratively, checking at every step, if enough information exists in the
 * knowledgebase to construct a conjecture. If there was no enough information
 * to compute a conjecture, a list of words (called queries) that are to be
 * classifed by the user is produced. The words are presented to the user for
 * classification and this information is added to the knowledgebase. When
 * during an advance, if a conjecture was computed, then it is presented to the
 * user to check its equivalence. If the conjecture was correct, the file
 * "output_online.dot" containing the code for the automaton is created.
 * However, if the conjecture is incorrect, then the user is prompted to provide
 * a counter example which is used in furthur construction of the conjecture. In
 * this demo, the ANGLUIN algorithm is used.
 */

#include <iostream>
#include <string>
#include <list>

// Used to define the libalf name space.
#include <libalf/alf.h>
// Angluin's L* algorithm
#include <libalf/algorithm_angluin.h>

using namespace std;
using namespace libalf;

/*
 * Print a short information text.
 */
void print_Help() {

	cout << endl;
	cout << "This example demonstrates how to use libalf's online algorithms. It puts" << endl;
	cout << "the user, i.e. you, in the position of the teacher. Using libalf is easy:" << endl;
	cout << endl;
	cout << "1) Input the size of the alphabet you want to use." << endl << endl;
	cout << "   libalf uses integers as symbols. This means that a word is a sequence" << endl;
	cout << "   of integers in the range between 0 and the size of the alphabet - 1." << endl << endl;
	cout << "   For reasons of easier parsing, you are only allowed alphabets up to" << endl;
	cout << "   ten symbols in this example (which is, however, no restriction in" << endl;
	cout << "   general)." << endl << endl;
	cout << "2) Answer the queries (simply follow the instructions on the screen)." << endl << endl;
	cout << "   Membership queries have to be classified as belonging to the target" << endl;
	cout << "   language (1) or not (0)." << endl << endl;
	cout << "   Conjectures to equivalence queries are given in the Graphviz dot" << endl;
	cout << "   format. You can use the dot tool to draw the automata." << endl;
	cout << endl;
}

/*
 * Request the alphabet size from the user. The alphabet size has to be between
 * 1 and 9.
 */
int get_AlphabetSize() {
	int i;
	cout << "Please insert the alphabet size (between 1 and 10): ";
	cin >> i;

	return i;
}

/*
 * Requests a counter-example from the user. The counter-example must not
 * contain illegal characters, i.e. not 0, ..., alphabetsize - 1.
 */
list<int> get_CounterExample(int alphabetsize) {
	list<int> cex;
	string c;

	bool ok;
	do {
		ok = true;

		cout << "Please enter a counter example (dot '.' as epsilon): ";
		cin >> c;
		if(c == ".")
			c = "";

		unsigned int i;
		for (i = 0; i < c.length(); i++) {
			if (c.at(i) < '0' || c.at(i) > ('0' + alphabetsize - 1)) {
				cout << "Found illegal character " << c.at(i) << endl;
				ok = false;
				cex.clear();
				break;
			}
			cex.push_back(c.at(i) - '0');
		}
	} while (!ok);

	return cex;
}

/*
 * Function to check if the correct conjecture is computed. The conjecture is
 * displayed to the user. The user classifies by entering 'y' to mark it correct
 * and 'n' mark it incorrect.
 */
bool check_Equivalence(conjecture * cj) {

	// display the automaton
	if (cj != NULL) {
		finite_automaton * a = dynamic_cast<finite_automaton*> (cj);
		cout << endl << "Conjecture:" << endl << endl;
		cout << a->visualize();
	}

	// query the user for an answer and retrieve the answer.
	string answer;
	do {
		cout << "Please specify whether the conjecture is equivalent (y/n): ";
		cin >> answer;
		if (answer.compare("y") == 0)
			return true;
		else if (answer.compare("n") == 0)
			return false;
		else
			cout << "Wrong Input" << endl;
	} while (true);
}

/*
 * Function used to obtain information about the classification of a word.
 * The Query is presented to the user.
 * The user classifies the word by typing "y" for acceptance and "n" for
 * rejection.
 */
bool answer_Membership(list<int> query) {

	string answer;
	do {
		cout << "Please classify the word '";

		// print the query on screen
		list<int>::iterator it;
		for (it = query.begin(); it != query.end(); it++)
			cout << *it;
		cout << "' (0/1): ";
		cin >> answer;
		if (answer.compare("1") == 0)
			return true;
		else if (answer.compare("0") == 0)
			return false;
		else
			cout << "Wrong Input" << endl;
	} while (true);

	return answer.compare("1") == 0 ? true : false;
}

/*
 * The main method
 */
int main(int argc, char**argv) {

	// First, print some help info
	print_Help();

	// Get alphabet size
	int alphabet_size = get_AlphabetSize();

	// Create new knowledgebase. In this case, we choose bool as type for the knowledgebase.
	knowledgebase<bool> base;

	// Create learning algorithm (Angluin L*) without a logger (2nd argument is NULL) and alphabet size alphabet_size
	angluin_simple_table<bool> algorithm(&base, NULL, alphabet_size);

	conjecture * result = NULL;

	/*
	 * The method "advance" is iterated in a loop which checks if there is
	 * enough information to formulate a conjecture. If there was no enough
	 * information for the same, the method creates a list of words that are
	 * to be classified by the user. This is referred to as "queries". The
	 * user answers the query and the answer is added to the knowledgebase.
	 * This information may either give enough knowledge to the algorithm to
	 * produce a conjecture or may produce more queries to be resolved. This
	 * is identified when "advance" is invoked in the next iteration.
	 *
	 * On the otherhand, If a conjecture was computed, it is displayed on the
	 * screen and the user classifies it as correct or incorrect.
	 * If marked correct, the conjecture is stored in a variable
	 * named "automaton" which is later used to construct the ".dot" file.
	 * If the conjecture is rejected, the algorithm requires a counter
	 * example from the user and iteration is continued.
	 */
	do {

		// Advance the learning algorithm
		conjecture * cj = algorithm.advance();

		// Resolve membership queries
		if (cj == NULL) {

			// retrieve queries
			list<list<int> > queries = base.get_queries();

			// iterate through all queries
			list<list<int> >::iterator li;
			for (li = queries.begin(); li != queries.end(); li++) {

				// Answer query
				bool a = answer_Membership(*li);

				// Add answer to knowledgebase
				base.add_knowledge(*li, a);
			}
		}
		// Resolve equivalence queries
		else {
			bool is_equivalent = check_Equivalence(cj);

			if (is_equivalent) {
				result = cj;
			} else {
				// Get a counter-example
				list<int> ce = get_CounterExample(alphabet_size);

				// Add counter-example to algorithm
				algorithm.add_counterexample(ce);

				// Delete old conjecture
				delete cj;
			}
		}

	} while (result == NULL);

	//Display the result on the screen.
	cout << endl << "Result:" << endl << result->visualize() << endl;

	// Delete result
	delete result;

	return 0;
}
