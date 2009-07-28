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
#include <string>

#include <getopt.h>

using namespace std;

void usage()
{
	cerr << "Finite Automata Tool\n"
		"Version "VERSION "\n"
		"\n"
		"allowed parameters:\n"
		"\n"
		"\t-g or --generate <nfa|dfa|regex>:<int>\n"
		"\t\t\tgenerate an automaton, either using a random\n"
		"\t\t\tnfa, dfa or regex with given model-size\n"
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
		"\t-D or --dotfile\n"
		"\t\t\tgenerate dotfile of automaton or knowledgebase\n"
		"\n"
		"\t-s or --sample <rpni|delete2|biermann|random|depth>[:<int>]\n"
		"\t\t\tgenerate sample-set (knowledgebase) from automaton,\n"
		"\t\t\tw.r.t. some criterion. <int> only is required for\n"
		"\t\t\t random: number of samples\n"
		"\t\t\t depth:  depth of prefix-accepting tree to generate\n"
		"\n";
}

int main(int argc, char**argv)
{
	const struct option server_long_options[] = {
		// { const char *name , int has_arg , int *flag, int val }
		// has_arg \in { required_argument, optional_argument, no_argument };
		{ "generate",        required_argument,      NULL, 'g' },	// generate a random finite automaton. parameter: {nfa, dfa, regex}:{model-size}
		{ "minimize",        no_argument,            NULL, 'm' },	// minimize finite automaton
		{ "determinize",     no_argument,            NULL, 'd' },	// determinize automaton
		{ "rfsa",            no_argument,            NULL, 'r' },	// get canonical RFSA of this automaton
//		{ "universal",       no_argument,            NULL, 'u' },	// universal automaton

		{ "dotfile",         no_argument,            NULL, 'D' },	// generate dotfile
		{ "text",            no_argument,            NULL, 't' },	// use text-output (tostring() for knowledgebase, write() for automata)

		{ "sample",          required_argument,      NULL, 's' },	// create sample-set from automaton. parameter: rpni, delete2, biermann, random
		{ 0,0,0,0 }
	};

	if(argc < 2) {
		cerr << "missing parameters.\n\n";
		usage();
		return -1;
	}

	string gentype, sampletype;
	int c;
	while(0 <= (c = getopt_long(argc, argv, "g:mdrDs:", server_long_options, NULL))) {
		switch (c) {
			case 'g':
				gentype = optarg;

				break;
			case 'm':

				break;
			case 'd':

				break;
			case 'r':

				break;
			case 'D':

				break;
			case 's':
				sampletype = optarg;

				break;
			case '?':
			default:
				cerr << "bad argument.\n\n";
				usage();
				return -1;
		}
	}

	if(optind < argc) { // unparsed arguments left
		cerr << "too many arguments.\n\n";
		usage();
		return -1;
	}


}

