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
 * (c) 2008,2009,2010,2011
 *		Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *	    and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <iostream>
#include <string>
#include <list>

#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <stdarg.h>

#include <libalf/alf.h>

#include "main.h"
#include "protocol.h"
#include "serversocket.h"
#include "servant.h"

using namespace std;

static bool show_help = false;

std::string listen_address; // if empty, listen on all
static uint16_t  listen_port = DISPATCHER_DEFAULT_PORT;

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

				listen_address = optarg;

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
	cout << "\n"
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

char * dispatcher_version()
{{{
	static char version[1024];
	static bool done = false;

	if(!done) {
		snprintf(version, 1023, "ALF dispatcher version %s\n"
					"%s\n"
					"dispatcher protocol version %d\n"

					"(c) 2008,2009,2010,2011\n"
					"         Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University\n"
					"     and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University\n"
					"Authors: Carsten Kern, Daniel Neider, David R. Piegdon, Stefan Rieger and Stefan Schulz\n"
					"http://libalf.informatik.rwth-aachen.de/\n",
					VERSION,
					libalf::libalf_version(),
					DISPATCHER_PROTOCOL_VERSION);
		version[1023] = 0;
		done = true;
	}

	return version;
}}}

void print_time()
{{{
	char timestr[64];
	time_t now;
	struct tm* now_brk;


	now = time(NULL);
	now_brk = localtime(&now);

	strftime(timestr, 64, "[%F %T] ", now_brk);
	timestr[63] = 0;

	printf("%s", timestr);
}}}

void log(const char * format, ...)
{{{
	va_list ap;

	print_time();

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}}}

int main(int argc, char**argv)
{{{
	cout << dispatcher_version() << "\n";

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
	serversocket *master = new serversocket;

	if( ! master->bind(listen_address, listen_port) ) {
		cout << "failed to bind socket to " << listen_address << ":" << listen_port << ". aborting.\n";
		cout << "(" << strerror(errno) << ")\n";
		return -1;
	}
	if( ! master->listen(5) ) {
		cout << "failed to set socket to listen mode. aborting.\n";
		return -2;
	}

	log("dispatcher now listening on port %d.\n", listen_port);

	fd_set fds;
	timeval t;

	while(1) {
		FD_ZERO(&fds);
		FD_SET(master->sock, &fds);
		t.tv_sec = 1;
		t.tv_usec = 0;
		if(select(master->sock+1, &fds, NULL, NULL, &t)) {
			struct sockaddr_in remote_addr;
			socklen_t sin_size = sizeof(struct sockaddr_in);
			serversocket *cl = master->accept(remote_addr, sin_size);

			if( ! cl) {
				log("ASSERT: master->accept() returned NULL. ignoring.\n");
			} else {
				int pid = fork();
				if(pid < 0) {
					log("FAILED TO FORK. ABORTING.\n");
					return -3;
				}
				if(pid == 0) {
					// child
					// get rid of master socket
					delete master;

					servant sv(cl);

					while(sv.serve()) ;

					// end child.
					log("TERMINATING: client %d.\n", getpid());
					return 0;
				} else {
					log("NEW CLIENT: from %s, PID %d.\n", inet_ntoa(remote_addr.sin_addr), pid);
					// parent
					// get rid of client socket
					delete cl;
				}
			}
		}
		waitpid(-1, NULL, WNOHANG);
	}

	// this should never be reached
	return -4;
}}}

