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

#include <unistd.h>

#include "fat.h"

#include <libalf/basic_string.h>
#include <libalf/automaton.h>

bool generate_samples_rpni(finite_automaton * automaton, knowledgebase<bool> & base)
{
	do_transformation(automaton, trans_mdfa);
	
	return false;
}

bool generate_samples_delete2(finite_automaton * automaton, knowledgebase<bool> & base)
{
	do_transformation(automaton, trans_rfsa);
	
	return false;
}

bool generate_samples_biermann(finite_automaton * automaton, knowledgebase<bool> & base)
{
	do_transformation(automaton, trans_mdfa);
	
	return false;
}

bool generate_samples_random(finite_automaton * automaton, knowledgebase<bool> & base, int count)
{
	
	return false;
}

bool generate_samples_depth(finite_automaton * automaton, knowledgebase<bool> & base, int depth)
{
	
	return false;
}

bool generate_samples(finite_automaton * automaton, knowledgebase<bool> & base, string sampletype)
{{{
	size_t pos;
	string st;
	string para;
	// sampletype: <rpni|delete2|biermann|random|depth>[:<int>]

	pos = sampletype.find_first_of(':');
	if(pos != string::npos) {
		st = sampletype.substr(0, pos);
		para = sampletype.substr(pos+1, string::npos);
	} else {
		st = sampletype;
		para = "";
	}

	if(st == "rpni") {
		return generate_samples_rpni(automaton, base);
	} else {
		if(st == "delete2") {
			return generate_samples_delete2(automaton, base);
		} else {
			if(st == "biermann") {
				return generate_samples_biermann(automaton, base);
			} else {
				if(st == "random") {
					int count = atoi(para.c_str());
					if(count <= 0) {
						cerr << "invalid parameter for random sample-set\n";
						return false;
					}
					return generate_samples_random(automaton, base, count);
				} else {
					if(st == "depth") {
						int depth = atoi(para.c_str());
						if(depth <= 0) {
							cerr << "invalid parameter for random sample-set\n";
							return false;
						}
						return generate_samples_depth(automaton, base, depth);
					} else {
						cerr << "invalid sample-set option\n";
						return false;
					}
				}
			}
		}
	}

	return false;
}}}

bool write_output(finite_automaton *& automaton, output out, string sampletype)
{{{
	basic_string<int32_t> serial;
	basic_string<int32_t>::iterator si;

	bool f_is_dfa;
	int f_alphabet_size;
	int f_state_count;
	std::set<int> f_initial;
	std::set<int> f_final;
	multimap<pair<int,int>, int> f_transitions;

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
			// nasty...
			serial = automaton->serialize();
			si = serial.begin();
			if(!deserialize_automaton(si, serial.end(), f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
				cerr << "failed to decompose automaton to human readable(1)\n";
				return false;
			}
			if(si != serial.end()) {
				cerr << "failed to decompose automaton to human readable(2)\n";
				return false;
			}
			f_is_dfa = automaton_is_deterministic(f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
			cout << write_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);

			return true;
		case output_sample:
		case output_sample_text:
			knowledgebase<bool> base;

			if(!generate_samples(automaton, base, sampletype)) {
				cerr << "failed to generate samples\n";
				return false;
			}

			if(out == output_sample) {
				basic_string<int32_t> serial;
				serial = base.serialize();
				if(!basic_string_to_fd(serial, STDOUT_FILENO)) {
					cerr << "failed to write serial\n";
					return false;
				}
			} else {
				if(out == output_sample_text) {
					cout << base.generate_dotfile();
				}
			}

			return true;
	}

	return false;
}}}

