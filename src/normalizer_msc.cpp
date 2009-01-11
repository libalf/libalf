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
	buffers = NULL;
}}}

normalizer_msc::normalizer_msc(vector<int> &total_order, vector<int> &process_match, vector<int> &buffer_match, int max_buffer_length)
{{{
	this->total_order = total_order;
	this->process_match = process_match;
	this->buffer_match = buffer_match;
	this->max_buffer_length = max_buffer_length;

	int maxbuffers = 0;
	for(vector<int>::iterator vi = buffer_match.begin(); vi != buffer_match.end(); vi++)
		if(*vi > maxbuffers)
			maxbuffers = *vi;

	buffers = new queue<int>[maxbuffers + 1];
}}}

void normalizer_msc::clear()
{{{
	if(buffers) {
		delete[] buffers;
		buffers = NULL;
	}
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
	int maxbuffers = 0;
	enum normalizer::type type;

	if(it == limit)
		goto deserialization_failed;

	clear();

	// data size
	size = ntohl(*it);

	// check type
	it++; if(size <= 0 || limit == it) goto deserialization_failed;
	type = (enum normalizer::type) ntohl(*it);
	if(type != NORMALIZER_MSC)
		goto deserialization_failed;

	// get total order
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	count = ntohl(*it);
	for(count = ntohl(*it); count > 0; count--) {
		size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
		total_order.push_back(ntohl(*it));
	}

	// get message-process-matching
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	count = ntohl(*it);
	for(count = ntohl(*it); count > 0; count--) {
		size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
		process_match.push_back(ntohl(*it));
	}

	// get message-buffer-matching
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	count = ntohl(*it);
	for(count = ntohl(*it); count > 0; count--) {
		size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
		if(*it < 0)
			return false;
		if(*it > maxbuffers)
			maxbuffers = *it;
		buffer_match.push_back(ntohl(*it));
	}

	// get max buffer length
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	max_buffer_length = ntohl(*it);

	size--; it++;

	// FIXME: sanity-check size of vectors. message-buffer-match may be null if max_buffer_size <= 0

	if(size == 0) {
		buffers = new queue<int>[maxbuffers + 1];
		return true;
	}

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

bool normalizer_msc::check_bottom(list<int> & word, bool pnf)
{{{
	if(word.front() == BOTTOM_CHAR)
		return true;

	if(max_buffer_length <= 0)
		return false;

	list<int>::iterator wi;
	bool bottom = false;

	for(wi = word.begin(); wi != word.end(); wi++) {
		int buffer = buffer_match[*wi];
		int msg = *wi / 2;

		if((*wi % 2 && pnf) || (*wi % 2 == 0 && !pnf)) {
			// PNF: odd: receive-event
			// SNF: even: act like receive-event
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
			// PNF: even: send-event
			// SNF: odd: act like send-event
			buffers[buffer].push(msg);
			if(max_buffer_length > 0) {
				if(buffers[buffer].size() > (unsigned int)max_buffer_length) {
					bottom = true;
					break;
				}
			}
		}
	}

	// clear used buffers
	for(wi = word.begin(); wi != word.end(); wi++) {
		queue<int> * q = &buffers[buffer_match[*wi]];
		while(!q->empty())
			q->pop();
	}

	return bottom;
}}}

list<int> normalizer_msc::prefix_normal_form(list<int> & w, bool & bottom)
{{{
	list<int> ret;
	list<int>::iterator wi;
	int i;

	bottom = check_bottom(w, true);
	if(bottom) {
		ret.push_back(BOTTOM_CHAR);
		return ret;
	}

	// create a MSC from the word
	for(wi = w.begin(), i = 0; wi != w.end(); wi++, i++)
		graph_add_node(i, *wi, true);

//	graph_print();

	// create normalized word
	while( ! graph.empty())
		ret.push_back(graph_reduce(true));

	return ret;
}}}

list<int> normalizer_msc::suffix_normal_form(list<int> & w, bool & bottom)
{{{
	list<int> ret;
	list<int>::iterator wi;
	int i = 0;

	for(wi = w.begin(); wi != w.end(); wi++)
		ret.push_front(*wi);

	bottom = check_bottom(ret, false);
	if(bottom) {
		ret.clear();
		ret.push_back(BOTTOM_CHAR);
		return ret;
	}

	// create a MSC from the word
	while(!ret.empty()) {
		graph_add_node(i, ret.front(), false);
		ret.pop_front();
		i++;
	}

//	graph_print();

	// create normalized word
	while( ! graph.empty())
		ret.push_front(graph_reduce(false));

	return ret;
}}}

void normalizer_msc::graph_add_node(int id, int label, bool pnf)
{{{
	msc::msc_node * newnode = new msc::msc_node();
	list<msc::msc_node*>::iterator ni, extrema;

	newnode->id = id;
	newnode->label = label;
	graph.push_back(newnode);

	// PROC-connection:
	// PNF: connection to node from other youngest node with same process that is not connected
	// SNF: connection from node to other youngest node with same process that is not connected
	extrema = graph.end();
	for(ni = graph.begin(); *ni != newnode; ni++) {
		if(pnf && (*ni)->is_process_connected() || !pnf && (*ni)->is_process_referenced())
			continue;
		if(process_match[(*ni)->label] != process_match[label])
			continue;
		if(extrema == graph.end()) {
			extrema = ni;
		} else {
			if(pnf && (*extrema)->id < (*ni)->id)
				extrema = ni;
			if(!pnf && (*extrema)->id > (*ni)->id)
				extrema = ni;
		}
	}
	if(extrema != graph.end()) {
		if(pnf)
			(*extrema)->connect_process(newnode);
		else
			newnode->connect_process(*extrema);
	}

	// MSG-connection:
	// PNF: if this is a receiving event, connect to oldest corresponding send-event that is not connected
	// SNF: if this is a sending event, connect from oldest corresponding send-event that is not connected
	extrema = graph.end();
	if( (label % 2) && pnf || (label % 2 == 0) && !pnf) {
		// pnf: odd, receive-event
		// snf: even, send-event
		for(ni = graph.begin(); *ni != newnode; ni++) {
			if( ((*ni)->label / 2 != label / 2))
				continue;
			if(pnf) {
				if(((*ni)->label % 2) || (*ni)->is_buffer_connected())
					continue;
			} else {
				if(((*ni)->label % 2 == 0) || (*ni)->is_buffer_referenced())
					continue;
			}
			if(extrema == graph.end()) {
				extrema = ni;
			} else {
				if((*ni)->id < (*extrema)->id)
					extrema = ni;
			}
		}
	}
	if(extrema != graph.end()) {
		if(pnf)
			(*extrema)->connect_buffer(newnode);
		else
			newnode->connect_buffer(*extrema);
	}
}}}

int normalizer_msc::graph_reduce(bool pnf)
{{{
	list<msc::msc_node*>::iterator ni, extrema;

	extrema = graph.end();
	for(ni = graph.begin(); ni != graph.end(); ni++) {
		// PNF: only use minimal nodes (without incoming edges)
		if(pnf && ((*ni)->is_process_referenced() || (*ni)->is_buffer_referenced()))
			continue;
		// SNF: only use maximal nodes (without outgoing edges)
		if(!pnf && ((*ni)->is_process_connected() || (*ni)->is_buffer_connected()))
			continue;
		if(extrema == graph.end()) {
			extrema = ni;
			continue;
		}
		// PNF: if there exists a minimal receive-event, never fall back to send-events
		// SNF: if there exists a minimal send-event, never fall back to receive-events
		if(pnf && ((*extrema)->label % 2) && ((*ni)->label % 2 == 0) )
			continue;
		if(pnf && ((*extrema)->label % 2 == 0) && ((*ni)->label % 2) ) {
			extrema = ni;
			continue;
		}
		if(!pnf && ((*extrema)->label % 2 == 0) && ((*ni)->label % 2) )
			continue;
		if(!pnf && ((*extrema)->label % 2) && ((*ni)->label % 2 == 0) ) {
			extrema = ni;
			continue;
		}

		if(total_order[(*ni)->label] < total_order[(*extrema)->label])
			extrema = ni;
	}

	if(extrema == graph.end())
		return -1;

	int label;

	label = (*extrema)->label;
	(*extrema)->disconnect();
	delete *extrema;
	graph.erase(extrema);

	return label;
}}}

void normalizer_msc::graph_print()
{{{
	list<msc::msc_node*>::iterator ni;

	printf("~~~ graph\n");

	for(ni = graph.begin(); ni != graph.end(); ni++) {
		printf(" node %d: label %d\n", (*ni)->id, (*ni)->label);
		printf("   process con: ");
		if((*ni)->is_process_connected())
			printf("to %d ", (*ni)->process_out->id);
		if((*ni)->is_process_referenced())
			printf("from %d", (*ni)->process_in->id);
		printf("\n" "   buffer con: ");
		if((*ni)->is_buffer_connected())
			printf("to %d", (*ni)->buffer_out->id);
		if((*ni)->is_buffer_referenced())
			printf("from %d", (*ni)->buffer_in->id);
		printf("\n");
	}

	printf("---\n");
}}}

}; // end namespace libalf

