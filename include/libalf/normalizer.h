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

#ifndef __libalf_normalizer_h__
# define __libalf_normalizer_h__

#include <list>
#include <string>

namespace libalf {

using namespace std;

class normalizer {
	public:
		enum type {
			NORMALIZER_NONE = 0,
			NORMALIZER_MSC = 1
		};

		virtual ~normalizer() { };

		virtual basic_string<int32_t> serialize() = 0;
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		virtual bool deserialize_extension(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		virtual list<int> prefix_normal_form(list<int> w, bool &bottom) = 0;
		virtual list<int> suffix_normal_form(list<int> w, bool &bottom) = 0;
		// if the normalizer finds that the word is not in the MSCs language, bottom will be set to true.
		// that way, an learning algorithm can automatically set the words row to bottom.
};


// normalizer for learning protocols from message-sequence-charts
class normalizer_msc : public normalizer {
	private:
		// total order of all messages
		list<int> total_order;
		// in total_order, each successive pair of elements
		// belongs together in the form that the first is the send-event
		// of the sending process, the second is the receive-event
		// of the receiving process.

		// relation matching an event to a process
		list<int> msg_process_match;

		// relation matching an event to a queue
		list<int> msg_buffer_match;

		// max number of messages in a queue
		int max_queue_length;
		// if <= 0, max queue length will not be checked.
	public:
		normalizer_msc();
		normalizer_msc(list<int> &total_order, list<int> &msg_process_match, list<int> &msg_buffer_match, int max_queue_length);

		virtual ~normalizer_msc() { };

		void clear();

		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

		virtual bool deserialize_extension(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

		virtual list<int> prefix_normal_form(list<int> w, bool &bottom);
		virtual list<int> suffix_normal_form(list<int> w, bool &bottom);
};

}; // end of namespace libalf

#endif // __libalf_normalizer_h__

