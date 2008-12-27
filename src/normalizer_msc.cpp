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

normalizer_msc::normalizer_msc(vector<int> &total_order, vector<int> &msg_process_match, vector<int> &msg_buffer_match, int max_buffer_length)
{{{
	this->total_order = total_order;
	this->msg_process_match = msg_process_match;
	this->msg_buffer_match = msg_buffer_match;
	this->max_buffer_length = max_buffer_length;
}}}

void normalizer_msc::clear()
{{{
	total_order.clear();
	msg_process_match.clear();
	msg_buffer_match.clear();
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
	ret += htonl(msg_process_match.size());
	for(vi = msg_process_match.begin(); vi != msg_process_match.end(); vi++)
		ret += htonl(*vi);

	// message-buffer-matching
	ret += htonl(msg_buffer_match.size());
	for(vi = msg_buffer_match.begin(); vi != msg_buffer_match.end(); vi++)
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
		msg_process_match.push_back(ntohl(*it));
	}

	// get message-buffer-matching
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	count = ntohl(*it);
	for(count = ntohl(*it); count > 0; count--) {
		size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
		msg_buffer_match.push_back(ntohl(*it));
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
{
	list<int> ret;
	bottom = false;

	// first create a MSC from the word
	list<int>::iterator wi;
	int i;
	for(wi = w.begin(), i = 0; wi != w.end(); wi++, i++)
		if(!graph_add_node(i, *wi))
			bottom = true;

	// create normalized word
	while( ! graph.empty()) {
		// FIXME: check max_buffer_length
		ret.push_back(graph_reduce());
	}

	return w;
}

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

