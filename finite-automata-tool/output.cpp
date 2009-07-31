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

#include <iostream>

#include "fat.h"

#include <libalf/basic_string.h>

bool write_output(finite_automaton *& automaton, output out, string sampletype)
{
	basic_string<int32_t> serial;

	switch(out) {
		case output_serial:
			serial = automaton->serialize();
			if(!basic_string_to_fd(serial, STDOUT_FILENO)) {
				cerr << "failed to write serial\n";
				return false;
			}
			return true;
		case output_dotfile:
			cout << automaton->generate_dotfile();
			return true;
		case output_human_readable:
//			cout << automaton.write();
			break;
		case output_sample:
		case output_sample_text:
			knowledgebase<bool> base;

			// FIXME: generate knowledgebase from automaton
			

			if(out == output_sample) {
				
			} else {
				if(out == output_sample_text) {
					cout << base.generate_dotfile();
				}
			}

			return true;
	}

	return false;
}

