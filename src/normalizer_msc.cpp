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

#include <stdlib.h>

#include <list>
#include <queue>
#include <string>

#include <arpa/inet.h>

#include <libalf/alphabet.h>
#include <libalf/normalizer.h>
#include <libalf/normalizer_msc.h>

namespace libalf {

using namespace std;

msc::msc_node::msc_node()
{{{
	id = -1;
	label = -1;
	process_in = NULL;
	process_out = NULL;
	buffer_in = NULL;
	buffer_out = NULL;
}}}

msc::msc_node::~msc_node()
{{{
	// delete references from other nodes
	if(process_in)
		process_in->process_out = NULL;
	if(buffer_in)
		buffer_in->buffer_out = NULL;
}}}



normalizer_msc::normalizer_msc()
{{{
	// nothing.
}}}

normalizer_msc::normalizer_msc(vector<int> &total_order, vector<int> &process_match, vector<int> &buffer_match, int max_buffer_length)
{{{
	this->total_order = total_order;
	this->process_match = process_match;
	this->buffer_match = buffer_match;
	this->max_buffer_length = max_buffer_length;
}}}

void normalizer_msc::clear()
{{{
	total_order.clear();
	process_match.clear();
	buffer_match.clear();
	max_buffer_length = 0;
}}}

basic_string<int32_t> normalizer_msc::serialize()
{{{
	basic_string<int32_t> ret;
	vector<int>::iterator vi;

	ret += 0; // length field, will be filled in later.

	// type
	ret += htonl(normalizer::NORMALIZER_MSC);

	// total order
	ret += htonl(total_order.size());
	for(vi = total_order.begin(); vi != total_order.end(); vi++)
		ret += htonl(*vi);

	// message-process-matching
	ret += htonl(process_match.size());
	for(vi = process_match.begin(); vi != process_match.end(); vi++)
		ret += htonl(*vi);

	// message-buffer-matching
	ret += htonl(buffer_match.size());
	for(vi = buffer_match.begin(); vi != buffer_match.end(); vi++)
		ret += htonl(*vi);

	// max buffer length
	ret += htonl(max_buffer_length);

	ret[0] = htonl(ret.size() - 1);

	return ret;
}}}

bool normalizer_msc::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{{{
	int size;
	int count;
	enum normalizer::type type;

	if(it == limit)
		goto deserialization_failed;

	clear();

printf("MSC deserializer:\n");

	// data size
	size = ntohl(*it);
printf("size %d\n", size);

	// check type
	it++; if(size <= 0 || limit == it) goto deserialization_failed;
	type = (enum normalizer::type) ntohl(*it);
	if(type != NORMALIZER_MSC)
		goto deserialization_failed;
printf("type %d is ok\n", type);

	// get total order
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	count = ntohl(*it);
printf("total order of size %d\n", count);
	for(count = ntohl(*it); count > 0; count--) {
		size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
		total_order.push_back(ntohl(*it));
	}
printf("ok.\n");

	// get message-process-matching
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	count = ntohl(*it);
printf("msg-process-match of size %d\n", count);
	for(count = ntohl(*it); count > 0; count--) {
		size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
		process_match.push_back(ntohl(*it));
	}
printf("ok.\n");

	// get message-buffer-matching
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	count = ntohl(*it);
printf("msg-buffer-match of size %d\n", count);
	for(count = ntohl(*it); count > 0; count--) {
		size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
		buffer_match.push_back(ntohl(*it));
	}

printf("ok.\n");

	// get max buffer length
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	max_buffer_length = ntohl(*it);

printf("max buffer length %d\n", max_buffer_length);

	size--; it++;

	if(size == 0)
		return true;

printf("too much data supposed to be at end: %d\n", size);
if(it == limit)
	printf("but limit == it\n");

deserialization_failed:
	clear();
	max_buffer_length = 0;
	return false;
}}}

bool normalizer_msc::deserialize_extension(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
	// FIXME: implement normalizer_msc::deserialize_extension
	return false;
}


list<int> normalizer_msc::prefix_normal_form(list<int> & w, bool &bottom)
{{{
	if(w.front() == BOTTOM_CHAR) {
		bottom = true;
		return w;
	}

	list<int> ret;
	list<int>::iterator wi;
	unsigned int i;
	int maxbuffers;

	bottom = false;

	// check for bottom;
	// FIXME: this SUCKS due to its inefficiency.
	// instead, use some class-internally allocated fifos and clear them after pnf().
	for(i = 0; i < buffer_match.size(); i++)
		if(maxbuffers < buffer_match[i])
			maxbuffers = buffer_match[i];

	queue<int> *buffers = new queue<int>[maxbuffers+1];

	for(wi = w.begin(); wi != w.end(); wi++) {
		int buffer = buffer_match[*wi];
		int msg = *wi / 2;
		if(*wi % 2) {
			// odd: receive-event
			if(buffers[buffer].size() == 0) {
				bottom = true;
				break;
			}
			if(buffers[buffer].front() != msg) {
				bottom = true;
				break;
			}
			buffers[buffer].pop();
		} else {
			// even: send-event
			buffers[buffer].push(msg);
			if(max_buffer_length > 0) {
				if(buffers[buffer].size() > (unsigned int)max_buffer_length) {
					bottom = true;
					break;
				}
			}
		}
	}

	delete[] buffers;

	// if bottom, return bad word and mark it.
	if(bottom) {
		ret.push_back(BOTTOM_CHAR);
	} else {
		// first create a MSC from the word
		for(wi = w.begin(), i = 0; wi != w.end(); wi++, i++)
			graph_add_node(i, *wi);

		//graph_print();

		// create normalized word
		while( ! graph.empty()) {
			int r;
			r = graph_reduce();
			ret.push_back(r);
		}
	}

	return ret;
}}}

list<int> normalizer_msc::suffix_normal_form(list<int> & w, bool &bottom)
{{{
	if(w.front() == BOTTOM_CHAR) {
		bottom = true;
		return w;
	}

	list<int> tmp,snf;
	list<int>::reverse_iterator wi;

	// invert order and send/receive
	for(wi = w.rbegin(); wi != w.rend(); wi++) {
		if(*wi % 2)
			snf.push_back(*wi - 1);
		else
			snf.push_back(*wi + 1);
	}

	// PNF of inverted
	tmp = prefix_normal_form(snf, bottom);
	if(bottom)
		return tmp;

	snf.clear();

	// back-invert order and send/receive
	for(wi = tmp.rbegin(); wi != tmp.rend(); wi++) {
		if(*wi % 2)
			snf.push_back(*wi - 1);
		else
			snf.push_back(*wi + 1);
	}

	return snf;
}}}

void normalizer_msc::graph_add_node(int id, int label)
{{{
	msc::msc_node n;
	list<msc::msc_node>::iterator ni, newnode, extrema;

	n.id = id;
	n.label = label;
	graph.push_back(n);

	newnode = graph.end();
	newnode--;

	// PROC-connection:
	// connect node to other youngest node with same process
	// that is not connected.
	extrema = graph.end();
	for(ni = graph.begin(); ni != newnode; ni++) {
		if(ni->is_process_connected())
			continue;
		if(process_match[ni->label] != process_match[label])
			continue;
		if(extrema == graph.end() || ( extrema->id < ni->id ))
			extrema = ni;
	}
	if(extrema != graph.end())
		extrema->connect_process(*newnode);

	// MSG-connection:
	// if this is a receiving event:
	// connect node to oldest corresponding send-event
	// that is not connected.
	extrema = graph.end();
	if(label % 2) { // odd == receiving event
		for(ni = graph.begin(); ni != newnode; ni++) {
			if( (ni->label / 2 != label / 2) || (ni->label % 2) )
				continue;
			if(ni->is_buffer_connected())
				continue;
			if(extrema == graph.end() || ( ni->id < extrema->id ))
				extrema = ni;
		}
	}
	if(extrema != graph.end())
		extrema->connect_buffer(*newnode);
}}}

int normalizer_msc::graph_reduce()
{{{
	list<msc::msc_node>::iterator ni, extrema;

	extrema = graph.end();
	for(ni = graph.begin(); ni != graph.end(); ni++) {
		// only use minimal nodes (without incoming edges)
		if(ni->is_process_referenced() || ni->is_buffer_referenced())
			continue;
		if(extrema == graph.end()) {
			extrema = ni;
			continue;
		}
		// if there exists a minimal receive-event, never fall back to send-events
		if( (extrema->label % 2) && (ni->label % 2 == 0) )
			continue;
		if( (extrema->label % 2 == 0) && (ni->label % 2) ) {
			extrema = ni;
			continue;
		}

		if(ni->label < extrema->label)
			extrema = ni;
	}

	if(extrema == graph.end())
		return -1;

	int label;

	label = extrema->label;
	extrema->disconnect();
	graph.erase(extrema);

	return label;
}}}

void normalizer_msc::graph_print()
{{{
	list<msc::msc_node>::iterator ni;

	printf("~~~ graph\n");

	for(ni = graph.begin(); ni != graph.end(); ni++) {
		printf(" node %d: label %d\n", ni->id, ni->label);
		printf("   process con: ");
		if(ni->is_process_connected())
			printf("to %d", ni->process_out->id);
		if(ni->is_process_referenced())
			printf("from %d", ni->process_in->id);
		printf("\n" "   buffer con: ");
		if(ni->is_buffer_connected())
			printf("to %d", ni->buffer_out->id);
		if(ni->is_buffer_referenced())
			printf("from %d", ni->buffer_in->id);
		printf("\n");
	}

	printf("---\n");
}}}

}; // end namespace libalf

