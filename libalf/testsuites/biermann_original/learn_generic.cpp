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
 * (c) 2010 David R. Piegdon
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <ostream>
#include <fstream>
#include <list>
#include <string>

#include <libalf/alf.h>
#include <libalf/algorithm_biermann_original.h>
#include <liblangen/prng.h>

using namespace std;
using namespace libalf;
using namespace liblangen;


void string2file(string filename, string content)
{{{
	ofstream file;
	file.open(filename.c_str());
	file << content;
	file.close();
}}};

void generate_random_knowledgebase(knowledgebase<int> & kb, int input_alphabet_size, int output_alphabet_size, int entry_count)
{{{
	int avg_len = 2. * log(entry_count) / log(input_alphabet_size);
	int var_len = avg_len / 3;

	int fail = 0;
	for(int i = 0; i < entry_count; ++i) {
		// generate random word
		int len = avg_len + prng::random_int(var_len*2) - var_len;

		list<int> input_word;
		int answer;

		for(int l = 0; l < len; ++l)
			input_word.push_back(prng::random_int(input_alphabet_size));
		answer = prng::random_int(output_alphabet_size);
#ifdef DEBUG
		cout << "added " << word2string(input_word) << " as " << answer << ".\n";
#endif

		if(!kb.node_exists(input_word)) {
			kb.add_knowledge(input_word, answer);
		} else {
#ifdef DEBUG
			cout << "collision\n";
#endif
			--i; // word already existed in knowledgebase
			++fail;
		}
		if(fail > entry_count) {
			cerr << "(info) collision break.\n";
			break;
		}
	}
}}};

int main(int argc, char**argv)
{
	prng::seed_prng();
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<int> kb;

	int nondeterminism = 3;
	int sample_count = 10;

	int input_alphabet_size = 4;
	int output_alphabet_size = 3;

	generate_random_knowledgebase(kb, input_alphabet_size, output_alphabet_size, sample_count);

	string2file("knowledgebase", kb.to_string());

	original_biermann<int> orgy(&kb, &log, input_alphabet_size, nondeterminism);
	conjecture *cj;

	if(!orgy.conjecture_ready())
		log(LOGGER_WARN, "biermann says that no conjecture is ready! trying anyway...\n");

	if( NULL == (cj = orgy.advance()) ) {
		log(LOGGER_ERROR, "advance() returned false!\n");
	} else {
		log(LOGGER_INFO, "hypothesis:\n");
		cout << cj->visualize();
		string2file("hypothesis", cj->visualize());
	}

	return 0;
};

