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

#include <getopt.h>

#include "fat.h"

void usage()
{{{
	cerr << "Finite Automata Tool\n"
		"Version "VERSION "\n"
		"\n"
		"allowed parameters:\n"
		"\n"
		"Input options: (default is to read serialized automaton from STDIN)\n"
		"\n"
		"\t-h or --human_readable_input\n"
		"\t\t\tread human readable automaton from STDIN\n"
		"\n"
		"\t-g or --generate <nfa|dfa|regex>:<int>\n"
		"\t\t\tgenerate an automaton, either using a random\n"
		"\t\t\tnfa, dfa or regex with given model-size\n"
		"\n"
		"Transformation options:\n"
		"\n"
		"\t-m or --minimize\n"
		"\t\t\tminimize automaton\n"
		"\n"
		"\t-d or --determinize\n"
		"\t\t\tdeterminize automaton\n"
		"\n"
		"\t-r or --rfsa\n"
		"\t\t\tget minimal RFSA of automaton\n"
		"\n"
//		"\t-u or --universal\n"
//		"\t\t\tget universal automaton\n"
//		"\n"
		"Output options:\n"
		"\n"
		"\t-D or --dotfile\n"
		"\t\t\tgenerate dotfile of automaton or knowledgebase\n"
		"\n"
		"\t-H or --human_readable_output\n"
		"\t\t\tgenerate dotfile of automaton or knowledgebase\n"
		"\n"
		"\t-S or --sample <rpni|delete2|biermann|random|depth>[:<int>]\n"
		"\t\t\tgenerate sample-set (knowledgebase) from automaton,\n"
		"\t\t\tw.r.t. some criterion. <int> only is required for\n"
		"\t\t\t random: number of samples\n"
		"\t\t\t depth:  depth of prefix-accepting tree to generate\n"
		"\n"
		"\t-T or --text_sample <rpni|delete2|biermann|random|depth>[:<int>]\n"
		"\t\t\tgenerate sample-set and output it in textform\n"
		"\n";
}}}

int main(int argc, char**argv)
{

	const struct option server_long_options[] = {
		// { const char *name , int has_arg , int *flag, int val }
		// has_arg \in { required_argument, optional_argument, no_argument };
		{ "human_readable_input",   no_argument,            NULL, 'h' },	// read human-readable automaton from stdin
		{ "generate",               required_argument,      NULL, 'g' },	// generate a random finite automaton. parameter: {nfa, dfa, regex}:{model-size}

		{ "minimize",               no_argument,            NULL, 'm' },	// minimize finite automaton
		{ "determinize",            no_argument,            NULL, 'd' },	// determinize automaton
		{ "rfsa",                   no_argument,            NULL, 'r' },	// get canonical RFSA of this automaton
//		{ "universal",              no_argument,            NULL, 'u' },	// universal automaton

		{ "dotfile",                no_argument,            NULL, 'D' },	// generate dotfile
		{ "human_readable_output",  no_argument,            NULL, 'H' },	// use text-output (tostring() for knowledgebase, write() for automata)
		{ "sample",                 required_argument,      NULL, 'S' },	// create sample-set from automaton. parameter: rpni, delete2, biermann, random
		{ "text_sample",            required_argument,      NULL, 'T' },	// create text-version of sample-set
		{ 0,0,0,0 }
	};

	if(argc < 2) {
		cerr << "missing parameters.\n\n";
		usage();
		return -1;
	}

	string gentype, sampletype;

	enum input in = input_serial;
	enum transformation trans = trans_none;
	enum output out = output_serial;

	int c;
	while(0 <= (c = getopt_long(argc, argv, "hg:mdrDHS:T", server_long_options, NULL)))
	{{{ // parse command line
		switch (c) {
			case 'h':
				if(in != input_serial) {
					cerr << "invalid input constellation.\n\n";
					usage();
					return -1;
				} else {
					in = input_human_readable;
				}
				break;
			case 'g':
				if(in != input_serial) {
					cerr << "invalid input constellation.\n\n";
					usage();
					return -1;
				} else {
					in = input_generate;
				}
				gentype = optarg;

				break;
			case 'm':
				if(trans != trans_none) {
					switch (trans) {
						case trans_determinize:
							trans = trans_mdfa;
							break;
						default:
							cerr << "invalid transformation constellation.\n\n";
							usage();
							return -1;
					}
				} else {
					trans = trans_minimize;
				}

				break;
			case 'd':
				if(trans != trans_none) {
					switch (trans) {
						case trans_minimize:
							trans = trans_mdfa;
							break;
						default:
							cerr << "invalid transformation constellation.\n\n";
							usage();
							return -1;
					}
				} else {
					trans = trans_determinize;
				}

				break;
			case 'r':
				if(trans != trans_none) {
					cerr << "invalid transformation constellation.\n\n";
					usage();
					return -1;
				} else {
					trans = trans_rfsa;
				}

				break;
			case 'D':
				if(out != output_serial) {
					cerr << "invalid output constellation.\n\n";
					usage();
					return -1;
				} else {
					out = output_dotfile;
				}

				break;
			case 'H':
				if(out != output_serial) {
					cerr << "invalid output constellation.\n\n";
					usage();
					return -1;
				} else {
					out = output_human_readable;
				}

				break;
			case 'S':
				if(out != output_serial) {
					cerr << "invalid output constellation.\n\n";
					usage();
					return -1;
				} else {
					out = output_sample;
				}
				sampletype = optarg;

				break;
			case 'T':
				if(out != output_serial) {
					cerr << "invalid output constellation.\n\n";
					usage();
					return -1;
				} else {
					out = output_sample_text;
				}

				break;
			case '?':
			default:
				cerr << "bad argument.\n\n";
				usage();
				return -1;
		}
	}}}

	if(trans != trans_none && ( out == output_sample || out == output_sample_text) ) {
		cerr << "invalid transformation/output constellation\n\n";
		usage();
		return -1;
	}


	if(optind < argc) { // unparsed arguments left
		cerr << "too many arguments.\n\n";
		usage();
		return -1;
	}

	finite_automaton * automaton;
	knowledgebase<bool> knowledge;

	if(!get_input(automaton, in, gentype)) {
		cerr << "input failed\n";
		return -2;
	}

	if(!do_transformation(automaton, trans)) {
		cerr << "transformation failed\n";
		return -3;
	}

	if(!write_output(automaton, out, sampletype)) {
		cerr << "output failed\n";
		return -4;
	};

	return 0;
};

