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

#ifndef __libalf_stamina_h__
# define __libalf_stamina_h__

// Standard C++ includes
#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>

// libalf includes
#include <libalf/alf.h>

using namespace std;
using namespace libalf;

/*
 * Creates a string representation of a libalf word.
 */
string print(list<int> &word)
// FIXME: should be replaced with libalf::word2string().
{{{
	stringstream str;

	str << "[";
	unsigned int i=0;
	list<int>::iterator it;

	for(it = word.begin(); it != word.end(); it++) {
		str << *it;
		if(i < word.size() - 1)
			str << ", ";
		i++;
	}

	str << "]";

	return str.str();
}}}

/*
 * Simulates the run of a simple automaton on a given input.
 */
bool run(simple_automaton *automaton, list<int> &input)
// FIXME: will be incorporated into future version of conjecture
{{{

	// Check whether the given automaton is deterministic
	if(!automaton->is_valid()) {
		cerr << "Error in run(...): provided automaton is not valid!" << endl;
		return false;
	}

	// Get initial state
	set<int> *current = new set<int>(automaton->initial);

	// Simulate run letter by letter
	list<int>::iterator input_it;
	for(input_it = input.begin(); input_it != input.end(); input_it++) {

		set<int> *next = new set<int>();

		// Iterate over all current source states
		set<int>::iterator current_it;
		for(current_it = current->begin(); current_it != current->end(); current_it++) {

			pair<int, int> source(*current_it, *input_it);

			// Iterate over all outgoing transitions
			multimap<pair<int, int>, int>::iterator dest_it;
			for (dest_it = automaton->transitions.equal_range(source).first; dest_it != automaton->transitions.equal_range(source).second; dest_it++)
				next->insert(dest_it->second);

		}

		delete current;
		current = next;
	}

	// Compute intersection of reached states and final states
	bool accept;
	accept = false;
	set<int>::iterator reached_it, final_it;

	// For each reached state ...
	for(reached_it = current->begin(); reached_it != current->end(); reached_it++) {

		// ... check whether it is a final state
		for(final_it = automaton->final.begin(); final_it != automaton->final.end(); final_it++) {
			if(*reached_it == *final_it) {
				accept = true;
				break;
			}
		}

		if(accept)
			break;
	}

	// Memory cleanup
	delete current;

	return accept;
}}}

/*
 * This class represents a stamina training and test set.
 */
class stamina_set {

	private:

		map<list<int>, bool> training_set;		// The training set
		vector<list<int> > test_set;			// The test set
		int alphabet_size;						// The size of the used alphabet

		/*
		 * Creates a new instance with the given training set, the test set, and
		 * the alphabet size;
		 */
		stamina_set(map<list<int>, bool> &training_set, vector<list<int> > &test_set, int alphabet_size)
		{{{

			// Store arguments
			this->training_set = training_set;
			this->test_set = test_set;
			this->alphabet_size = alphabet_size;
		}}}

		/*
		 * Reads a training set from a text file and stores it in the map training_set
		 * given as argument. This map maps a libalf word to its classification.
		 * The size of the alphabet is the greatest integer encountered during
		 * parsing and stored in the alphabet_size argument.
		 *
		 * This method returns true if the file could be parsed successfully and
		 * otherwise false.
		 */
		static bool read_training_set(const char *filename, map<list<int>, bool> &training_set, int *alphabet_size)
		{{{
			// Maximal encountered letter
			int max = 0;

			// Open file
			ifstream file;
			file.open (filename);

			// Read content line by line
			string line;

			if (file.is_open()) {

				// Process each line
				while (! file.eof() ) {

					// Read line
					getline (file, line);

					// Process line
					vector<string> tokens = split(line);

					vector<string>::iterator it;
					it = tokens.begin();

					// Check error
					if(it == tokens.end())
						continue;

					// Get classification, i.e. first token
					bool classification;
					if(*it == "+")
						classification = true;
					else if(*it == "-")
						classification = false;
					else
						continue;

					// Extract characters
					list<int> word;
					for ( it++; it != tokens.end(); it++) {

						// Parse and add int to word
						int symbol = atoi (it->c_str());
						word.push_back(symbol);

						// Adjust alphabet size if necessary
						if(symbol > max)
							max = symbol;
					}

					// Store line
					training_set[word] = classification;
				}

				// Close file
				file.close();

				// Set alphabet size
				*alphabet_size = max + 1;

			} else {

				// Could not open the given file!
				stringstream err;
				err << "Could not open file '" << filename << "'";
				perror (err.str().c_str());
				return false;
			}

			// Return
			return true;
		}}}

