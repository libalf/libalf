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
 * (c) 2008,2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *                and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_algorithm_NLstar_h__
# define __libalf_algorithm_NLstar_h__

#include <list>
#include <vector>
#include <string>
#include <ostream>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <libalf/answer.h>
#include <libalf/learning_algorithm.h>
#include <libalf/serialize.h>

namespace libalf {

using namespace std;

// NLstar, angluin-style learning algorithm for RFSA-automata,
// as described in
//	* B. Bollig, P. Habermehl, C. Kern, M. Leucker: "Angluin-Style Learning of NFA"
//	  http://www.lsv.ens-cachan.fr/Publis/RAPPORTS_LSV/PDF/rr-lsv-2008-28.pdf

template <class answer>
class NLstar_table : public learning_algorithm<answer> {
	public:
		typedef list< list<int> > columnlist;

		class table_row {
			public:
				typedef vector<answer> acceptances;
			public:
				list<int> index;
				acceptances acceptance;
			public:
				bool __attribute__((const)) covers(table_row & other)
				{{{
					typename acceptances::iterator ai1, ai2;
					for(ai1 = this->acceptance.begin(), ai2 = other.acceptance.begin(); ai1 != this->acceptance.end() && ai2 != other.acceptance.end(); ai1++, ai2++)
						if(*ai1 == false && *ai2 == true)
							return false;

					return ( ai1 == this->acceptance.end() && ai2 == other.acceptance.end() );
				}}}
				bool __attribute__((const)) mutual_noncover(table_row & other)
				{{{
					typename acceptances::iterator ai1, ai2;
					bool tNCo = false, oNCt = false;
					for(ai1 = this->acceptance.begin(), ai2 = other.acceptance.begin(); ai1 != this->acceptance.end() && ai2 != other.acceptance.end(); ai1++, ai2++) {
						if(*ai1 == false && *ai2 == true) {
							if(oNCt)
								return true;
							else
								tNCo = true;
						}
						if(*ai1 == true && *ai2 == false) {
							if(tNCo)
								return true;
							else
								oNCt = true;
						}
					}

					if( ai1 == this->acceptance.end() && ai2 == other.acceptance.end() )
						return tNCo && oNCt;
					else
						return false;
				}}}
				bool __attribute__((const)) operator==(table_row & other)
				{{{
					typename acceptances::iterator ai1, ai2;
					for(ai1 = this->acceptance.begin(), ai2 = other.acceptance.begin(); ai1 != this->acceptance.end() && ai2 != other.acceptance.end(); ai1++, ai2++)
						if(*ai1 != *ai2)
							return false;

					return ( ai1 == this->acceptance.end() && ai2 == other.acceptance.end() );
				}}}
				bool __attribute__((const)) operator!=(table_row & other)
				{{{
					return ( ! (*this == other) );
				}}}

				// join other row into this row
				void operator|=(table_row & other)
				{{{
					typename acceptances::iterator ai1, ai2;
					for(ai1 = this->acceptance.begin(), ai2 = other.acceptance.begin(); ai1 != this->acceptance.end() && ai2 != other.acceptance.end(); ai1++, ai2++)
						if(*ai2 == true)
							*ai1 = true;
				}}}

				basic_string<int32_t> serialize()
				{{{
					return ::serialize(index) + ::serialize(acceptance);
				}}}
				bool deserialize(serial_stretch & serial)
				{{{
					if(!::deserialize(index, serial)) goto fail;
					if(!::deserialize(acceptance, serial)) goto fail;
					return true;
fail:
					index.clear();
					acceptance.clear();
					return false;

				}}}

				string to_string()
				{{{
					string s;
					typename acceptances::iterator acci;

					s = word2string(index);
					s += ": ";
					for(acci = acceptance.begin(); acci != acceptance.end(); acci++) {
						if(*acci == true)
							s += "+ ";
						else
							if(*acci == false)
								s += "- ";
							else
								s += "? ";
					}
					s += ";\n";

					return s;
				}}}
				size_t memory_usage()
				{{{
					size_t s;
					list< list<int> >::iterator acci;

					s = sizeof(list<int>) + sizeof(int) * index.size();
					s += sizeof(acceptances) + sizeof(answer) * acceptance.size();

					return s;
				}}}
		};

		typedef list<table_row> table;

