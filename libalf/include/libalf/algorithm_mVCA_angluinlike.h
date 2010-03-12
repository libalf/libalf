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
class mVCA_angluinlike : public learning_algorithm<answer> {
	public: // types

		class equivalence_approximation : public triple<list<int>, int, vector<answer> > {
			public: // methods
				inline list<int>	& prefix()		{ return this->first; };
				inline int		& cv()			{ return this->second; };
				inline vector<answer>	& acceptances()		{ return this->third; };

				inline equivalence_approximation()
				{ };

				inline equivalence_approximation(const list<int> & prefix, int cv)
				{ this->prefix() = prefix; this->cv() = cv; };

				inline bool equivalent(equivalence_approximation & other)
				{ return cv() == other.cv() && acceptances() == other.acceptances(); }

				bool fill(list<list<int> > & samples, knowledgebase<answer> * base)
				{{{
					typename vector<answer>::iterator acci;
					typename list<list<int> >::iterator sui;

					for(acci = acceptances().begin(), sui = samples.begin(); acci != acceptances().end() && sui != samples.end(); ++acci, ++sui)
						/* nothing */ ;

					if(sui == samples.end())
						return true;

					typename knowledgebase<answer>::node *pre, *word;

					pre = base->get_nodeptr(prefix());

					bool full_query = true;
					while(sui != samples.end()) {
						word = pre->find_or_create_descendant(sui->begin(), sui->end());
						if(full_query) {
							if(word->is_answered()) {
								acceptances().push_back(word->get_answer());
							} else {
								word->mark_required();
								full_query = false;
							}
						} else {
							word->mark_required();
						}
						++sui;
					}

					return full_query;
				}}}

