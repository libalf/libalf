/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_normalizer_msc_h__
# define __libalf_normalizer_msc_h__

#include <list>
#include <vector>
#include <string>
#include <queue>

#include <sys/types.h>

#include <libalf/normalizer.h>

namespace libalf {

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

			inline void connect_process(msc_node * other)
			{{{
				other->process_in = this;
				this->process_out = other;
			}}}

			inline bool is_process_connected()
			{{{
				return (process_out != NULL);
			}}}

			inline bool is_process_referenced()
			{{{
				return (process_in != NULL);
			}}}

			inline void connect_buffer(msc_node * other)
			{{{
				other->buffer_in = this;
				this->buffer_out = other;
			}}}

			inline bool is_buffer_connected()
			{{{
				return (buffer_out != NULL);
			}}}

			inline bool is_buffer_referenced()
			{{{
				return (buffer_in != NULL);
			}}}

			inline void disconnect()
			{{{
				if(process_in) {
					process_in->process_out = NULL;
					process_in = NULL;
				}
				if(process_out) {
					process_out->process_in = NULL;
					process_out = NULL;
				}
				if(buffer_in) {
					buffer_in->buffer_out = NULL;
					buffer_in = NULL;
				}
				if(buffer_out) {
					buffer_out->buffer_in = NULL;
					buffer_out = NULL;
				}
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
		std::vector<int> total_order;

		// relation matching an event to a process
		std::vector<int> process_match;

		// relation matching an event to a buffer
		std::vector<int> buffer_match;

		// max number of messages in a buffer
		int max_buffer_length;
		// if <= 0, max buffer length will not be checked.
	public:
		normalizer_msc();
		normalizer_msc(std::vector<int> &total_order, std::vector<int> &process_match, std::vector<int> &buffer_match, int max_buffer_length);

		virtual ~normalizer_msc();

		virtual enum type get_type() const
		{ return NORMALIZER_MSC; };

		void clear();

		virtual std::basic_string<int32_t> serialize() const;
		virtual bool deserialize(serial_stretch & serial);

		virtual bool deserialize_extension(serial_stretch & serial);

		virtual std::list<int> prefix_normal_form(const std::list<int> & w, bool & bottom) const;
		virtual std::list<int> suffix_normal_form(const std::list<int> & w, bool & bottom) const;


	private:
		mutable std::list<msc::msc_node*> graph;
		mutable std::queue<int> * buffers;
		mutable unsigned int buffercount;
		// any label that can be normalized has to be in [0, label_bound)
		mutable unsigned int label_bound;

	protected:
		void graph_add_node(int id, int label, bool pnf) const;

		void clear_buffers(const std::list<int> & word) const;
		bool check_buffer(int label, bool pnf) const;
		void advance_buffer_status(int label, bool pnf) const;

		int graph_reduce(bool pnf) const;

		void graph_print() const;
};

}; // end of namespace libalf

#endif // __libalf_normalizer_msc_h__