	protected:
		columnlist column_names;

		table upper_table;
		table lower_table;

		bool initialized;

	public:
		NLstar_table(knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(base);
			initialized = false;
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			// add new suffixes for all words in upper table
			if(new_asize < this->get_alphabet_size())
				return;

			typename table::iterator uti;
			table_row row;

			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				row.index = uti->index;

				for(int new_suffix = this->get_alphabet_size(); new_suffix < new_asize; new_suffix++) {
					row.index.push_back(new_suffix);
					this->lower_table.push_back(row);
					row.index.pop_back();
				}

				this->set_alphabet_size(new_asize);
			}
		}}}

		virtual memory_statistics get_memory_statistics()
		{{{
			memory_statistics ret;
			// get memory footprint:
			typename columnlist::iterator ci;
			typename table::iterator ti;

			ret.bytes = sizeof(this);
			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				ret.bytes += sizeof(list<int>) + sizeof(int) * (ci->size());
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				ret.bytes += ti->memory_usage();
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++)
				ret.bytes += ti->memory_usage();
			// members
			ret.columns = column_names.size();
			ret.upper_table = upper_table.size();
			ret.lower_table = lower_table.size();
			ret.members = ret.columns * (ret.upper_table + ret.lower_table);
			ret.words = ret.members;
			return ret;
		}}}

		virtual void receive_generic_statistics(generic_statistics & stat)
		{{{
			int bytes = 0;
			int c, ut, lt;
			// get memory footprint:
			typename columnlist::iterator ci;
			typename table::iterator ti;

			c = column_names.size();
			ut = upper_table.size();
			lt = lower_table.size();

			stat["columns"] = c;
			stat["upper_table"] = ut;
			stat["lower_table"] = lt;
			stat["words"] = c*(ut+lt);

			bytes = sizeof(this);
			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				bytes += sizeof(list<int>) + sizeof(int) * (ci->size());
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				bytes += ti->memory_usage();
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++)
				bytes += ti->memory_usage();

			stat["bytes"] = bytes;
		}}}

		virtual bool sync_to_knowledgebase()
		{{{
			(*this->my_logger)(LOGGER_ERROR, "NL* does not support sync-operation (undo) for now.\n");
			return false;
		}}}
		virtual bool supports_sync()
		{{{
			return false; // not for now.
		}}}

		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;

			ret += 0; // size - filled in later.
			ret += htonl(learning_algorithm<answer>::ALG_NL_STAR);
			ret += htonl(this->get_alphabet_size());

			ret += ::serialize(column_names);
			ret += serialize_table(upper_table);
			ret += serialize_table(lower_table);
			// end of tables.
			ret += ::serialize(initialized ? 1 : 0);

			ret[0] = htonl(ret.length() - 1);

			return ret;
		}}}
		virtual bool deserialize(serial_stretch & serial)
		{{{
			int size;
			int type;

			if(!::deserialize(size, serial)) goto deserialization_failed;
			if(size < 1) goto deserialization_failed;
			if(!::deserialize(type, serial)) goto deserialization_failed;
			if(type != learning_algorithm<answer>::ALG_NL_STAR) goto deserialization_failed;
			if(!::deserialize(size, serial)) goto deserialization_failed;
			this->set_alphabet_size(size);

			if(!::deserialize(column_names, serial)) goto deserialization_failed;

			if(!deserialize_table(upper_table, serial)) goto deserialization_failed;
			if(!deserialize_table(lower_table, serial)) goto deserialization_failed;

			if(!::deserialize(size, serial)) goto deserialization_failed;
			initialized = size;

			return true;

deserialization_failed:
			this->set_alphabet_size(0);
			column_names.clear();
			upper_table.clear();
			lower_table.clear();
			initialized = false;
			return false;
		}}}

		virtual void print(ostream &os)
		{{{
			os << this->to_string();
		}}}

		virtual string to_string()
		{{{
			string s;
			typename columnlist::iterator ci;
			typename table::iterator ti;

			s = "NL* table {\n";
			s += "\tcolumns:";
			for(ci = column_names.begin(); ci != column_names.end(); ci++) {
				s += " ";
				s += word2string(*ci);
			};
			s += " ;\n\tupper table:\n";
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++) {
				s += "\t\t";
				s += ti->to_string();
			}
			s += "\n\tlower table:\n";
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++) {
				s += "\t\t";
				s += ti->to_string();
			}
			s += "}\n";

			return s;
		}}}

		virtual bool conjecture_ready()
		{{{
			return initialized && columns_filled() && is_closed() && is_consistent();
		}}}

		virtual bool add_counterexample(list<int> w)
		{{{
			if(this->my_knowledge == NULL) {
				(*this->my_logger)(LOGGER_ERROR, "NLstar_table: add_counterexample() without knowledgebase!\n");
				return false;
			}
			// add counterexample and all its suffixes to the columns
			int sigma = -1;
			unsigned int wsize = w.size();
			while(!w.empty()) { // epsilon is always in table
				if(!add_column(w)) {
					if(wsize ==  w.size()) {
						string s;
						s = word2string(w);
						(*this->my_logger)(LOGGER_ERROR, "NLstar_table: add_counterexample(): you are trying to add a counterexample (%s) which is already contained in the table. trying to ignore...\n", s.c_str());
						return false;
					}
					// if a prefix is already in, we dont need
					// to check the other. column_names is prefix-closed.
					break;
				}
				if(w.front() > sigma)
					sigma = w.front();
				w.pop_front();
			}
			if(sigma+1 > this->get_alphabet_size()) {
				(*this->my_logger)(LOGGER_ALGORITHM, "NLstar_table: add_counterexample(): implicit increase of alphabet_size from %d to %d.\n", this->get_alphabet_size(), sigma+1);
				increase_alphabet_size(sigma+1);
			}

			return true;
		}}}

	protected:
		// returns true if word was added, false if it was already in column_names
		virtual bool add_column(list<int> word)
		{{{
			typename columnlist::reverse_iterator ci;

			// going reversed may abort faster, as the longer suffixes are at the back
			for(ci = column_names.rbegin(); ci != column_names.rend(); ci++)
				if(*ci == word)
					return false;

			column_names.push_back(word);
			return true;
		}}}

		virtual bool row_is_prime(typename table::iterator & row)
		{{{
//			string s,t;
//			s = word2string(row->index);
//			(*this->my_logger)(LOGGER_DEBUG, "\tchecking if %s is prime:\n", s.c_str());
			table_row merge;
			int cn = column_names.size();
			int i;
			bool joined = false; // avoid taking a pure "-" row as non prime

			// initialize merge-row
			answer a_false;
			a_false = false;
			for(i = 0; i < cn; i++)
				merge.acceptance.push_back(a_false);

			// join all covered rows into merge-row
			typename table::iterator ti;
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				// NEVER EVER join a row r that is equal to row!
				if(ti != row && *ti != *row)
					if(row->covers(*ti)) {
						merge |= *ti;
						joined = true;
//						s = ti->to_string(); t = merge.to_string(); //						(*this->my_logger)(LOGGER_DEBUG, "\t\tjoining upper %s\t\t\t=> %s", s.c_str(), t.c_str());
					}

			// quick check if we are done
			if(joined && (merge == *row))
				return false;

			for(ti = lower_table.begin(); ti != lower_table.end(); ti++)
				// NEVER EVER join a row r that is equal to row!
				if(ti != row && *ti != *row)
					if(row->covers(*ti)) {
						joined = true;
						merge |= *ti;
//						s = ti->to_string(); t = merge.to_string();
//						(*this->my_logger)(LOGGER_DEBUG, "\t\tjoining lower %s\t\t\t=> %s", s.c_str(), t.c_str());
					}

			// if they are equal now, *row is composed from other rows
			return (! (joined && (merge == *row)));
		}}}

		virtual void add_word_to_upper_table(list<int> & word, bool check_uniq = true)
		{{{
			table_row row;
			bool done = false;

			if(check_uniq) {
				if(search_upper_table(word) != upper_table.end())
					return;

				typename table::iterator ti;
				ti = search_lower_table(word);
				if(ti != lower_table.end()) {
					// word is already in lower. so move it up.
					done = true;
					upper_table.push_back(*ti);
					// we have to do this before erase(), because word may be a ref from a row we are about to delete.
					row.index = word;
					lower_table.erase(ti);
				}
			} else {
				// don't move this out of else, otherwise it may conflict with deleted reference
				row.index = word;
			}

			if(!done)
				upper_table.push_back(row);

			// add all suffixes to lower table
//			for(int sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
			for(int sigma = this->get_alphabet_size()-1; sigma >= 0; sigma--) {
				row.index.push_back(sigma);

				if(check_uniq) {
					if(search_upper_table(row.index) == upper_table.end())
						lower_table.push_back(row);
				} else {
					lower_table.push_back(row);
				}

				row.index.pop_back();
			}
		}}}

		virtual typename table::iterator search_upper_table(list<int> &word)
		{{{
			typename table::iterator uti;

			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				if(word == uti->index)
					return uti;
			}
			return upper_table.end();
		}}}

		virtual typename table::iterator search_lower_table(list<int> &word)
		{{{
			typename table::iterator lti;

			for(lti = lower_table.begin(); lti != lower_table.end(); lti++)
				if(word == lti->index)
					return lti;
			return lower_table.end();
		}}}

		virtual typename table::iterator search_tables(list<int> &word)
		{{{
			typename table::iterator it;

			it = search_upper_table(word);
			if(it != upper_table.end())
				return it;

			return search_lower_table(word);
		}}}

		virtual void initialize_table()
		{{{
			list<int> word; // empty word;

			column_names.clear();
			upper_table.clear();
			lower_table.clear();

			column_names.push_back(word);
			add_word_to_upper_table(word);

			initialized = true;
		}}}

		// return true if all columns could be filled,
		// false if new knowledge is required (in case of knowledgebase)
		virtual bool fill_missing_columns(table & t)
		{{{
			typename table::iterator ti;
			bool complete = true;

			// upper table
			for(ti = t.begin(); ti != t.end(); ti++) {
				if(ti->acceptance.size() < column_names.size()) {
					if(!ti->index.empty() && ti->index.front() == BOTTOM_CHAR) {
						int delta = column_names.size() - ti->acceptance.size();
						answer a;
						a = false;
						for(/* -- */; delta > 0; delta--)
							ti->acceptance.push_back(a);
					} else {
						// fill in missing acceptance information
						columnlist::iterator ci;
						ci = column_names.begin();
						for(int i = ti->acceptance.size(); i > 0; i--)
							ci++;
						bool column_skipped = false;
						for(/* -- */; ci != column_names.end(); ci++) {
							if(!ci->empty() && ci->front() == BOTTOM_CHAR) {
								answer a;
								a = false;
								ti->acceptance.push_back(a);
							} else {
								list<int> *w;
								w = concat(ti->index, *ci);
								answer a;
								if(this->my_knowledge->resolve_or_add_query(*w, a)) {
									if(!column_skipped)
										ti->acceptance.push_back(a);
								} else {
									column_skipped = true;
									complete = false;
								}

								delete w;
							}
						}
					}
				}
			}

			return complete;
		}}}

		virtual bool columns_filled()
		{{{
			typename table::iterator ti;
			unsigned int columns;

			columns = column_names.size();

			// check upper table
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				if(ti->acceptance.size() != columns)
					return false;
			// check lower table
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++)
				if(ti->acceptance.size() != columns)
					return false;

			return true;
		}}}

		// table is closed if for all rows R in lower table, R is composed from upper primes
		// that are covered by R
		virtual bool is_closed()
		{{{
			typename table::iterator ti;
			list<typename table::iterator> upper_primes;
			// prepare merge row
			int i;
			int cn = column_names.size();
			bool first = true;
			table_row merge;
			answer a_false;

			// so first find all upper primes
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				if(row_is_prime(ti))
					upper_primes.push_back(ti);

			first = true;
			a_false = false;
			for(i = 0; i < cn; i++)
				merge.acceptance.push_back(a_false);

			// now check closed-ness of lower table
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++) {
				// reset merge row
				if(first)
					first = false;
				else
					for(i = 0; i < cn; i++)
						merge.acceptance[i] = a_false;

				bool joined = false;
				typename list<typename table::iterator>::iterator pri;
				for(pri = upper_primes.begin(); pri != upper_primes.end(); pri++)
					if(ti->covers(**pri)) {
						merge |= **pri;
						joined = true;
					}

				if(!joined || merge != *ti) {
					// this row is not composed from upper primes!
					return false;
				}
			}

			return true;
		}}}

		// close table
		// returns true if table was closed.
		// returns false if table was changed (and thus needs to be filled)
		virtual bool close()
		{{{
//			string s,t;
//			(*this->my_logger)(LOGGER_DEBUG, "close() getting upper primes:\n");
			typename table::iterator ti;
			list<typename table::iterator> upper_primes;
			// prepare merge row
			int i;
			int cn = column_names.size();
			bool first = true;
			table_row merge;
			answer a_false;

			// first find all upper primes
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				if(row_is_prime(ti)) {
					upper_primes.push_back(ti);
//					s = word2string(ti->index);
//					(*this->my_logger)(LOGGER_DEBUG, "\t%s is upper prime.\n", s.c_str());
				}

			first = true;
			a_false = false;
			for(i = 0; i < cn; i++)
				merge.acceptance.push_back(a_false);

//			(*this->my_logger)(LOGGER_DEBUG, "close() finding non-prime-covered rows in lower table:\n");
			// now check closed-ness of lower table
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++) {
//				s = word2string(ti->index);
//				(*this->my_logger)(LOGGER_DEBUG, "\tchecking %s :\n", s.c_str());
				// reset merge row
				if(first)
					first = false;
				else
					for(i = 0; i < cn; i++)
						merge.acceptance[i] = a_false;

				bool joined = false;
				typename list<typename table::iterator>::iterator pri;
				for(pri = upper_primes.begin(); pri != upper_primes.end(); pri++)
					if(ti->covers(**pri)) {
						merge |= **pri;
						joined = true;
//						s = (*pri)->to_string(); t = merge.to_string();
//						(*this->my_logger)(LOGGER_DEBUG, "\t\tjoining upper %s\t\t\t=> %s", s.c_str(), t.c_str());
					}

				if(!joined || merge != *ti) {
					// this row is not composed from upper primes!
					// find a non-covered prime in lower and move it up:

					for(ti = lower_table.begin(); ti != lower_table.end(); ti++) {
						if(row_is_prime(ti)) {
							bool already_in_upper = false;
							for(pri = upper_primes.begin(); pri != upper_primes.end(); pri++) {
								if(**pri == *ti) {
									already_in_upper = true;
									break;
								}
							}
							if(!already_in_upper) {
//								s = word2string(ti->index);
//								(*this->my_logger)(LOGGER_DEBUG, "    moving word %s to upper to close table.\n", s.c_str());
								add_word_to_upper_table(ti->index);
								return false;

							}
						}
					}
				}
			}

			return true;
		}}}

		virtual bool w1suffixes_cover_w2suffixes(list<int> w1, list<int> w2)
		{{{
			for(int sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
				typename table::iterator suffix1, suffix2;

				w1.push_back(sigma);
				suffix1 = search_tables(w1);
				w1.pop_back();

				w2.push_back(sigma);
				suffix2 = search_tables(w2);
				w2.pop_back();

				if( ! suffix1->covers(*suffix2))
					return false;
			}
			return true;
		}}}

		// table is consistent if for all upper rows with index u, v:
		// u covers v -> for all N in Sigma: u.N covers v.N
		virtual bool is_consistent()
		{{{
			typename table::iterator uti1, uti2;

			for(uti1 = upper_table.begin(); uti1 != upper_table.end(); uti1++) {
				uti2 = uti1;
				uti2++;
				for(/* nothing */; uti2 != upper_table.end(); uti2++) {
					if(uti1->covers(*uti2) && !w1suffixes_cover_w2suffixes(uti1->index, uti2->index))
						return false;
					if(uti2->covers(*uti1) && !w1suffixes_cover_w2suffixes(uti2->index, uti1->index))
						return false;
				}
			}

			return true;
		}}}

		virtual bool let_w1suffixes_cover_w2suffixes(list<int> w1, list<int> w2)
		// returns false if something was changed
		{{{
			for(int sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
				typename table::iterator suffix1, suffix2;

				w1.push_back(sigma);
				suffix1 = search_tables(w1);
				w1.pop_back();

				w2.push_back(sigma);
				suffix2 = search_tables(w2);
				w2.pop_back();

				typename table_row::acceptances::iterator ai1, ai2;
				columnlist::iterator ci;
				for(ai1 = suffix1->acceptance.begin(), ai2 = suffix2->acceptance.begin(), ci = column_names.begin();
				    ai1 != suffix1->acceptance.end(); ++ai1, ++ai2, ++ci) {
					if(*ai1 == false && *ai2 == true) {
						list<int> new_suffix;
						new_suffix = *ci;
						new_suffix.push_front(sigma);
						add_column(new_suffix);
						return false;
					}
				}
			}
			return true;
		}}}

		// make table consistent
		// returns true if table was consistent.
		// returns false if table was changed (and thus needs to be filled)
		virtual bool make_consistent()
		{{{
			typename table::iterator uti1, uti2;

			for(uti1 = upper_table.begin(); uti1 != upper_table.end(); uti1++) {
				uti2 = uti1;
				uti2++;
				for(/* nothing */; uti2 != upper_table.end(); uti2++) {
					if(uti1->covers(*uti2) && !let_w1suffixes_cover_w2suffixes(uti1->index, uti2->index))
						return false;
					if(uti2->covers(*uti1) && !let_w1suffixes_cover_w2suffixes(uti2->index, uti1->index))
						return false;
				}
			}

			return true;
		}}}

		virtual bool complete()
		{{{
			if(!initialized)
				initialize_table();

			if(fill_missing_columns(upper_table) && fill_missing_columns(lower_table)) {
//				string s = this->to_string();
//				(*this->my_logger)(LOGGER_DEBUG, "----------------------- \n%s", s.c_str());
				if(!close())
					return complete();

				if(!make_consistent())
					return complete();

				return true;
			} else {
				return false;
			}
		}}}

		// derive an automaton and return it
		virtual conjecture * derive_conjecture()
		{{{
			simple_automaton *ret = new simple_automaton;

			// NFA is (Q, Q0, F, delta)
			// with
			//	Q =		upper primes
			//	Q0 =		{ r \in Q | r is covered by epsilon }
			//			NOTE: epsilon is not in Q0 iff. epsilon is not prime (iff. epsilon not in Q)
			//	F =		{ r \in Q | .r.epsilon. = + }
			//	delta(u, a) =	{ r \in Q | r is covered by .u.a. for all u in Q }
			vector<typename table::iterator> upper_primes;
			typename table::iterator ti;
			typename table::iterator epsilon = upper_table.begin();

			// get all distinguished states
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				if(row_is_prime(ti))
					upper_primes.push_back(ti);

			for(unsigned int i = 0; i < upper_primes.size(); i++) {
				list<int> succ_w;
				typename table::iterator successor;

				// get initial states
				if(epsilon->covers(*upper_primes[i]))
					ret->initial.insert(i);

				// get final states (column 0 is always epsilon)
				if(upper_primes[i]->acceptance.front() == true)
					ret->final.insert(i);

				// and all transitions from this state
				pair<int, int> trid;
				trid.first = i;
				succ_w = upper_primes[i]->index;
				for(unsigned int sigma = 0; (int)sigma < this->get_alphabet_size(); sigma++) {
					succ_w.push_back(sigma);
					successor = search_tables(succ_w);
					succ_w.pop_back();

					trid.second = sigma;

					for(unsigned int dst = 0; dst < upper_primes.size(); dst++)
						if(successor->covers(*upper_primes[dst]))
							ret->transitions.insert( pair<pair<int, int>, int>( trid, dst) );

				}
			}

			ret->is_deterministic = false;
			ret->alphabet_size = this->get_alphabet_size();
			ret->state_count = upper_primes.size();
			ret->valid = true;

			return ret;
		}}}

		basic_string<int32_t> serialize_table(table & t)
		{{{
			std::basic_string<int32_t> serialized_list;
			typename table::iterator ti;
			int size = 0;

			for(ti = t.begin(); ti != t.end(); ++ti) {
				serialized_list += ti->serialize();
				++size;
			}

			return ::serialize(size) + serialized_list;

		}}}
		bool deserialize_table(table & t, serial_stretch & serial)
		{{{
			int size;

			if(!::deserialize(size, serial)) return false;
			while(size) {
				table_row tmp;
				if(!tmp.deserialize(serial)) return false;
				t.push_back(tmp);
				--size;
			}
			return true;
		}}}
};

}; // end of namespace libalf

#endif // __libalf_algorithm_NLstar_h__

