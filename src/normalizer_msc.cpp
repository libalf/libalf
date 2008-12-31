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
#include <string>

#include <arpa/inet.h>

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
		buffer_match.push_back(ntohl(*it));
	}

	// get max buffer length
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	max_buffer_length = ntohl(*it);

	size--; it++;

	if(size == 0)
		return true;

deserialization_failed:
	clear();
	max_buffer_length = 0;
	return false;
}}}

bool normalizer_msc::deserialize_extension(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
	return false;
}


list<int> normalizer_msc::prefix_normal_form(list<int> & w, bool &bottom)
{{{
	list<int> ret;

	bool cleanup = false;
	int * buffer_length;
	int * bli;

	list<int>::iterator wi;
	int i;

	bottom = false;

	// first create a MSC from the word
	for(wi = w.begin(), i = 0; wi != w.end(); wi++, i++)
		if(!graph_add_node(i, *wi))
			bottom = true;

	if(!bottom) {
		cleanup = true;
		buffer_length = (int*)calloc(total_order.size(), sizeof(int));
	}

	// create normalized word
	while( ! graph.empty()) {
		if(!bottom) {
			i = graph_reduce();

			bli = &(buffer_length[buffer_match[i]]);

			// check buffer sizes
			if(i % 1) {
				// odd, so this is a send-event
				*bli++;
				if(*bli > max_buffer_length)
					bottom = true;
			} else {
				// even, so this is a receive-event
				*bli--;
				if(*bli < 0)
					bottom = true;
			}

			ret.push_back(i);
		} else {
			ret.push_back(graph_reduce());
		}
	}

	if(cleanup)
		free(buffer_length);
	return ret;
}}}

list<int> normalizer_msc::suffix_normal_form(list<int> & w, bool &bottom)
{
	// FIXME: implement normalizer_msc::suffix_normal_form
	bottom = false;
	return w;
}

bool normalizer_msc::graph_add_node(int id, int label)
{
	// FIXME: implement normalizer_msc::graph_add_node
}

int normalizer_msc::graph_reduce()
{
	// FIXME: implement normalizer_msc::graph_reduce
}

}; // end namespace libalf

