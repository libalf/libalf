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
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_algorithm_mvcl_angluinlike_h__
# define __libalf_algorithm_mvcl_angluinlike_h__

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <ostream>
#include <sstream>

#include <stdio.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <libalf/alphabet.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>

#include <libalf/triple.h>

namespace libalf {

using namespace std;

template <class answer>
class mVCL_angluinlike : public learning_algorithm<answer> {
	public: // types

		class equivalence_approximation : public triple<list<int>, int, vector<answer> > {
			// where first == prefix, second == cv(prefix), third == acceptances.
			public: // methods
				list<int>	& prefix()		{ return this->first; };
				int		& cv()			{ return this->second; };
				vector<answer>	& acceptances()		{ return this->third; };


				bool equivalent(equivalence_approximation & other)
				{ return this->first == other.first && this->second == other.second; };

				bool fill(list<list<int> > & samples, knowledgebase<answer> * base)
				{{{
					list<int> query_word;

					typename vector<answer>::iterator acci;
					typename list<list<int> >::iterator sui;

					for(acci = acceptances().begin(), sui = samples.begin(); acci != acceptances().end() && sui != samples.end(); ++acci, ++sui)
						/* nothing */ ;

					while(sui != samples.end()) {
						answer a;
						query_word = prefix() + *sui;
						if(base->resolve_or_add_query(query_word, a))
							acceptances().push_back(a);
						else
							break;
						++sui;
					}

					return (sui == samples.end());
				}}}

		};

		class equivalence_table : public list<equivalence_approximation> {
			public:
				bool fill(list<list<int> > & samples, knowledgebase<answer> * base)
				{{{
					typename list<equivalence_approximation>::iterator equi;
					bool complete = true;

					for(equi = this->begin(); equi != this->end(); ++equi)
						if(!equi->fill(samples, base))
							complete = false;;

					return complete;
				}}}
		};

		class m_representatives : public pair<list<list<int> >, triple<equivalence_table, equivalence_table, equivalence_table> > {
			// where first == returning, second == internal and third == calling.
			public: // methods
				list<list<int> > & samples()		{ return this->first; };
				equivalence_table & returning()		{ return this->second.first; };
				equivalence_table & internal()		{ return this->second.second; };
				equivalence_table & calling()		{ return this->second.third; };

		};

		class stratified_observationtable : public vector<m_representatives> {
			public: // methods
				bool fill(knowledgebase<answer> * base)
				{{{
					bool complete = true;
					typename vector<m_representatives>::iterator previous, current, next;

					// initialize iterators
					previous = this->end();
					current = this->begin();
					next = this->begin();
					if(next != this->end())
						++next;

					while(current != this->end()) {
						// fill all tables of current
						if(previous != this->end())
							if(!current->returning().fill(previous->samples(), base))
								complete = false;
						if(!current->internal().fill(current->samples(), base))
							complete = false;
						if(next != this->end())
							if(!current->calling().fill(next->samples(), base))
								complete = false;

						// increment iterators
						if(previous == this->end())
							previous = this->begin();
						else
							++previous;
						++current;
						if(next != this->end())
							++next;
					}

					return complete;
				}}}
		};

	protected: // data
		bool initialized;

		map<int, int> pushdown_directions; // maps a label to its pushdown direction:
		// -1 == down == return ; 0 == stay == internal ; +1 == up == call

		int known_equivalence_bound; // the m for which the current data is isomorphic to the model

		stratified_observationtable table;

	public: // methods
		mVCL_angluinlike()
		{{{
			this->set_logger(NULL);
			this->set_knowledge_source(NULL);
			clear();
		}}}

