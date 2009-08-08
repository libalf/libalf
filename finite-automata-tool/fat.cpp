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
		"libalf Version "VERSION "\n"
		"\n"
		"All input and output is done via stdin/stdout.\n"
		"Allowed parameters:\n"
		"\n"
		"Input options:\n"
		"\tdefault:\n"
		"\t\t\tread serialized automaton\n"
		"\t-h or --human_readable_input\n"
		"\t\t\tread human readable automaton\n"
		"\t-g or --generate <nfa|dfa|regex>:<int>:<int>\n"
		"\t\t\tgenerate an automaton, either using a random\n"
		"\t\t\tnfa, dfa or regex with given alphabet-size : model-size\n"
		"\n"
		"Transformation options (not changing the language):\n"
		"\tdefault:\n"
		"\t\t\tno transformation\n"
		"\t-m or --minimize\n"
		"\t\t\tminimize automaton\n"
		"\t-d or --determinize\n"
		"\t\t\tdeterminize automaton\n"
		"\t-r or --rfsa\n"
		"\t\t\tget minimal RFSA of automaton\n"
		"\n"
		"Output options:\n"
		"\tdefault:\n"
		"\t\t\twrite serialized automaton\n"
		"\t-H or --human_readable_output\n"
		"\t\t\tgenerate human readable version of automaton\n"
		"\t-D or --dotfile\n"
		"\t\t\tgenerate dotfile of automaton\n"
		"\t-S or --sample <rpni|delete2|biermann|random|depth>[:<int>]\n"
		"\t\t\tgenerate sample-set (knowledgebase) from automaton,\n"
		"\t\t\tw.r.t. some criterion. <int> only is required for\n"
		"\t\t\t random: number of samples\n"
		"\t\t\t depth:  depth of prefix-accepting tree to generate\n"
		"\t-T or --text_sample <rpni|delete2|biermann|random|depth>[:<int>]\n"
		"\t\t\tgenerate sample-set as above and output it in textform\n"
		"\n";
}}}

void tolower(string & str)
{{{
	size_t s;
	for(s = 0; s < str.size(); s++)
		str[s] = tolower(str[s]);
}}}

int main(int argc, char**argv)
{

	const struct option server_long_options[] = {
		// { const char *name , int has_arg , int *flag, int val }
		// has_arg \in { required_argument, optional_argument, no_argument };
		{ "help",                   no_argument,            NULL, '?' },	// help

		{ "human_readable_input",   no_argument,            NULL, 'h' },	// read human-readable automaton from stdin
		{ "generate",               required_argument,      NULL, 'g' },	// generate a random finite automaton. parameter: {nfa, dfa, regex}:{model-size}

		{ "minimize",               no_argument,            NULL, 'm' },	// minimize finite automaton
		{ "determinize",            no_argument,            NULL, 'd' },	// determinize automaton
		{ "rfsa",                   no_argument,            NULL, 'r' },	// get canonical RFSA of this automaton

		{ "human_readable_output",  no_argument,            NULL, 'H' },	// use text-output (tostring() for knowledgebase, write() for automata)
		{ "dotfile",                no_argument,            NULL, 'D' },	// generate dotfile
		{ "sample",                 required_argument,      NULL, 'S' },	// create sample-set from automaton. parameter: rpni, delete2, biermann, random
		{ "text_sample",            required_argument,      NULL, 'T' },	// create text-version of sample-set
		{ 0,0,0,0 }
	};

	string gentype, sampletype;

	enum input in = input_serial;
	enum transformation trans = trans_none;
	enum output out = output_serial;

	int c;
	while(0 <= (c = getopt_long(argc, argv, "hg:mdrHDS:T?", server_long_options, NULL)))
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
				tolower(gentype);

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
			case 'H':
				if(out != output_serial) {
					cerr << "invalid output constellation.\n\n";
					usage();
					return -1;
				} else {
					out = output_human_readable;
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
			case 'S':
				if(out != output_serial) {
					cerr << "invalid output constellation.\n\n";
					usage();
					return -1;
				} else {
					out = output_sample;
				}
				sampletype = optarg;
				tolower(sampletype);

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
				usage();
				return 0;
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

