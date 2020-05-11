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
 * This class demonstrates how to use offline algorithms in libalf. In this
 * example, the user acts as information source and has to provide a set of
 * classified examples.
 *
 * The program takes the sample words and their classifications as the input,
 * computes the conjecture and produces a Graphviz representation of the 
 * conjecture as output.
 *
 * In the beginning, knowledgebase is created. The user is first required to
 * mention the alphabet size. Then, the user is requested to list all the sample
 * words and classify them accordingly as whether they have to be accepted or
 * rejected by the automaton. This information is simultaneously stored in the
 * knowledgebase. After the user has completed providing the samples, the
 * learning algorithm is created with the algorithm type, the knowledgebase, and
 * the alphabet size as parameters. The learning algorithm is then made to advance
 * by which it constructs a conjecture from the information available in the
 * knowledgebase. In this demo, the RPNI algorithm is used.
 */

#include <iostream>
#include <string>
#include <list>

// Used to define the libalf name space.
#include <libalf/alf.h>
// The RPNI algorithm
#include <libalf/algorithm_RPNI.h>

using namespace std;
using namespace libalf;

/*
 * Print a short information text.
 */
void print_Help() {

	cout << endl;
	cout << "This example demonstrates how to use libalf's offline algorithms. It puts" << endl;
	cout << "the user, i.e. you, in the position of the teacher. Using libalf is easy:" << endl;
	cout << endl;
	cout << "1) Input the size of the alphabet you want to use." << endl << endl;
	cout << "   libalf uses integers as symbols. This means that a word is a sequence" << endl;
	cout << "   of integers in the range between 0 and the size of the alphabet - 1." << endl << endl;
	cout << "   For reasons of easier parsing, you are only allowed alphabets up to" << endl;
	cout << "   ten symbols in this example (which is, however, no restriction in" << endl;
	cout << "   general)." << endl << endl;
	cout << "2) Provide a list of classified words. A classification is either 1 (the" << endl;
	cout << "   word has to be accepted) or 0 (the word has to be rejected)." << endl;
	cout << endl;

}

/*
 * Function to obtain the information about the Alphabet size from the user.
 * The alphabet size has to be between 1 and 9.
 */
int get_AlphabetSize() {

	int i;
	do{
		cout << "Please enter the alphabet size: ";
		cin >> i;
		if (i>=0 && i<=9)
			return i;
		else
			cout<<"Wrong input. Please enter value between 0 and 9."<<endl;
		}
	while(true);
}

/*
 * Function to obtain the samples from the user.
 * The input sample obtained as a <code>string</code> and is converted
 * to an array of integers containing the alphabets.
 */
list<int> get_Samples(int alphabetsize) {

	list<int> word;
	string c;
	bool ok = true;
	do {
		ok = true;

		cout << "Enter a Word: ";
		cin >> c;

		unsigned int i = 0;
		for (i = 0; i < c.length(); i++) {
			if (c.at(i) < '0' || c.at(i) > ('0' + alphabetsize - 1)) {
				cout << "Found illegal character " << c.at(i) << endl;
				ok = false;
				word.clear();
				break;
			}

			word.push_back(c.at(i) - '0');
		}

	} while (!ok);

	return word;
}
/*
 * Function to retrieve the classification of the sample from the user.
 * The user enters '1' to classify the sample as an accepted word and
 * enters '0' to classify it as a rejected word.
 */
bool get_Classification() {
	int wc;
	do {
		cout << "Enter the Classification (0/1): ";
		cin >> wc;
		if (wc == 0)
			return false;
		else if (wc == 1)
			return true;
		else
			cout << "wrong input, enter '1' to accept and '0' to reject";
	} while (true);
}

string enough_Samples()
{
	string input;
	do{
	cout<<"Do you want to enter more samples (y/n)? ";
	cin>>input;
	if (input == "y" || input == "n") return input;
	else cout<<"Wrong input"<<endl;
	}while(true);

}
/*
 * The main method
 */
int main(int argc, char**argv) {
	
	// Print short help message
	print_Help();

	// An <code>Integer</code> to store the Alphabet Size.
	int alphabetsize;

	//A <code>string</code> to store information whether user wants to add more samples.
	string input = "y";

	//A List of Integers which is used to store the sample the sample words.
	list<int> words;

	//A boolean to store the classficiation of the sample word.
	bool classification;

	//Create new knowledgebase. In this case, we choose bool as type for the knowledgebase.
	knowledgebase<bool> base;

	//Information about the size of the alphabet is obtained from the user.
	alphabetsize = get_AlphabetSize();
	
	/*
	 * The loop iterates until the user responds "y" (implying "yes") or "n" (implying "no)
	 * when asked whether more samples are to be added.
	 * When user is ready to add a sample, the word and its classification are
	 * obtained which is consequently added to the Knowledgebase.
	 */
	while (input == "y") {
		words = get_Samples(alphabetsize);
		classification = get_Classification();
		base.add_knowledge(words, classification);
		input = enough_Samples();
	}

	/*
	 * Create learning algorithm (RPNI L*) without a logger (2nd argument is NULL)
	 * and alphabet size alphabet_size
	 */
	RPNI<bool> algorithm(&base, NULL, alphabetsize);

	/*
	 * The method "advance" of the learning algorithm computes the conjecture which
	 * is stored in <code>cj</code>
	 */
	conjecture *cj = algorithm.advance();

	//The conjecture in the ".dot" code is presented to the user.
	cout << endl << "Result:" << endl << cj->visualize() << endl;

	// Delete
	delete cj;
}

