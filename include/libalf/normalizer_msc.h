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

#ifndef __libalf_normalizer_msc_h__
# define __libalf_normalizer_msc_h__

#include <list>
#include <vector>
#include <string>

#include <libalf/normalizer.h>

namespace libalf {

using namespace std;

namespace msc {

	class msc_node {
		public:
			// position in word
			int id;
			// label (word[id])
			int label;
			// edge according to process_match
			msc_node * process_in;
			msc_node * process_out;
			// edge according to buffer_match
			msc_node * buffer_in;
			msc_node * buffer_out;

			msc_node();

			~msc_node();

			inline void connect_process(msc_node & other)
			{{{
				other.process_in = this;
				this->process_out = &other;
			}}}

			inline bool is_process_connected()
			{{{
				return (process_out == NULL);
			}}}

			inline bool is_process_referenced()
			{{{
				return (process_in == NULL);
			}}}

			inline void connect_buffer(msc_node & other)
			{{{
				other.buffer_in = this;
				this->buffer_out = &other;
			}}}

			inline bool is_buffer_connected()
			{{{
				return (buffer_out == NULL);
			}}}

			inline bool is_buffer_referenced()
			{{{
				return (buffer_in == NULL);
			}}}
	};

};

// normalizer for learning protocols from message-sequence-charts
class normalizer_msc : public normalizer {
	private:
		// besides the other attributes, each successive elements (below)
		// of the alphabet belong together in that the first (the odd) is the
		// send-event of the sending process, the second (the even) is
		// the receive-event of the receiving process.
		// thus we can split into send(odd)/receive(even) events via a test for (n%1)

		// total order of all messages
		vector<int> total_order;

		// relation matching an event to a process
		vector<int> process_match;

		// relation matching an event to a buffer
		vector<int> buffer_match;

		// max number of messages in a buffer
		int max_buffer_length;
		// if <= 0, max buffer length will not be checked.
	public:
		normalizer_msc();
		normalizer_msc(vector<int> &total_order, vector<int> &process_match, vector<int> &buffer_match, int max_buffer_length);

		virtual ~normalizer_msc() { };

		void clear();

		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

		virtual bool deserialize_extension(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

		virtual list<int> prefix_normal_form(list<int> & w, bool &bottom);
		virtual list<int> suffix_normal_form(list<int> & w, bool &bottom);



	private:
		list<msc::msc_node> graph;
	protected:
		bool graph_add_node(int id, int label);
		int graph_reduce();
};

}; // end of namespace libalf

#endif // __libalf_normalizer_msc_h__

