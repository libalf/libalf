/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>
#include <string>
#include <list>

#include <unistd.h>
#include <getopt.h>

// libalf headers
#include <libalf/alf.h>

// stuff needed for amore automata
#include <amore/vars.h>

// boost network classes
//#include <boost/asio.hpp>

#include "protocol.h"

using namespace std;
using namespace dispatcher;

static bool show_help = false;

std::string listen_addresses; // if empty, listen on all
static int  listen_port = DISPATCHER_DEFAULT_PORT;

bool parse_commandline(int argc, char**argv)
{{{
	bool port_set = false;
	bool listen_set = false;
	int c;
	std::string str;

	const struct option dispatcher_long_options[] = {
		{ "listen",	required_argument,	NULL,	'l' },
		{ "port",	required_argument,	NULL,	'p' },
		{ 0,0,0,0 }
	};

	while( 0 <= (c = getopt_long(argc, argv, "l:p:", dispatcher_long_options, NULL) ) ) {
		switch (c) {
			case 'l':
				if(listen_set)
					cout << "multiple listen-addresses given on command-line. only using last given listen-address.\n";
				listen_set = true;

				listen_addresses = optarg;

				break;
			case 'p':
				if(port_set)
					cout << "multiple listen-ports given on command-line. only using last given port.\n";
				port_set = true;

				listen_port = atoi(optarg);

				break;
			default:
				return false;
				break;
		}
	}

	if(optind < argc) {
		// unparsed garbage at end
		cout << "garbage at end of commandline.\n";
		return false;
	}

	return true;
}}}

void help()
{{{
	cout <<
		"\n"
		"libalf dispatcher (version " << VERSION << ")\n"
		"accepted command-line parameters:\n"
		"\t-l <IP>\n"
		"\t--listen <IP>\n"
		"\t          only listen on this IP\n"
		"\t-p <port>\n"
		"\t--port <port>\n"
		"\t          listen on this port (defaults to " << DISPATCHER_DEFAULT_PORT << ")\n"
		"\n"
		"to stop the server, just send a SIGINT or SIGQUIT (^C or ^\\)\n"
		"\n";
}}}

int main(int argc, char**argv)
{
	// parse command-line
	if( ! parse_commandline(argc, argv) ) {
		help();
		return 1;
	};

	if( show_help ) {
		help();
		return 0;
	};

	// set up server for listening





	// this should never be reached
	return -1;
}

