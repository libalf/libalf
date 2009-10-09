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
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
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

