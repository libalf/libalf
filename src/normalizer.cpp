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

namespace libalf {

using namespace std;

normalizer_msc::normalizer_msc()
{{{
	// nothing.
}}}

normalizer_msc::normalizer_msc(list<int> &total_order, list<int> &msg_process_match, list<int> &msg_buffer_match, int max_queue_length)
{{{
	this->total_order = total_order;
	this->msg_process_match = msg_process_match;
	this->msg_buffer_match = msg_buffer_match;
	this->max_queue_length = max_queue_length;
}}}

void normalizer_msc::clear()
{{{
	total_order.clear();
	msg_process_match.clear();
	msg_buffer_match.clear();
	max_queue_length = 0;
}}}

basic_string<int32_t> normalizer_msc::serialize()
{{{
	basic_string<int32_t> ret;
	list<int>::iterator li;

	ret += 0; // length field, will be filled in later.

	// type
	ret += htonl(normalizer::NORMALIZER_MSC);

	// total order
	ret += htonl(total_order.size());
	for(li = total_order.begin(); li != total_order.end(); li++)
		ret += htonl(*li);

	// message-process-matching
	ret += htonl(msg_process_match.size());
	for(li = msg_process_match.begin(); li != msg_process_match.end(); li++)
		ret += htonl(*li);

	// message-buffer-matching
	ret += htonl(msg_buffer_match.size());
	for(li = msg_buffer_match.begin(); li != msg_buffer_match.end(); li++)
		ret += htonl(*li);

	// max queue length
	ret += htonl(max_queue_length);

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

	// get max queue length
	size--; it++; if(size <= 0 || limit == it) goto deserialization_failed;
	max_queue_length = ntohl(*it);

	size--; it++;

	if(size == 0)
		return true;

deserialization_failed:
	clear();
	max_queue_length = 0;
	return false;
}}}

bool normalizer_msc::deserialize_extension(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
	return false;
}


list<int> normalizer_msc::prefix_normal_form(list<int> w, bool &bottom)
{
	// first create a MSC from the word

	// FIXME
	bottom = false;
	return w;
}

list<int> normalizer_msc::suffix_normal_form(list<int> w, bool &bottom)
{
	// FIXME
	bottom = false;
	return w;
}

}; // end namespace libalf

