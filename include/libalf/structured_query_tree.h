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

#ifndef __libalf_structured_query_tree_h__
# define __libalf_structured_query_tree_h__

namespace libalf {

#include <list>
#include <string>
#include <arpa/inet.h>

#include <libalf/answer.h>
#include <libalf/alphabet.h>

using namespace std;

template <class answer>
class structured_query_tree {
	private:

	public:
		virtual basic_string<int32_t> serialize()
		{
		}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
		}
		virtual void print()
		{
		}
		virtual void add_query(list<int> word, int prefix_count)
		{
		}
		virtual void clear()
		{
		}
}


} // end of namespace libalf

#endif // __libalf_structured_query_tree_h__