		/*
		 * Reads a test set from a text file and stores it in the vector test_set
		 * given as argument.
		 *
		 * This method returns true if the file could be parsed successfully.
		 *
		 */
		static bool read_test_set(const char *filename, vector<list<int> > &test_set)
		{{{
			// Open file
			ifstream file;
			file.open (filename);

			// Read content line by line
			string line;

			if (file.is_open()) {

				// Process each line
				while (! file.eof() ) {

					// Read line
					getline (file, line);

					// Process line
					vector<string> tokens = split(line);

					vector<string>::iterator it;
					it = tokens.begin();

					// Check for erroneous line
					if(it == tokens.end())
						continue;

					// Extract characters, skip first character (which should be a question mark)
					list<int> word;
					for ( it++; it != tokens.end(); it++) {

						// Parse and add int
						int symbol = atoi (it->c_str());
						word.push_back(symbol);
					}

					// Store word
					test_set.push_back(word);

				}

				// Close file
				file.close();

			}  else {

				// Could not open the given file!
				stringstream err;
				err << "Could not open file '" << filename << "'";
				perror (err.str().c_str());
				return false;
			}

			// Return success
			return true;
		}}}

		/*
		 * Splits a string with respect to the given delimiters. If no delimiters are
		 * given, a blank is used.
		 */
		static vector<string> split(const string& str, const string& delimiters = " ")
		{{{
			vector<string> tokens;

			// Skip delimiters at beginning.
			string::size_type lastPos = str.find_first_not_of(delimiters, 0);
			// Find first "non-delimiter".
			string::size_type pos     = str.find_first_of(delimiters, lastPos);

			while (string::npos != pos || string::npos != lastPos) {

				// Found a token, add it to the vector.
				tokens.push_back(str.substr(lastPos, pos - lastPos));
				// Skip delimiters.  Note the "not_of"
				lastPos = str.find_first_not_of(delimiters, pos);
				// Find next "non-delimiter"
				pos = str.find_first_of(delimiters, lastPos);
			}

			return tokens;
		}}}

	public:

		/*
		 * Creates a new stamina set, where the training set is read from the file
		 * specified as first argument and the test set is read from the second. The
		 * alphabet size is the biggest integer encountered during parsing the
		 * training set.
		 */
		static stamina_set *create_stamina_set(const char *training_file_name, const char *test_file_name)
		{{{
			// 1. Parse training set and alphabet size
			map<list<int>, bool> training_set;
			int *alphabet_size = new int();
			if(!read_training_set(training_file_name, training_set, alphabet_size))
				return NULL;

			// 2. Parse test set
			vector<list<int> > test_set;
			if(!read_test_set(test_file_name, test_set))
				return NULL;

			// Create stamina_set object
			stamina_set *set = new stamina_set(training_set, test_set, *alphabet_size);

			// Clean memory
			delete alphabet_size;

			// Return result
			return set;
		}}}

		/*
		 * Creates a new stamina set, the training and test sets are read from the
		 * files "<problem_key>_training.txt" and "<problem_key>_test.txt". The
		 * argument can be used to specify a folder etc.
		 *
		 * Also see the method
		 *    stamina_set *create_stamina_set(const char* training_file_name, const char * test_file_name)
		 */
		static stamina_set *create_stamina_set(int problem_key, const char *prefix = "")
		{{{
			// Prepare training set filename
			stringstream training_file;
			training_file << prefix << problem_key << "_training.txt";

			// Prepare test set filename
			stringstream test_file;
			test_file << prefix << problem_key << "_test.txt";

			// Return stamina set
			return create_stamina_set(training_file.str().data(), test_file.str().data());
		}}}

		/*
		 * Returns the training set.
		 */
		const map<list<int>, bool> & get_training_set()
		{ return training_set; };

		/*
		 * Returns the test set.
		 *
		 * Note that the classifications have to specified in the same order as in
		 * the vector returned by this method.
		 */
		const vector<list<int> > & get_test_set()
		{ return test_set; };

		/*
		 * Returns the size of the alphabet used in the training and test set.
		 *
		 * Note that the alphabet size is determined with respect to the training
		 * set only (which should be the same as for the test set).
		 */
		int get_alphabet_size()
		{ return alphabet_size; }
};

#endif // __libalf_stamina_h__