		mVCL_angluinlike(knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			this->set_logger(log);
			this->set_knowledge_source(base);
			clear();
			this->set_alphabet_size(alphabet_size);
		}}}

		void clear()
		{{{
			initialized = false;
			this->set_alphabet_size(0);
			known_equivalence_bound = -1;
			pushdown_directions.clear();
			table.clear();
		}}}

		virtual void indicate_pushdown_alphabet_directions(const map<int, int> & directions)
		{ pushdown_directions = directions; }

		virtual void increase_alphabet_size(int new_asize)
		{ this->set_alphabet_size(new_asize); }

		virtual memory_statistics get_memory_statistics()
		{
			memory_statistics ret;

			

			return ret;
		}

		virtual void receive_generic_statistics(generic_statistics & stat)
		{
			
		}

		virtual bool sync_to_knowledgebase()
		{{{
			(*this->my_logger)(LOGGER_ERROR, "mVCL_angluinlike does not support sync-operation.\n");
			return false;
		}}}
		virtual bool supports_sync()
		{ return false; };

		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;

			ret += 0; // size, filled in later.
			ret += ::serialize(initialized);
			ret += ::serialize(this->get_alphabet_size());
			ret += ::serialize(pushdown_directions);
			ret += ::serialize(known_equivalence_bound);
			ret += ::serialize(table);

			ret[0] = htonl(ret.length() - 1);

			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			int size;

			clear();

			serial_stretch serial;
			serial.current = it;
			serial.limit = limit;

			if(!::deserialize(size, serial)) goto deserialization_failed;
			// total size: we don't care.
			if(!::deserialize(initialized, serial)) goto deserialization_failed;
			if(!::deserialize(size, serial)) goto deserialization_failed;
			this->set_alphabet_size(size);
			if(!::deserialize(pushdown_directions, serial)) goto deserialization_failed;
			if(!::deserialize(known_equivalence_bound, serial)) goto deserialization_failed;
			if(!::deserialize(table, serial)) goto deserialization_failed;

			it = serial.current;
			return true;
deserialization_failed:
			clear();
			it = serial.current;
			return false;
		}}}

		bool deserialize_magic(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit, basic_string<int32_t> & result)
		{
			// FIXME: setting pushdown-directions of alphabet via this.
			
			return false;
		}

		virtual void print(ostream &os)
		{
			
		}
		virtual string to_string()
		{
			string ret;
			
			return ret;
		}

		virtual bool conjecture_ready()
		{
			// what kind of conjecture?
			
		}

		virtual bool indicate_partial_equivalence(int bound)
		{
			if(bound < known_equivalence_bound) {
				(*this->my_logger)(LOGGER_ERROR, "mVCL_angluinlike: you indicated a partial equivalence with m=%d, but equivalence was already indicated for m=%d!", bound, known_equivalence_bound);
				return false;
			} else {
				known_equivalence_bound = bound;
				return true;
			}
		}

		virtual bool add_counterexample(list<int> counterexample)
		{
			
		}

	protected: // methods
		int prefix_countervalue(list<int>::const_iterator word, list<int>::const_iterator limit, int initial_countervalue = 0)
		{{{
			for(/* nothing */; word != limit; ++word) {
				if(*word < 0 || *word >= this->get_alphabet_size()) {
					initial_countervalue = -1;
					break;
				}
				initial_countervalue += pushdown_directions[*word];
				if(initial_countervalue < 0)
					break;
			}
			return initial_countervalue;
		}}}

		int countervalue(const list<int> & word)
		{ return this->prefix_countervalue(word.begin(), word.end(), 0); }

		virtual void initialize_table()
		{
			
		}

		virtual bool fill_missing_columns()
		{ return table.fill(this->my_knowledge); }

		virtual bool close()
		{
			
		}

		virtual bool make_consistent()
		{
			
		}

		virtual bool complete()
		{
			if(!this->initialized)
				this->initialize_table();

			if(this->fill_missing_columns()) {
				if(!this->close())
					return complete();

				return true;
			} else {
				return false;
			}
		}

		virtual conjecture * derive_conjecture()
		{
			
		}
};


}; // end of namespace libalf

#endif

