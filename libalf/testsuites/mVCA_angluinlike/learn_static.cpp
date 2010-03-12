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

#include <libalf/alf.h>
#include <libalf/knowledgebase.h>
#include <libalf/algorithm_mVCA_angluinlike.h>

using namespace std;
using namespace libalf;

ostream_logger my_logger(&cout, LOGGER_DEBUG);

int main(int argc, char**argv)
{
	knowledgebase<bool> kb;
	mVCA_angluinlike<bool> table(&kb, &my_logger, 1);
	map<int, int> alphabet_pushdown_directions;

	alphabet_pushdown_directions[0] = 0;
	table.indicate_pushdown_alphabet_directions(alphabet_pushdown_directions);


	// real work with algorithm:
	table.advance();


	// debugging of table:
	table.print(cout);

	basic_string<int32_t> ser;
	ser = table.serialize();
	cout << "\n\n{ ";
	print_basic_string_2hl(ser, cout);
	cout << " }\n\n";

	mVCA_angluinlike<bool> table2(&kb, &my_logger, 1);
	serial_stretch s(ser);
	if(!table2.deserialize(s))
		cerr << "failed to deser!\n";

	return 0;
}

