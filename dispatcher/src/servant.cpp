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

#include <libalf/alf.h>

#include "servant.h"
#include "serversocket.h"
#include "protocol.h"

using namespace std;
using namespace libalf;

servant::servant()
{{{
	client = NULL;
}}}

servant::servant(serversocket *connection)
{{{
	client = connection;
}}}

servant::~servant()
{{{
	if(client)
		delete client;
}}}

bool servant::serve()
{
	if(client == NULL)
		return false;



	// FIXME
}

