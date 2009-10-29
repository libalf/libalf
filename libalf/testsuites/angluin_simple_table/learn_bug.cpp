/* $Id: learn_static.cpp 1000 2009-10-13 10:09:11Z davidpiegdon $
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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include <libalf/alf.h>
#include <libalf/algorithm_angluin.h>

#include <amore++/nondeterministic_finite_automaton.h>

#include "amore_alf_glue.h"

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;
using namespace amore;

int algorithm_data[] = {
	1, 6, 11, 0, 0, 1, 3, 31, 2, 5, 3, 59, 2, 2, 3, 59, 3, 2, 5, 3, 108, 3,
	5, 2, 3, 108, 3, 1, 2, 3, 108, 3, 4, 5, 3, 108, 4, 0, 1, 2, 3, 207, 4,
	4, 5, 2, 3, 207, 4, 4, 2, 5, 3, 207, 12, 15, 0, 1, 0, 11, 0, 0, 0, 0, 0,
	0, 0, 0, 2, 0, 0, 16, 1, -42, 0, 1, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	25, 10, 0, 1, 2, 3, 4, 5, 2, 4, 3, 5, 5, 0, 11, 2, 0, 0, 0, 0, 0, 0, 0,
	2, 0, 0, 24, 9, 0, 1, 2, 3, 4, 5, 2, 4, 5, 5, 0, 11, 0, 2, 0, 0, 0, 0,
	0, 2, 0, 0, 0, 23, 8, 0, 1, 2, 3, 4, 5, 2, 4, 5, 0, 11, 0, 0, 2, 0, 0,
	0, 0, 0, 0, 0, 0, 22, 7, 0, 1, 2, 3, 4, 5, 4, 5, 0, 11, 0, 0, 0, 0, 2,
	2, 0, 0, 0, 0, 0, 21, 6, 0, 1, 2, 3, 4, 5, 5, 0, 11, 0, 0, 0, 2, 0, 0,
	0, 0, 0, 2, 2, 20, 5, 0, 1, 2, 3, 4, 5, 0, 11, 0, 0, 0, 0, 0, 2, 0, 0,
	0, 0, 0, 19, 4, 0, 1, 2, 3, 5, 0, 11, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,
	18, 3, 0, 1, 2, 5, 0, 11, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 2, 0, 1,
	5, 0, 11, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 16, 1, 0, 5, 1, 11, 0, 0, 0,
	0, 0, 0, 2, 0, 0, 0, 0, 19, 16, 1, 2, 0, 1, 11, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 16, 1, 4, 0, 1, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 11, 0,
	1, 2, 3, 4, 5, 2, 4, 3, 5, 0, 0, 5, 11, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0,
	26, 11, 0, 1, 2, 3, 4, 5, 2, 4, 3, 5, 2, 0, 5, 11, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 26, 11, 0, 1, 2, 3, 4, 5, 2, 4, 3, 5, 4, 0, 5, 11, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 25, 10, 0, 1, 2, 3, 4, 5, 2, 4, 5, 0, 0, 5, 11,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 10, 0, 1, 2, 3, 4, 5, 2, 4, 5, 4,
	0, 5, 11, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 24, 9, 0, 1, 2, 3, 4, 5, 2,
	4, 3, 0, 5, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 8, 0, 1, 2, 3, 4,
	5, 4, 5, 0, 5, 11, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 2, 22, 7, 0, 1, 2, 3,
	4, 5, 0, 0, 5, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 7, 0, 1, 2, 3,
	4, 5, 2, 0, 5, 11, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 21, 6, 0, 1, 2, 3,
	2, 4, 0, 5, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 5, 0, 1, 2, 3, 0,
	0, 5, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 5, 0, 1, 2, 3, 2, 0, 5,
	11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 4, 0, 1, 0, 2, 0, 5, 11, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 4, 0, 1, 2, 4, 0, 5, 11, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 18, 3, 0, 1, 0, 0, 5, 11, 0, 0, 0, 0, 0, 0, 2, 0, 0,
	0, 0, 18, 3, 0, 1, 4, 0, 5, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 2,
	0, 2, 0, 5, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int normalizer_data[] = {
	1, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 1, 0, 0, 1, 6, 0, 0, 1, 1, 0, 0, 1
};

int knowledgebase_data[] = {
	357, -1, 1, 2, 0, 6, 0, 2, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1,
	3, 217, 2, 0, 0, 1, 13, 2, 0, 6, 0, 28, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1,
	2, 0, 0, 1, 3, 264, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 197, 2, 2, 0, 2,
	26, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 258, 2, 0, 0, 1, 0,
	0, 1, 2, 0, 0, 1, 3, 190, 2, 0, 0, 2, 0, 0, 2, 3, 100, 2, 0, 0, 5, 0, 0,
	1, 3, 188, 2, 0, 0, 3, 54, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1,
	3, 260, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 259, 2, 0, 0, 3, 191, 2, 0,
	0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 189, 2, 0, 0, 3, 99, 2, 0, 0, 3, 56, 2, 0,
	0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 266, 2, 0, 0, 5, 0, 0, 2, 2,
	0, 0, 1, 3, 265, 2, 0, 0, 3, 198, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3,
	196, 2, 0, 0, 3, 103, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 132, 2, 0, 0,
	2, 12, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 216, 2, 0, 0, 1,
	0, 0, 1, 2, 0, 0, 1, 3, 130, 2, 0, 0, 2, 0, 0, 2, 3, 69, 2, 0, 0, 5, 0,
	0, 1, 3, 128, 2, 0, 0, 3, 11, 2, 2, 6, 0, 24, 2, 0, 6, 0, 0, 0, 1, 1, 0,
	0, 1, 2, 0, 0, 1, 3, 254, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 183, 2, 0,
	0, 2, 0, 0, 2, 3, 96, 2, 0, 0, 5, 0, 0, 1, 3, 181, 2, 0, 0, 3, 52, 2, 0,
	0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 256, 2, 0, 0, 5, 0, 0, 2, 2,
	0, 0, 1, 3, 255, 2, 0, 0, 3, 184, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3,
	182, 2, 0, 0, 3, 95, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 127, 2, 0, 0,
	2, 25, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 257, 2, 0, 0, 1,
	0, 0, 1, 2, 0, 0, 1, 3, 187, 2, 0, 0, 2, 0, 0, 2, 3, 98, 2, 0, 0, 5, 0,
	0, 1, 3, 185, 2, 0, 0, 3, 53, 2, 0, 0, 4, 23, 2, 0, 6, 0, 0, 0, 1, 1, 0,
	0, 1, 2, 0, 0, 1, 3, 251, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 179, 2, 0,
	0, 2, 0, 0, 2, 3, 94, 2, 0, 0, 5, 0, 0, 1, 3, 177, 2, 0, 0, 3, 51, 2, 0,
	0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 253, 2, 0, 0, 5, 0, 0, 2, 2,
	0, 0, 1, 3, 252, 2, 0, 0, 3, 180, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3,
	178, 2, 0, 0, 3, 93, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 186, 2, 0, 0,
	3, 97, 2, 0, 0, 3, 37, 2, 0, 0, 4, 10, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1,
	2, 0, 0, 1, 3, 213, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 124, 2, 0, 0, 2,
	0, 0, 2, 3, 66, 2, 0, 0, 5, 0, 0, 1, 3, 123, 2, 0, 0, 3, 36, 2, 0, 0, 4,
	0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 215, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0,
	1, 3, 214, 2, 0, 0, 3, 125, 2, 0, 0, 5, 9, 2, 0, 6, 0, 21, 2, 0, 6, 0,
	0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 247, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0,
	1, 3, 172, 2, 0, 0, 2, 0, 0, 2, 3, 90, 2, 0, 0, 5, 0, 0, 1, 3, 170, 2,
	0, 0, 3, 49, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 249, 2, 0,
	0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 248, 2, 0, 0, 3, 173, 2, 0, 0, 5, 0, 0, 2,
	2, 0, 0, 1, 3, 171, 2, 0, 0, 3, 89, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3,
	122, 2, 0, 0, 2, 22, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3,
	250, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 176, 2, 0, 0, 2, 0, 0, 2, 3,
	92, 2, 0, 0, 5, 0, 0, 1, 3, 174, 2, 0, 0, 3, 50, 2, 2, 0, 4, 7, 2, 0, 6,
	0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 207, 2, 0, 0, 1, 0, 0, 1, 2, 0,
	0, 1, 3, 116, 2, 0, 0, 2, 0, 0, 2, 3, 63, 2, 0, 0, 5, 0, 0, 1, 3, 115,
	2, 0, 0, 3, 19, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 241, 2,
	0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 164, 2, 0, 0, 2, 0, 0, 2, 3, 86, 2, 0,
	0, 5, 0, 0, 1, 3, 163, 2, 0, 0, 3, 47, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1,
	5, 0, 0, 1, 3, 243, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 242, 2, 0, 0, 3,
	165, 2, 0, 0, 5, 5, 2, 2, 6, 0, 14, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2,
	0, 0, 1, 3, 226, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 145, 2, 2, 0, 2, 0,
	0, 2, 3, 77, 2, 0, 0, 5, 0, 0, 1, 3, 143, 2, 0, 0, 3, 42, 2, 0, 0, 4, 0,
	0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 228, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1,
	3, 227, 2, 0, 0, 3, 146, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 144, 2, 0,
	0, 3, 76, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 111, 2, 0, 0, 2, 15, 2, 0,
	6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 229, 2, 0, 0, 1, 0, 0, 1, 2,
	0, 0, 1, 3, 149, 2, 0, 0, 2, 0, 0, 2, 3, 79, 2, 0, 0, 5, 0, 0, 1, 3,
	147, 2, 0, 0, 3, 43, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3,
	231, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 230, 2, 0, 0, 3, 150, 2, 0, 0,
	5, 0, 0, 2, 2, 0, 0, 1, 3, 148, 2, 0, 0, 3, 78, 2, 0, 0, 3, 32, 2, 0, 0,
	4, 16, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 232, 2, 0, 0, 1,
	0, 0, 1, 2, 0, 0, 1, 3, 153, 2, 0, 0, 2, 0, 0, 2, 3, 81, 2, 0, 0, 5, 0,
	0, 1, 3, 151, 2, 0, 0, 3, 44, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0,
	1, 3, 234, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 233, 2, 0, 0, 3, 154, 2,
	0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 152, 2, 0, 0, 3, 80, 2, 0, 0, 5, 0, 0,
	2, 2, 0, 0, 1, 3, 110, 2, 0, 0, 3, 60, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1,
	5, 0, 0, 1, 3, 209, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 208, 2, 0, 0, 3,
	117, 2, 0, 0, 5, 6, 2, 0, 6, 0, 17, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2,
	0, 0, 1, 3, 235, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 157, 2, 0, 0, 2, 0,
	0, 2, 3, 83, 2, 0, 0, 5, 0, 0, 1, 3, 155, 2, 0, 0, 3, 45, 2, 0, 0, 4, 0,
	0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 237, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1,
	3, 236, 2, 0, 0, 3, 158, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 156, 2, 0,
	0, 3, 82, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 114, 2, 0, 0, 2, 0, 0, 2,
	3, 62, 2, 0, 0, 5, 0, 0, 1, 3, 112, 2, 0, 0, 3, 33, 2, 2, 0, 4, 18, 2,
	0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 238, 2, 0, 0, 1, 0, 0, 1,
	2, 0, 0, 1, 3, 161, 2, 0, 0, 2, 0, 0, 2, 3, 85, 2, 0, 0, 5, 0, 0, 1, 3,
	159, 2, 0, 0, 3, 46, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3,
	240, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 239, 2, 0, 0, 3, 162, 2, 0, 0,
	5, 0, 0, 2, 2, 0, 0, 1, 3, 160, 2, 0, 0, 3, 84, 2, 2, 0, 5, 0, 0, 2, 2,
	0, 0, 1, 3, 113, 2, 0, 0, 3, 61, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3,
	175, 2, 0, 0, 3, 91, 2, 0, 0, 3, 35, 2, 0, 0, 4, 8, 2, 0, 6, 0, 0, 0, 1,
	1, 0, 0, 1, 2, 0, 0, 1, 3, 210, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 119,
	2, 0, 0, 2, 0, 0, 2, 3, 64, 2, 0, 0, 5, 0, 0, 1, 3, 118, 2, 2, 0, 3, 34,
	2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 212, 2, 0, 0, 5, 0, 0,
	2, 2, 0, 0, 1, 3, 211, 2, 0, 0, 3, 120, 2, 0, 0, 5, 20, 2, 0, 6, 0, 0,
	0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 244, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1,
	3, 168, 2, 0, 0, 2, 0, 0, 2, 3, 88, 2, 2, 0, 5, 0, 0, 1, 3, 166, 2, 0,
	0, 3, 48, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 246, 2, 2, 0,
	5, 0, 0, 2, 2, 0, 0, 1, 3, 245, 2, 2, 0, 3, 169, 2, 0, 0, 5, 0, 0, 2, 2,
	0, 0, 1, 3, 167, 2, 0, 0, 3, 87, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3,
	121, 2, 0, 0, 3, 65, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 126, 2, 0, 0,
	3, 67, 2, 0, 0, 4, 27, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3,
	261, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 194, 2, 0, 0, 2, 0, 0, 2, 3,
	102, 2, 0, 0, 5, 0, 0, 1, 3, 192, 2, 0, 0, 3, 55, 2, 0, 0, 4, 0, 0, 2,
	2, 0, 0, 1, 5, 0, 0, 1, 3, 263, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 262,
	2, 0, 0, 3, 195, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 193, 2, 0, 0, 3,
	101, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 129, 2, 0, 0, 3, 68, 2, 0, 0,
	3, 38, 2, 0, 0, 4, 29, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3,
	267, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 201, 2, 0, 0, 2, 0, 0, 2, 3,
	105, 2, 0, 0, 5, 0, 0, 1, 3, 199, 2, 0, 0, 3, 57, 2, 0, 0, 4, 0, 0, 2,
	2, 0, 0, 1, 5, 0, 0, 1, 3, 269, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 268,
	2, 0, 0, 3, 202, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 200, 2, 0, 0, 3,
	104, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 131, 2, 0, 0, 3, 70, 2, 0, 0,
	2, 30, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 270, 2, 0, 0, 1,
	0, 0, 1, 2, 0, 0, 1, 3, 205, 2, 0, 0, 2, 0, 0, 2, 3, 107, 2, 0, 0, 5, 0,
	0, 1, 3, 203, 2, 0, 0, 3, 58, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0,
	1, 3, 272, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 271, 2, 0, 0, 3, 206, 2,
	0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 204, 2, 0, 0, 3, 106, 2, 0, 0, 3, 39,
	2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0, 0, 1, 3, 219, 2, 0, 0, 5, 0, 0,
	2, 2, 0, 0, 1, 3, 218, 2, 0, 0, 3, 134, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1,
	3, 133, 2, 0, 0, 3, 71, 2, 0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 109, 2, 0,
	0, 2, 3, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 220, 2, 0, 0,
	1, 0, 0, 1, 2, 0, 0, 1, 3, 137, 2, 0, 0, 2, 0, 0, 2, 3, 73, 2, 0, 0, 5,
	0, 0, 1, 3, 135, 2, 0, 0, 3, 40, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1, 5, 0,
	0, 1, 3, 222, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 221, 2, 0, 0, 3, 138,
	2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 136, 2, 0, 0, 3, 72, 2, 0, 0, 3, 31,
	2, 0, 0, 4, 4, 2, 0, 6, 0, 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 1, 3, 223, 2,
	0, 0, 1, 0, 0, 1, 2, 0, 0, 1, 3, 141, 2, 0, 0, 2, 0, 0, 2, 3, 75, 2, 0,
	0, 5, 0, 0, 1, 3, 139, 2, 0, 0, 3, 41, 2, 0, 0, 4, 0, 0, 2, 2, 0, 0, 1,
	5, 0, 0, 1, 3, 225, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 224, 2, 0, 0, 3,
	142, 2, 0, 0, 5, 0, 0, 2, 2, 0, 0, 1, 3, 140, 2, 0, 0, 3, 74, 2, 0, 0,
	5, 0, 0, 2, 2, 0, 0, 1, 3, 108, 2, 0, 0, 3, 59, 2, 0, 0
};

basic_string<int32_t> algorithm_serial, normalizer_serial, knowledgebase_serial;

void correct_byte_order()
{
	unsigned int i;

	algorithm_serial.push_back(0);
	for(i = 0; i < sizeof(algorithm_data)/sizeof(int); i++)
		algorithm_serial.push_back(htonl(algorithm_data[i]));
	algorithm_serial[0] = htonl(algorithm_serial.size() - 1);

	normalizer_serial.push_back(0);
	for(i = 0; i < sizeof(normalizer_data)/sizeof(int); i++)
		normalizer_serial.push_back(htonl(normalizer_data[i]));
	normalizer_serial[0] = htonl(normalizer_serial.size() - 1);

	knowledgebase_serial.push_back(0);
	for(i = 0; i < sizeof(knowledgebase_data)/sizeof(int); i++)
		knowledgebase_serial.push_back(htonl(knowledgebase_data[i]));
	knowledgebase_serial[0] = htonl(knowledgebase_serial.size() - 1);
};


int main(int argc, char**argv)
{
	statistics stats;

	finite_automaton *nfa;
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<ANSWERTYPE> knowledge;

	char filename[128];
	ofstream file;

	int iteration;
	bool success = false;

	int alphabet_size;
	unsigned int hypothesis_state_count = 0;

	alphabet_size = 6;

	// create angluin_simple_table and teach it the automaton
	angluin_simple_table<ANSWERTYPE> ot(&knowledge, &log, alphabet_size);

	correct_byte_order();
	basic_string<int32_t>::iterator seri;

	seri = knowledgebase_serial.begin();
	if(!knowledge.deserialize(seri, knowledgebase_serial.end())) {
		cerr << "failed to deser KB.\n";
		exit(1);
	};
	if(seri != knowledgebase_serial.end()) {
		cerr << "garbage at end of KB.\n";
		exit(1);
	};

	seri = algorithm_serial.begin();
	if(!ot.deserialize(seri, algorithm_serial.end())) {
		cerr << "failed to deser ALG.\n";
		exit(1);
	};
	if(seri != algorithm_serial.end()) {
		cerr << "garbage at end of ALG.\n";
		exit(1);
	};


	cout << knowledge.tostring();
	ot.print(cout);
//	exit(0);


	finite_automaton * hypothesis = NULL;

	for(iteration = 1; iteration <= 100; iteration++) {
		int c = 'a';
		conjecture * cj;

		while( NULL == (cj = ot.advance()) ) {
			// resolve missing knowledge:

			snprintf(filename, 128, "knowledgebase%02d%c.dot", iteration, c);
			file.open(filename); file << knowledge.generate_dotfile(); file.close();

			// create query-tree
			knowledgebase<ANSWERTYPE> * query;
			query = knowledge.create_query_tree();

			snprintf(filename, 128, "knowledgebase%02d%c-q.dot", iteration, c);
			file.open(filename); file << query->generate_dotfile(); file.close();

			// answer queries
			stats.queries.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*nfa, *query);

			snprintf(filename, 128, "knowledgebase%02d%c-r.dot", iteration, c);
			file.open(filename); file << query->generate_dotfile(); file.close();

			// merge answers into knowledgebase
			knowledge.merge_knowledgebase(*query);
			delete query;
			c++;
		}

		simple_automaton * ba = dynamic_cast<simple_automaton*>(cj);
		if(hypothesis)
			delete hypothesis;
		hypothesis = construct_amore_automaton(ba->is_deterministic, ba->alphabet_size, ba->state_count, ba->initial, ba->final, ba->transitions);
		delete cj;
		if(!hypothesis) {
			printf("generation of hypothesis failed!\n");
			return -1;
		}

		{{{ /* dump/serialize table */
			basic_string<int32_t> serialized;
			basic_string<int32_t>::iterator it;

			snprintf(filename, 128, "table%02d.text.angluin", iteration);
			file.open(filename); ot.print(file); file.close();

			/*
			serialized = ot.serialize();

			snprintf(filename, 128, "table%02d.serialized.angluin", iteration);
			file.open(filename);

			for(it = serialized.begin(); it != serialized.end(); it++) {
				file << ntohl(*it);
				file << ";";
			}

			file.close();
			*/
		}}}

		snprintf(filename, 128, "hypothesis%02d.dot", iteration);
		file.open(filename); file << hypothesis->generate_dotfile(); file.close();

		printf("hypothesis %02d state count %02d\n", iteration, hypothesis->get_state_count());
		if(hypothesis_state_count >= hypothesis->get_state_count()) {
			log(LOGGER_ERROR, "STATE COUNT DID NOT INCREASE\n");
			getchar();
		}
		hypothesis_state_count = hypothesis->get_state_count();

		// once an automaton is generated, test for equivalence with oracle_automaton
		// if this test is ok, all worked well

		list<int> counterexample;
		stats.queries.equivalence++;
		if(amore_alf_glue::automaton_equivalence_query(*nfa, *hypothesis, counterexample)) {
			// equivalent
			cout << "success.\n";
			success = true;
			break;
		}

		snprintf(filename, 128, "counterexample%02d.angluin", iteration);
		file.open(filename);
		print_word(file, counterexample);
		ot.add_counterexample(counterexample);
		file.close();
	}

	iteration++;
	snprintf(filename, 128, "knowledgebase%02d-final.dot", iteration);
	file.open(filename);
	file << knowledge.generate_dotfile();
	file.close();

	stats.memory = ot.get_memory_statistics();
	stats.queries.membership = knowledge.count_resolved_queries();


	cout << "\nrequired membership queries: " << stats.queries.membership << "\n";
	cout << "required uniq membership queries: " << stats.queries.uniq_membership << "\n";
	cout << "required equivalence queries: " << stats.queries.equivalence << "\n";
	cout << "sizes: bytes: " << stats.memory.bytes
	     << ", members: " << stats.memory.members
	     << ", words: " << stats.memory.words << "\n";
	cout << "upper table rows: " << stats.memory.upper_table
	     << ", lower table rows: " << stats.memory.lower_table
	     << ", columns: " << stats.memory.columns << "\n";
	cout << "minimal state count: " << hypothesis->get_state_count() << "\n";

	delete hypothesis;
	delete nfa;

	if(success)
		return 0;
	else
		return 2;
}

