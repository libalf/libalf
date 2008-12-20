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

#include <libalf/normalizer.h>

namespace libalf {

using namespace std;

msc_normalizer::msc_normalizer(list<int> &total_order, list<int> &msg_process_match, list<int> &msg_buffer_match, int max_queue_length)
{
}

list<int> msc_normalizer::prefix_normal_form(list<int> w)
{
}

list<int> msc_normalizer::suffix_normal_form(list<int> w)
{
}

basic_string<int32_t> msc_normalizer::serialize()
{
}

bool msc_normalizer::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
}

bool msc_normalizer::deserialize_extension(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
}

}; // end namespace libalf

