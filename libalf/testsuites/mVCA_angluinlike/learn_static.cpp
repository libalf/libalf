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
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include <libmVCA/mVCA.h>
#include <libmVCA/deterministic_mVCA.h>
#include <libmVCA/pushdown.h>

#include <libalf/alf.h>
#include <libalf/knowledgebase.h>
#include <libalf/algorithm_mVCA_angluinlike.h>

#include <mVCA_alf_glue.h>

using namespace std;
using namespace libalf;
using namespace libmVCA;

ostream_logger my_logger(&cout, LOGGER_DEBUG);

pushdown_alphabet get_alphabet()
{{{
	pushdown_alphabet ret(3);
	ret.set_direction(0, DIR_UP);
	ret.set_direction(1, DIR_STAY);
	ret.set_direction(2, DIR_DOWN);
	return ret;
}}}

mVCA * get_aNbcN3()
{{{
	map<int, map<int, map<int, set<int> > > > transitions;

	pushdown_alphabet al = get_alphabet();

	set<int> final;
	final.insert(1);

	transitions[0][0][0].insert(0);

	transitions[1][0][0].insert(0);
	transitions[1][1][2].insert(1);

	transitions[2][0][0].insert(0);
	transitions[2][1][2].insert(1);

	transitions[3][0][0].insert(0);
	transitions[3][1][2].insert(1);
	transitions[3][0][1].insert(1);

	return construct_mVCA(/*state-count*/ 2, al, /*initial state*/ 0, final, /*m_bound*/ 3, transitions);
}}};


int main(int argc, char**argv)
{
	knowledgebase<bool> kb;
	mVCA_angluinlike<bool> table(&kb, &my_logger, 3);
	map<int, int> alphabet_pushdown_directions;

	alphabet_pushdown_directions[0] = 1;
	alphabet_pushdown_directions[1] = 0;
	alphabet_pushdown_directions[2] = -1;
	table.indicate_pushdown_alphabet_directions(alphabet_pushdown_directions);

	mVCA * teacher;
	teacher = get_aNbcN3();

	// real work with algorithm:
	while(1) {
		conjecture * cj;

		int count = 1;
		while(NULL == (cj = table.advance())) {
			count = mVCA_alf_glue::automaton_answer_knowledgebase(*teacher, kb);
			cout << "answered " << count << " membership queries.\n";
			if(count == 0) break;
		}
		if(count == 0) break;

		// FIXME: check conjecture
		
		delete cj;

	}


	// debugging of table:
	table.print(cout);

	basic_string<int32_t> ser;
	ser = table.serialize();
	cout << "\n\n{ ";
	print_basic_string_2hl(ser, cout);
	cout << " }\n";

	mVCA_angluinlike<bool> table2(&kb, &my_logger, 1);
	serial_stretch s(ser);
	if(!table2.deserialize(s))
		cerr << "failed to deser!\n";

	ser = table2.serialize();
	cout << "\n{ ";
	print_basic_string_2hl(ser, cout);
	cout << " }\n\n";

	generic_statistics stat;
	table.receive_generic_statistics(stat);
	cout << "statistics:\n\t";
	stat.print(cout);
	cout << "\n";

	return 0;
}

