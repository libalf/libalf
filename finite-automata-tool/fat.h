/* $Id$
 * vim: fdm=marker
 *
 * This file is part of Finite Automata Tools (FAT).
 *
 * FAT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FAT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FAT.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <string>
#include <libalf/knowledgebase.h>
#include <amore++/finite_automaton.h>

using namespace std;
using namespace amore;
using namespace libalf;

enum input {
	input_serial,
	input_human_readable,
	input_generate
};

enum transformation {
	trans_none,
	trans_minimize,
	trans_determinize,
	trans_co_determinize,
	trans_reverse,
	trans_mdfa,
	trans_rfsa
};

enum output {
	output_serial,
	output_dotfile,
	output_human_readable,
	output_sample,
	output_sample_text
};

bool get_input(amore::finite_automaton *& automaton, input in, string gentype);

bool do_transformation(amore::finite_automaton *& automaton, transformation trans);

bool write_output(amore::finite_automaton *& automaton, output out, string sampletype);