				void print(ostream &os)
				{{{
					typename vector<answer>::iterator acci;

					os << "\t     |\t";
					print_word(os, prefix());
					os << " (" << cv() << "):";
					for(acci = acceptances().begin(); acci != acceptances().end(); ++acci) {
						if(false == (bool)*acci)
							os << " -";
						else if(true == (bool)*acci)
							os << " +";
						else
							os << " " << *acci;
					}
					os << " ;\n";
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

				void print(ostream & os)
				{{{
					typename list<equivalence_approximation>::iterator it;
					for(it = this->begin(); it != this->end(); ++it) {
						it->print(os);
					}
				}}}
		};

		class m_representatives : public pair<list<list<int> >, triple<equivalence_table, equivalence_table, equivalence_table> > {
			public: // methods
				inline list<list<int> > & samples()		{ return this->first; };
				inline equivalence_table & returning()		{ return this->second.first; };
				inline equivalence_table & internal()		{ return this->second.second; };
				inline equivalence_table & calling()		{ return this->second.third; };
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

				void print(ostream & os)
				{{{
					int cv;
					typename vector<m_representatives>::iterator vi;

					os << "stratified_observationtable {\n";

					for(vi = this->begin(), cv = 0; vi != this->end(); ++cv, ++vi) {
						// samples
						os << "\tsamples for cv = " << cv << ":";
						for(list<list<int> >::iterator si = vi->samples().begin(); si != vi->samples().end(); ++si) {
							os << " ";
							print_word(os, *si);
						}
						os << " ;\n";

						// returning
						os << "\tcv = " << cv << ", returning representatives (with true cv=" << cv - 1 << "):\n";
						vi->returning().print(os);

						// internal
						os << "\tcv = " << cv << ", internal representatives:\n";
						vi->internal().print(os);

						// calling
						os << "\tcv = " << cv << ", calling representatives (with true cv=" << cv + 1 << "):\n";
						vi->calling().print(os);
					}

					os << "};\n";
				}}}
		};

	protected: // data
		bool initialized;

		map<int, int> pushdown_directions; // maps a label to its pushdown direction:
		// -1 == down == return ; 0 == stay == internal ; +1 == up == call

		int known_equivalence_bound; // the m for which the current data is isomorphic to the model

		stratified_observationtable table;

	public: // methods
		mVCA_angluinlike()
		{{{
			this->set_logger(NULL);
			this->set_knowledge_source(NULL);
			clear();
		}}}

		mVCA_angluinlike(knowledgebase<answer> *base, logger *log, int alphabet_size)
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
		// get_memory_statistics() is obsolete and will be removed in the future.
		// use receive_generic_statistics() instead.
		{{{
			generic_statistics stat;
			memory_statistics ret;

			receive_generic_statistics(stat);

			ret.bytes = stat["bytes"];
			ret.words = stat["words"];

			return ret;
		}}}

		virtual void receive_generic_statistics(generic_statistics & stat)
		{
			int words = 0; // words that are stored in the tables
			int bytes = 0; // bytes this algorithm consumes over all

			// FIXME
			

			stat["words"] = words;
			stat["bytes"] = bytes;
		}

		virtual bool sync_to_knowledgebase()
		{{{
			(*this->my_logger)(LOGGER_ERROR, "mVCA_angluinlike does not support sync-operation.\n");
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
		virtual bool deserialize(serial_stretch & serial)
		{{{
			int size;

			clear();

			if(!::deserialize(size, serial)) goto deserialization_failed;
			// total size: we don't care.
			if(!::deserialize(initialized, serial)) goto deserialization_failed;
			if(!::deserialize(size, serial)) goto deserialization_failed;
			this->set_alphabet_size(size);
			if(!::deserialize(pushdown_directions, serial)) goto deserialization_failed;
			if(!::deserialize(known_equivalence_bound, serial)) goto deserialization_failed;
			if(!::deserialize(table, serial)) goto deserialization_failed;

			return true;
deserialization_failed:
			clear();
			return false;
		}}}

		bool deserialize_magic(serial_stretch & serial, basic_string<int32_t> & result)
		{{{
			int command;

			result.clear();

			if(!::deserialize(command, serial)) return false;

			switch(command) {
				case 0: { // indicate pushdown property of alphabet
						map<int, int> dirs;
						if(!::deserialize(dirs, serial)) return false;
						pushdown_directions = dirs;
						return true;
					}
				case 1: { // indicate partial equivalence
						int bound;
						if(!::deserialize(bound, serial)) return false;
						result += ::serialize(indicate_partial_equivalence(bound));
						return true;
					}
			};
			// bad command?
			return false;
		}}}

		virtual void print(ostream &os)
		{{{
			table.print(os);
		}}}
		virtual string to_string()
		{{{
			stringstream str;
			print(str);
			return str.str();
		}}}

		virtual bool conjecture_ready()
		// TODO: implement *efficient* version
		{{{
			if(!complete())
				return false;

			conjecture * cj = derive_conjecture();
			if(NULL == cj)
				return false;
			delete cj;
			return true;
		}}}

		virtual bool indicate_partial_equivalence(int bound)
		{
			if(bound < known_equivalence_bound) {
				(*this->my_logger)(LOGGER_ERROR, "mVCA_angluinlike: you indicated a partial equivalence with m=%d, but equivalence was already indicated for m=%d!", bound, known_equivalence_bound);
				return false;
			} else {
				known_equivalence_bound = bound;
				return true;
			}
		}

		virtual bool add_counterexample(list<int> counterexample)
		{
			// check if word is already in table, then return false.
			

			return true; // FIXME
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
		{{{
			if(!initialized) {
				m_representatives mr;
				table.push_back(mr);
				table[0].samples().push_back(list<int>());
				table[0].internal().push_back(equivalence_approximation(list<int>(), 0));

				initialized = true;
			}
		}}}

		virtual bool fill_missing_columns()
		{ return table.fill(this->my_knowledge); }

		virtual bool close()
		{
			bool no_changes = true;

			

			return no_changes; // FIXME
		}

		virtual bool make_consistent()
		{
			bool no_changes = true;

			

			return no_changes; // FIXME
		}

		virtual bool complete()
		{{{
			if(!initialized)
				initialize_table();

			if(!fill_missing_columns())
				return false;

			if(!close())
				return complete();

			if(!make_consistent())
				return complete();

			return true;
		}}}

		virtual conjecture * derive_conjecture()
		{
			conjecture * cj = NULL;

			

			return cj; // FIXME
		}
};


}; // end of namespace libalf

#endif

