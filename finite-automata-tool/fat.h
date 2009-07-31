/* $Id$
 * vim: fdm=marker
 *
 * Finite Automata Tools
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <string>
#include <amore++/finite_automaton.h>
#include <libalf/knowledgebase.h>

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
	trans_mdfa,
	trans_rfsa,
	trans_universal
};

enum output {
	output_serial,
	output_dotfile,
	output_human_readable,
	output_sample,
	output_sample_text
};

bool get_input(finite_automaton *& automaton, input in, string gentype);

bool do_transformation(finite_automaton *& automaton, transformation trans);

bool write_output(finite_automaton *& automaton, output out, string sampletype);

