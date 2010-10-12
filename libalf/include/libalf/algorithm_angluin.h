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
 *                and David R. Piegdon
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_algorithm_angluin_h__
# define __libalf_algorithm_angluin_h__

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <ostream>
#include <sstream>
#include <typeinfo>

#include <stdio.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <libalf/alphabet.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>

namespace libalf {


/* NOTE
 *
 * For Dana Angluins L* algorithm for learning regular languages, there exist
 * a lot of extensions. libalf provides the following algorithms based on L*:
 *
 *	- classical L* (table based)
 *		in this file, class angluin_simple_table
 *	- L* putting counterexamples into the columns
 *		in this file, class angluin_col_table
 *	- Rivest&Shapire extension that analyses the counterexample to find an optimal split
 *		in algorithm_rivest_shapire.h, class rivest_shapire_table
 *
 *	all the above do not just support learning of finite_automata, but of abstract
 *	moore_machine<answer> types. if you choose bool, the conjectures will be of type
 *	finite_automaton (which are derived from moore_machine<bool> and carry some
 *	extra methods), otherwise the conjectures will be of type moore_machine<answer>.
 *
 *	- L*-like learning of visible m-bounded 1-counter automata
 *		in algorithm_mVCA_angluinlike.h, class mVCA_angluinlike;
 *		this for now only supports learning of simple mVCAs.
 */



using namespace std;

	namespace algorithm_angluin {
		template <class table>
		class automaton_state {
			public:
				int id;
				typename table::iterator tableentry;
		};
	};

/*
 * template-table for angluin learning algorithm
 * NEVER use this direclty. if you need an alguin-style learning algorithm,
 * please stick to those declared below this template.
 */
template <class answer, class table, class acceptances>
class angluin_table : public learning_algorithm<answer> {
	/*\
	 * NOTES FOR <TABLE> AND <ACCEPTANCE> TEMPLATE CLASSES:
	 *	table has to be iterable. iterator elements have to provide the following:
	 *	table::iterator->operator==(*(table::iterator) &)
	 *		has to return true, if acceptance of both table entries is same
	 *	table::iterator->acceptance has to be a container with acceptance information,
	 *		in the same order as column_names. following functionality is required:
	 *		- has to be iterable, and thus to provide begin() and end()
	 *		- type of *(acceptance.begin()) has to be <answer>
	 *		- has to be indicable: e.g. it->acceptance[5]
	 *		- has to provide push_back(answer a): e.g. it->acceptance.push_back(true)
	 *		- has to provide front(): e.g. answer b = it->acceptance.front()
	 *		- has to provide swap(*(table::iterator).acceptance, *(table::iterator).acceptance)
	 *	table::iterator->ut_timestamp has to be an int
	 *	table::iterator->lt_timestamp has to be an int
	 *	basic_string<int32_t> table::iterator->serialize()
	 *	bool table::iterator->deserialize(basic_string<int32_t>::iterator it, basic_string<int32_t>::iterator limit);
	 *	(see implementation notes on serialization members)
	 *
	 *	note: acceptance can e.g. be a member vector<answer> of the *(table::iterator) type
	\*/

	public: // types
		typedef vector< list<int> > columnlist;
	protected: // data
		columnlist column_names;
		list<unsigned int> column_timestamps;

		table upper_table;
		table lower_table;

		bool initialized;

	public: // methods
		angluin_table()
		{{{
			this->set_knowledge_source(NULL);
			this->set_logger(NULL);
			this->set_normalizer(NULL);
			this->set_alphabet_size(0);
			initialized = false;
		}}}

		virtual bool sync_to_knowledgebase()
		{{{
			if(this->my_knowledge == NULL) {
				(*this->my_logger)(LOGGER_WARN, "angluin_table: sync_to_knowledgebase is only supported in combination with a knowledgebase!\n");
				return false;
			}

			if(initialized) {
				// we will check all knowledge back with the knowledgebase.
				bool ret = true;
				if(!sync_columns())
					ret = false;
				if(!sync_tables())
					ret = false;

				return ret;
			} else {
				(*this->my_logger)(LOGGER_WARN, "angluin_table: sync_to_knowledgebase: trying to sync a non-initialized table!\n");
				return false;
			}
		}}}

		virtual bool supports_sync() const
		{ return true; }


		virtual basic_string<int32_t> serialize() const
		{{{
			basic_string<int32_t> ret;
			basic_string<int32_t> temp;
			typename table::const_iterator ti;
			typename columnlist::const_iterator ci;
			typename list<unsigned int>::const_iterator tsi;

			// length (filled in later)
			ret += 0;

			// implementation type
			ret += htonl(learning_algorithm<answer>::ALG_ANGLUIN);

			// initialized
			ret += ::serialize(initialized);

			// alphabet size
			ret += htonl(this->get_alphabet_size());

			// column list
			ret += htonl(column_names.size());
			for(ci = column_names.begin(), tsi = column_timestamps.begin();
					ci != column_names.end() && tsi != column_timestamps.end();
					ci++, tsi++) {
				// column label
				ret += ::serialize(*ci);
				// column timestamp
				ret += htonl(*tsi);
			}

			// upper table
			ret += htonl(upper_table.size());
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				ret += ti->serialize();

			// lower table
			ret += htonl(lower_table.size());
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++)
				ret += ti->serialize();

			ret[0] = htonl(ret.length() - 1);

			return ret;
		}}}

		// (implementation specific)
		virtual bool deserialize(serial_stretch & serial) = 0;

		virtual void print(ostream &os) const
		{{{
			typename columnlist::const_iterator ci;
			typename list<unsigned int>::const_iterator tsi;
			typename table::const_iterator ti;
			typename acceptances::const_iterator acci;
			char buf[32];

			os << "angluin_table {\n";
			os << "\tcolumns:";
			for(ci = column_names.begin(), tsi = column_timestamps.begin(); ci != column_names.end() && tsi != column_timestamps.end(); ci++, tsi++) {
				os << " ";
				print_word(os, *ci);
				snprintf(buf, 32, " (%d)", *tsi);
				buf[31] = 0;
				os << buf;
			}
			os << " ;\n";

			os << "\tupper table:\n";
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++) {
				os << "\t\t";

				snprintf(buf, 32, "(u%03d,l%03d) ", ti->ut_timestamp, ti->lt_timestamp);
				buf[31] = 0;
				os << buf;

				print_word(os, ti->index);
				os << ": ";
				for(acci = ti->acceptance.begin(); acci != ti->acceptance.end(); acci++) {
					os << *acci << " ";
				}
				os << ";\n";
			}

			os << "\tlower_table:\n";
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++) {
				os << "\t\t";

				snprintf(buf, 32, "(u%03d,l%03d) ", ti->ut_timestamp, ti->lt_timestamp);
				buf[31] = 0;
				os << buf;

				print_word(os, ti->index);
				os << ": ";
				for(acci = ti->acceptance.begin(); acci != ti->acceptance.end(); acci++) {
					os << *acci << " ";
				}
				os << ";\n";
			}

			os << "}\n";
		}}}

		virtual bool conjecture_ready()
		{{{
			return initialized && columns_filled() && is_closed() && is_consistent();
		}}}

		virtual bool add_counterexample(list<int> word)
		{{{
#ifdef DEBUG_ANGLUIN
			(*this->my_logger)(LOGGER_DEBUG, "angluin_table: new counterexample %s.\n", word2string(word).c_str());
#endif
			typename table::const_iterator ti;
			list<int>::const_iterator wi;

			if(this->my_knowledge == NULL) {
				(*this->my_logger)(LOGGER_ERROR, "angluin_table: add_counterexample() without knowledgebase!\n");
				return false;
			}

			ti = search_tables(word);
			if(ti != lower_table.end()) {
				if(!this->norm) {
					(*this->my_logger)(LOGGER_ERROR, "angluin_table: add_counterexample(): you are trying to add a counterexample (%s) "
							"which is already contained in the table. trying to ignore...\n", word2string(word).c_str());
					return false;
				} else {
					(*this->my_logger)(LOGGER_DEBUG, "angluin_table: add_counterexample(): you are trying to add a counterexample (%s) "
							"which is already contained in the table. as you are using a normalizer, it is possible that some "
							"prefixes of it are missing in the table. so i'll check the prefixes anyway. NOTE: this most likely "
							"happens if you added non-normalized counter-examples (which is allowed).\n", word2string(word).c_str());
					// fall through and try anyway.
				}
			}

			// check for increase in alphabet size
			int new_asize = this->get_alphabet_size();
			bool asize_changed = false;
			for(wi = word.begin(); wi != word.end(); wi++) {
				if(*wi >= new_asize) {
					new_asize = *wi+1;
					asize_changed = true;
				}
			}
			if(asize_changed) {
				(*this->my_logger)(LOGGER_ALGORITHM, "angluin_table: add_counterexample(): implicit increase of alphabet_size "
						"from %d to %d.\nNOTE: it is possible that the next hypothesis does not increase in state-count.\n",
						this->get_alphabet_size(), new_asize);
				increase_alphabet_size(new_asize);
			}

			// add word and all prefixes to upper table
			while(!word.empty()) {
				add_word_to_upper_table(word);
				word.pop_back();
			}

			return true;
		}}}

		virtual list< list<int> > *get_columns()
		{{{
			list< list<int> > *l = new list< list<int> >();
			typename columnlist::iterator ci;

			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				l->push_back(*ci);

			return l;
		}}}

		virtual void increase_alphabet_size(int new_asize) = 0;

	protected:
		virtual void initialize_table() = 0;
		virtual void add_word_to_upper_table(list<int> word, bool check_uniq = true) = 0;

		// this expects a NORMALIZED word!
		virtual typename table::iterator search_upper_table(const list<int> &word)
		{{{
			typename table::iterator uti;

			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				if(word == uti->index)
					return uti;
			}
			return upper_table.end();
		}}}

		// this expects a NORMALIZED word!
		virtual typename table::iterator search_upper_table(const list<int> &word, int &index)
		{{{
			typename table::iterator uti;
			index = 0;

			for(uti = upper_table.begin(); uti != upper_table.end(); uti++, index++) {
				if(word == uti->index)
					return uti;
			}
			index = -1;
			return upper_table.end();
		}}}

		// this expects a NORMALIZED word!
		virtual typename table::iterator search_lower_table(const list<int> &word)
		{{{
			typename table::iterator lti;

			for(lti = lower_table.begin(); lti != lower_table.end(); lti++)
				if(word == lti->index)
					return lti;
			return lower_table.end();
		}}}

		// this expects a NORMALIZED word!
		virtual typename table::iterator search_lower_table(const list<int> &word, int &index)
		{{{
			typename table::iterator lti;
			index = 0;

			for(lti = lower_table.begin(); lti != lower_table.end(); lti++, index++)
				if(word == lti->index)
					return lti;
			index = -1;
			return lower_table.end();
		}}}

		// this expects a NORMALIZED word!
		virtual typename table::iterator search_tables(const list<int> &word)
		{{{
			typename table::iterator it;

			it = search_upper_table(word);
			if(it != upper_table.end())
				return it;

			return search_lower_table(word);
		}}}

		virtual bool add_column(const list<int> & word)
		// returns true if column was added,
		// false if column was there earlier
		{{{
			typename columnlist::iterator ci;
			list<int> nw;

			bool bottom;
			if(this->norm) {
				nw = this->norm->suffix_normal_form(word, bottom);
				if(bottom)
					(*this->my_logger)(LOGGER_ERROR, "angluin_table: add_column() just tried to add a bottom-column (%s). most likely, your normalizer and your teacher are not well-aligned. trying to ignore...\n", word2string(word).c_str());
			} else {
				nw = word;
			}

			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				if(*ci == nw)
					return false;

			if(this->my_knowledge != NULL)
				column_timestamps.push_back(this->my_knowledge->get_timestamp());
			else
				column_timestamps.push_back(0);

			column_names.push_back(nw);
			return true;
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
						ci += ti->acceptance.size();
						bool column_skipped = false;
						for(/* -- */; ci != column_names.end(); ci++) {
							if(!ci->empty() && ci->front() == BOTTOM_CHAR) {
								answer a;
								a = false;
//printf("query for %s::%s: BOTTOM\n", word2string(ti->index).c_str(), word2string(*ci).c_str());
								ti->acceptance.push_back(a);
							} else {
								list<int> *w;
								w = concat(ti->index, *ci);
								answer a;
								if(this->my_knowledge->resolve_or_add_query(*w, a)) {
									if(!column_skipped)
										ti->acceptance.push_back(a);
//printf("query for %s: %d\n", word2string(*w).c_str(), (int)a);
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

		virtual bool sync_columns()
		{{{
			// remove column-names that are too young.
			// later in sync_tables their entries in each row will be deleted.

			bool timestamp_ok = false;

			while(!timestamp_ok) {
				if(column_timestamps.size() > 0) {
					// FIXME: <= or < ?
					if(column_timestamps.back() < this->my_knowledge->get_timestamp()) {
						timestamp_ok = true;
					} else {
						column_names.pop_back();
						column_timestamps.pop_back();
					}
				} else {
					(*this->my_logger)(LOGGER_WARN, "angluin_table: sync_to_knowledgebase: columns are empty after sync! resetting table.\n");
					upper_table.clear();
					lower_table.clear();
					initialized = false;
					return false;
				}
			}
			return true;
		}}}

		virtual bool sync_tables()
		{{{
			typename table::iterator uti, lti;
			unsigned int now;

			now = this->my_knowledge->get_timestamp();

			uti = upper_table.begin();
			while(uti != upper_table.end()) {
				if(uti->ut_timestamp >= now) {
					// if required, move entry down
					if(uti->lt_timestamp > 0 && uti->lt_timestamp < now) {
						uti->lt_timestamp = 0;
						lower_table.push_back(*uti);
					}

					// remove entry
					upper_table.erase(uti);

					uti = upper_table.begin();
				} else {
					uti++;
				}
			}

			lti = lower_table.begin();
			while(lti != lower_table.end()) {
				if(lti->lt_timestamp >= now) {
					// remove entry
					lower_table.erase(lti);

					lti = lower_table.begin();
				} else {
					lti++;
				}
			}

			if(upper_table.size() == 0 || lower_table.size() == 0) {
				(*this->my_logger)(LOGGER_WARN, "angluin_table: sync_to_knowledgebase: rows are empty after sync! resetting table.\n");
				upper_table.clear();
				lower_table.clear();
				initialized = false;
				return false;
			}

			sync_table_acceptances(upper_table);
			sync_table_acceptances(lower_table);

			return true;
		}}}

		virtual void sync_table_acceptances(table & t)
		{{{
			typename columnlist::iterator ci;
			typename acceptances::iterator ai;
			typename table::iterator ti;

			for(ti = t.begin(); ti != t.end(); ti++) {
				unsigned int col;

				// sync all knowledge
				for(ci = column_names.begin(),  ai = ti->acceptance.begin(), col = 0;
				    ci != column_names.end() && ai != ti->acceptance.end();
				    ci++, ai++, col++) {
					list<int> *w;
					answer a;
					w = concat(ti->index, *ci);

					if(this->my_knowledge->resolve_query(*w, a)) {
						delete w;

						if(*ai != a) {
							(*this->my_logger)(LOGGER_WARN, "angluin_table: undo: acceptances differ between revisions!\n");
							*ai = a;
						}
					} else {
						delete w;

						// remove anything behind this one, including this one
						while(ti->acceptance.size() > col)
							ti->acceptance.pop_back();

						break;
					}
				}

				// remove remainder at end of acceptances, due to deleted column_names
				if(ai != ti->acceptance.end())
					while( ti->acceptance.size() > column_names.size())
						ti->acceptance.pop_back();
			}
		}}}

		//  all existing answer-rows in
		//  lower table already exist in upper table
		//  (for angluin)
		virtual bool is_closed()
		{{{
			typename table::iterator uti, lti;
			for(lti = lower_table.begin(); lti != lower_table.end(); lti++) {
				bool match_found = false;

				for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
					if(*lti == *uti) {
						match_found = true;
						break;
					}
				}
				if(!match_found)
					return false;
			}
			return true;
		}}}

		// close table: perform operations to close it.
		// returns true if table was closed.
		// returns false if table was changed (and thus needs to be filled)
		virtual bool close()
		{{{
			typename table::iterator uti, lti, tmplti;

			for(lti = lower_table.begin(); lti != lower_table.end(); lti++) {
				bool match_found = false;

				for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
					if(*lti == *uti) {
						match_found = true;
						break;
					}
				}
				if(!match_found) {
#ifdef DEBUG_ANGLUIN
					(*this->my_logger)(LOGGER_DEBUG, "angluin_table: closing: moving %s up.\n", word2string(lti->index).c_str());
#endif
					// create entry in upper table
					add_word_to_upper_table(lti->index, false);
					return false;
				}
			}

			return true;
		}}}

		//  for all _equal_ rows in upper table: all +1 successors over all
		//  members of alphabet have to have equal rows
		virtual bool is_consistent()
		{{{
			bool urow_ok[upper_table.size()];
			typename table::iterator uti_1, uti_2, ut_last_row;
			unsigned int i,j;

			for(i = 0; i < upper_table.size(); i++)
				urow_ok[i] = false;

			ut_last_row = upper_table.end();
			ut_last_row--;

			for(i = 0, uti_1 = upper_table.begin(); uti_1 != ut_last_row; i++, uti_1++) {
				if(urow_ok[i])
					continue;
				urow_ok[i] = true;
				uti_2 = uti_1;
				uti_2++;
				for(j=i+1; uti_2 != upper_table.end(); uti_2++, j++) {
					if(urow_ok[j])
						continue;
					if(*uti_1 == *uti_2) {
						// uti_1->acceptance == uti_2->acceptance
						// -> test if all equal suffixes result in equal acceptance as well
						list<int> word1 = uti_1->index;
						list<int> word2 = uti_2->index;
						typename table::iterator w1succ, w2succ;
						int sigma;
						for(sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
							if(word1.front() != BOTTOM_CHAR)
								word1.push_back(sigma);
							if(word2.front() != BOTTOM_CHAR)
								word2.push_back(sigma);

							if(this->norm) {
								bool bottom;
								list<int> w1n, w2n;
								w1n = this->norm->prefix_normal_form(word1, bottom);
								w2n = this->norm->prefix_normal_form(word2, bottom);
								w1succ = search_tables(w1n);
								w2succ = search_tables(w2n);
							} else {
								w1succ = search_tables(word1);
								w2succ = search_tables(word2);
							}

							if(word1.front() != BOTTOM_CHAR)
								word1.pop_back();
							if(word2.front() != BOTTOM_CHAR)
								word2.pop_back();

							if(*w1succ != *w2succ)
								return false;
						}
					}
				}
			}
			return true;
		}}}

		// make table consistent: perform operations to do that.
		// returns true if table was consistent.
		// returns false if table was changed (and thus needs to be filled)
		virtual bool make_consistent()
		{{{
			bool urow_ok[upper_table.size()];
			typename table::iterator uti_1, uti_2, ut_last_row;
			unsigned int i,j;

			for(i = 0; i < upper_table.size(); i++)
				urow_ok[i] = false;

			ut_last_row = upper_table.end();
			ut_last_row--;

			for(i = 0, uti_1 = upper_table.begin(); uti_1 != ut_last_row; i++, uti_1++) {
				if(urow_ok[i])
					continue;
				urow_ok[i] = true;
				uti_2 = uti_1;
				uti_2++;
				for(j=i+1; uti_2 != upper_table.end(); uti_2++, j++) {
					if(urow_ok[j])
						continue;
					if(*uti_1 == *uti_2) {
						// uti_1->acceptance == uti_2->acceptance
						// -> test if all equal suffixes result in equal acceptance as well
						list<int> word1 = uti_1->index;
						list<int> word2 = uti_2->index;
						typename table::iterator w1_succ, w2_succ;
						for(int sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
							if(word1.front() != BOTTOM_CHAR)
								word1.push_back(sigma);
							if(word2.front() != BOTTOM_CHAR)
								word2.push_back(sigma);
							if(this->norm) {
								bool bottom;
								list<int> w1n, w2n;
								w1n = this->norm->prefix_normal_form(word1, bottom);
								w2n = this->norm->prefix_normal_form(word2, bottom);
								w1_succ = search_tables(w1n);
								w2_succ = search_tables(w2n);
							} else {
								w1_succ = search_tables(word1);
								w2_succ = search_tables(word2);
							}
							if(word1.front() != BOTTOM_CHAR)
								word1.pop_back();
							if(word2.front() != BOTTOM_CHAR)
								word2.pop_back();

							if(*w1_succ != *w2_succ) {
								if(w1_succ->acceptance.size() == w2_succ->acceptance.size()) {
									// add suffixes resulting in different states to column_names
									typename columnlist::iterator ci;
									int cindex = 0;
									typename acceptances::iterator w1_acc_it, w2_acc_it;

									ci = column_names.begin();
									w1_acc_it = w1_succ->acceptance.begin();
									w2_acc_it = w2_succ->acceptance.begin();

									while(w1_acc_it != w1_succ->acceptance.end()) {
										if(*w1_acc_it != *w2_acc_it) {
											list<int> newsuffix;
											// generate and add suffix
											newsuffix = *ci;
											newsuffix.push_front(sigma);
											if(add_column(newsuffix)) {
#ifdef DEBUG_ANGLUIN
												(*this->my_logger)(LOGGER_DEBUG, "angluin_table: making consistent: new suffix %s to separate %s and %s\n",
														word2string(newsuffix).c_str(),
														word2string(word1).c_str(),
														word2string(word2).c_str()
															);
#else
												/* nothing */
#endif
											} else {
												(*this->my_logger)(LOGGER_ERROR, "angluin_table: adding column %s in make_consistent(), "
																"but it already existed! trying to ignore.\n",
																word2string(newsuffix).c_str());
											}
											ci = column_names.begin();

											return false;
										}
										w1_acc_it++;
										w2_acc_it++;
										ci++;
										cindex++;
									}
								}
							}
						}
					}
				}
			}

			return true;
		}}}

		// complete table, so an automaton can be derived
		// returns true, if automaton may be derived.
		// returns false if new knowledge is required (in case of knowledgebase).
		virtual bool complete()
		{{{
			if(!initialized)
				initialize_table();

			if(fill_missing_columns(upper_table) && fill_missing_columns(lower_table)) {
#ifdef DEBUG_ANGLUIN
				(*this->my_logger)(LOGGER_DEBUG, "angluin_table: contents of table:\n%s\n", this->to_string().c_str());
#endif
				if(!close())
					return complete();

				if(!make_consistent())
					return complete();

#ifdef DEBUG_ANGLUIN
				(*this->my_logger)(LOGGER_DEBUG, "angluin_table: hypothesis ready.\n");
#endif
				return true;
			} else {
				return false;
			}
		}}}


		// derive an automaton and return it
		virtual conjecture * derive_conjecture()
		{{{
			conjecture * cj;
			list<algorithm_angluin::automaton_state<table> > states;

			cj = this->derive_conjecture_memorize(states);

			return cj;
		}}}

		// derive an automaton and return it, such that a table<->automaton mapping is stored in states
		virtual libalf::moore_machine<answer> * derive_conjecture_memorize(list<algorithm_angluin::automaton_state<table> > & states)
		{{{
			libalf::moore_machine<answer> * ret;

			if(typeid(answer) == typeid(bool))
				ret = dynamic_cast<libalf::moore_machine<answer>*>(new libalf::finite_automaton);
			else
				ret = new libalf::moore_machine<answer>;

			// derive deterministic finite automaton from this table
			typename table::iterator uti, ti;

			algorithm_angluin::automaton_state<table> state;
			states.clear();
			state.id = 0;
			typename list<algorithm_angluin::automaton_state<table> >::iterator state_it, state_it2;

			// list of states of automaton: each different acceptance-row
			// in the upper table represents one DFA state
#ifdef DEBUG_ANGLUIN
			(*this->my_logger)(LOGGER_DEBUG, "angluin_table: state/row assignment for hypothesis:\n");
#endif
			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				bool known = false;
				for(state_it = states.begin(); state_it != states.end(); state_it++) {
					if(*uti == *(state_it->tableentry)) {
						// state is already known. skip.
						known = true;
						break;
					}
				}
				if(known)
					continue;

				state.tableentry = uti;

				states.push_back(state);

#ifdef DEBUG_ANGLUIN
				(*this->my_logger)(LOGGER_DEBUG, "angluin_table:    state %d is row %s\n", state.id, word2string(state.tableentry->index).c_str());
#endif

				state.id++;
			}

			// q0 is row(epsilon)
			// as epsilon is the first row in uti, it will have id 0.
			ret->initial_states.insert( 0 );

			for(state_it = states.begin(); state_it != states.end(); state_it++) {
				// the final, accepting states are the rows with
				// acceptance in the epsilon-column
				ret->output_mapping[state_it->id] = state_it->tableentry->acceptance.front();

				// the transformation function is:
				// \delta: (row, char) -> row : (row(s), a) -> row(sa)
				list<int> index;
				typename table::iterator ri;
				index = state_it->tableentry->index;
				for(int i = 0; i < this->get_alphabet_size(); i++) {
					// find successor in table
					index.push_back(i);
					if(this->norm) {
						bool bottom;
						list<int> nw;
						nw = this->norm->prefix_normal_form(index, bottom);
						ti = search_tables(nw);
					} else {
						ti = search_tables(index);
					}
					index.pop_back();

					// find matching state for successor
					for(state_it2 = states.begin(); state_it2 != states.end(); state_it2++) {
						if(*ti == *(state_it2->tableentry)) {
							ret->transitions[state_it->id][i].insert(state_it2->id);
							break;
						}
					}
				}
			}

			ret->input_alphabet_size = this->get_alphabet_size();
			ret->state_count = states.size();
			ret->is_deterministic = true;

			ret->valid = true;

			return ret;
		}}};

};

	namespace algorithm_angluin {
		template <class answer, class acceptances>
		class simple_row {
			public:
				list<int> index;
				acceptances acceptance;
				unsigned int ut_timestamp;
				unsigned int lt_timestamp;

				bool __attribute__((const)) operator==(const simple_row<answer, acceptances> &other) const
				{{{
					return (acceptance == other.acceptance);
				}}}

				bool __attribute__((const)) operator!=(const simple_row<answer, acceptances> &other) const
				{{{
					return (acceptance != other.acceptance);
				}}}

				bool __attribute__((const)) operator>(const simple_row<answer, acceptances> &other) const
				{{{
					typename acceptances::const_iterator ai;
					typename acceptances::const_iterator oai;

					ai = acceptance.begin();
					oai = other.acceptance.begin();

					for(/* -- */; ai < acceptance.end() && oai < other.acceptance.end(); ai++, oai++) {
						if(*ai > *oai)
							return true;
					}

					return false;
				}}}

				basic_string<int32_t> serialize() const
				{{{
					basic_string<int32_t> ret;

					ret += ::serialize(index);
					ret += ::serialize(ut_timestamp);
					ret += ::serialize(lt_timestamp);
					ret += ::serialize(acceptance);

					return ret;
				}}}

				bool deserialize(serial_stretch & serial)
				{{{
					if(!::deserialize(index, serial)) goto deserialization_failed;
					if(!::deserialize(ut_timestamp, serial)) goto deserialization_failed;
					if(!::deserialize(lt_timestamp, serial)) goto deserialization_failed;
					if(!::deserialize(acceptance, serial)) goto deserialization_failed;

					return true;

				deserialization_failed:
					index.clear();
					acceptance.clear();
					return false;
				}}}

		};
	};




// this is the classical table-base angluin learning algorithm, L*,
// implemented in a table-fashion: prefix-closed rows and suffix-closed columns
template <class answer>
class angluin_simple_table : public angluin_table<answer, list< algorithm_angluin::simple_row<answer, vector<answer> > >, vector<answer> > {
	public:
		angluin_simple_table()
		{{{
			this->set_alphabet_size(0);
			this->set_knowledge_source(NULL);
			this->set_logger(NULL);
		}}}
		angluin_simple_table(knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(base);
		}}}

		virtual memory_statistics get_memory_statistics() const
		// get_memory_statistics() is obsolete and will be removed in the future.
		// use receive_generic_statistics() instead.
		{{{
			memory_statistics ret;

			typename angluin_table<answer, list< algorithm_angluin::simple_row<answer, vector<answer> > >, vector<answer> >::columnlist::const_iterator ci;
			typename list< algorithm_angluin::simple_row<answer, vector<answer> > >::const_iterator ti;

			ret.columns = this->column_names.size();
			ret.upper_table = this->upper_table.size();
			ret.lower_table = this->lower_table.size();

			ret.members = ret.columns * ( ret.upper_table + ret.lower_table );
			ret.words = ret.members;

			// approx. memory usage:
			ret.bytes = sizeof(*this);
			// columns
			ret.bytes += sizeof(vector<int>);
			for(ci = this->column_names.begin(); ci != this->column_names.end(); ci++)
				ret.bytes += sizeof(int) * ci->size() + sizeof(list<int>);
			// upper table bare rows
			for(ti = this->upper_table.begin(); ti != this->upper_table.end(); ti++)
				ret.bytes += sizeof(algorithm_angluin::simple_row<answer, vector<answer> >) + sizeof(int) * ti->index.size();
			// lower table bare rows
			for(ti = this->lower_table.begin(); ti != this->lower_table.end(); ti++)
				ret.bytes += sizeof(algorithm_angluin::simple_row<answer, vector<answer> >) + sizeof(int) * ti->index.size();
			// table fields
			ret.bytes += sizeof(answer) * ret.members;

			return ret;
		}}}

		virtual void receive_generic_statistics(generic_statistics & stat) const
		{{{
			int c, ut, lt, bytes;

			typename angluin_table<answer, list< algorithm_angluin::simple_row<answer, vector<answer> > >, vector<answer> >::columnlist::const_iterator ci;
			typename list< algorithm_angluin::simple_row<answer, vector<answer> > >::const_iterator ti;

			c = this->column_names.size();
			ut = this->upper_table.size();
			lt = this->lower_table.size();
			stat["columns"] = c;
			stat["upper_table"] = ut;
			stat["lower_table"] = lt;
			stat["words"] = c*(ut+lt);

			// approx. memory usage:
			bytes = sizeof(this);
			// columns
			bytes += sizeof(vector<int>);
			for(ci = this->column_names.begin(); ci != this->column_names.end(); ci++)
				bytes += sizeof(int) * ci->size() + sizeof(list<int>);
			// upper table bare rows
			for(ti = this->upper_table.begin(); ti != this->upper_table.end(); ti++)
				bytes += sizeof(algorithm_angluin::simple_row<answer, vector<answer> >) + sizeof(int) * ti->index.size();
			// lower table bare rows
			for(ti = this->lower_table.begin(); ti != this->lower_table.end(); ti++)
				bytes += sizeof(algorithm_angluin::simple_row<answer, vector<answer> >) + sizeof(int) * ti->index.size();
			// table fields
			bytes += sizeof(answer) * c * (ut+lt);

			stat.set_integer_property("bytes", bytes);
			stat["bytes"] = bytes;
		}}}

		virtual bool deserialize(serial_stretch & serial)
		{{{
			int size;
			enum learning_algorithm<answer>::algorithm type;
			int count;

			this->column_names.clear();
			this->column_timestamps.clear();

			this->upper_table.clear();
			this->lower_table.clear();

			if(serial.current == serial.limit) goto deserialization_failed;

			// size
			size = ntohl(*serial.current);

			// check implementation type
			serial.current++; if(size <= 0 || serial.current == serial.limit) goto deserialization_failed;
			type = (enum learning_algorithm<answer>::algorithm) ntohl(*serial.current);
			if(type != learning_algorithm<answer>::ALG_ANGLUIN)
				goto deserialization_failed;

			// initialized
			if(!::deserialize(this->initialized, serial)) goto deserialization_failed;

			// alphabet size
			serial.current++; size--; if(size <= 0 || serial.current == serial.limit) goto deserialization_failed;
			this->set_alphabet_size(ntohl(*serial.current));
			if(this->get_alphabet_size() < 0)
				goto deserialization_failed;

			// column count
			serial.current++; size--; if(size <= 0 || serial.current == serial.limit) goto deserialization_failed;
			count = ntohl(*serial.current);
			if(count < 0)
				goto deserialization_failed;

			// column list
			serial.current++; size--; if(size <= 0 || serial.current == serial.limit) goto deserialization_failed;
			for(/* -- */; count > 0; count--) {
				// column label
				list<int> column_label;
				if(!::deserialize(column_label, serial))
					goto deserialization_failed;
				size -= column_label.size();
				this->column_names.push_back(column_label);

				// column timestamp
				if(serial.current == serial.limit)
					goto deserialization_failed;
				this->column_timestamps.push_back(ntohl(*serial.current));
				serial.current++; size--;
			}

			// upper table
			if(serial.current == serial.limit)
				goto deserialization_failed;
			count = ntohl(*serial.current);
			if(count < 0)
				goto deserialization_failed;
			// rows for upper table
			serial.current++; size--; if(size <= 0 || serial.current == serial.limit) goto deserialization_failed;
			for(/* -- */; count > 0; count--) {
				algorithm_angluin::simple_row<answer, vector<answer> > row;
				// peek size
				if(serial.current == serial.limit) goto deserialization_failed;
				size -= ntohl(*serial.current);
				if(size < 0) goto deserialization_failed;

				if(!row.deserialize(serial))
					goto deserialization_failed;

				this->upper_table.push_back(row);
			}

			// lower table
			if(serial.current == serial.limit)
				goto deserialization_failed;
			count = ntohl(*serial.current);
			if(count < 0)
				goto deserialization_failed;
			// rows for lower table
			serial.current++; size--; if(size <= 0 || serial.current == serial.limit) goto deserialization_failed;
			for(/* -- */; count > 0; count--) {
				algorithm_angluin::simple_row<answer, vector<answer> > row;
				// peek size
				if(serial.current == serial.limit) goto deserialization_failed;
				size -= ntohl(*serial.current);
				if(size < 0) goto deserialization_failed;

				if(!row.deserialize(serial))
					goto deserialization_failed;

				this->lower_table.push_back(row);
			}

			if(serial.current != serial.limit) goto deserialization_failed;

			return true;

		deserialization_failed:
			this->set_alphabet_size(0);
			this->column_names.clear();
			this->column_timestamps.clear();
			this->upper_table.clear();
			this->lower_table.clear();
			this->initialized = false;
			return false;
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			if(new_asize <= this->get_alphabet_size())
				return;

			typename list< algorithm_angluin::simple_row<answer, vector<answer> > >::iterator uti;
			algorithm_angluin::simple_row<answer, vector<answer> > row;

			if(this->norm) {
				// for all words in the upper table,
				for(uti = this->upper_table.begin(); uti != this->upper_table.end(); uti++) {
					list<int> w = uti->index;

					// add them suffixed with the new characters into the lower table.
					for(int new_suffix = this->get_alphabet_size(); new_suffix < new_asize; new_suffix++) {
						bool bottom;
						w.push_back(new_suffix);
						row.index = this->norm->prefix_normal_form(w, bottom);
						w.pop_back();
						this->lower_table.push_back(row);
					}
				}
			} else {
				// for all words in the upper table,
				for(uti = this->upper_table.begin(); uti != this->upper_table.end(); uti++) {
					row.index = uti->index;

					// add them suffixed with the new characters into the lower table.
					for(int new_suffix = this->get_alphabet_size(); new_suffix < new_asize; new_suffix++) {
						row.index.push_back(new_suffix);
						this->lower_table.push_back(row);
						row.index.pop_back();
					}
				}
			}

			this->set_alphabet_size(new_asize);
		}}}

	protected:
		virtual void initialize_table()
		{{{
			list<int> word; // empty word!

			// add epsilon as column
			this->column_names.push_back(word);
			this->column_timestamps.push_back(0);

			// add epsilon to upper table
			// and all suffixes to lower table
			this->add_word_to_upper_table(word);

			this->initialized = true;
		}}}

		virtual void add_word_to_upper_table(list<int> word, bool check_uniq = true)
		{{{
			list<int> nw;
			algorithm_angluin::simple_row<answer, vector<answer> > row;
			bool done = false;
			bool bottom = false;

			// normalize word
			if(this->norm)
				nw = this->norm->prefix_normal_form(word, bottom);
			else
				nw = word;

			if(check_uniq || bottom) {
				if(this->search_upper_table(nw) != this->upper_table.end()) {
					// already in upper table.
					return;
				}
				if(!bottom) {
					// check if in lower table. if so, move up.
					typename list< algorithm_angluin::simple_row<answer, vector<answer> > >::iterator ti;
//printf("scanning for %s in lower.\n", word2string(nw).c_str());
					ti = this->search_lower_table(nw);
					if(ti != this->lower_table.end()) {
						done = true;
						if(this->my_knowledge != NULL)
							ti->ut_timestamp = this->my_knowledge->get_timestamp();
						else
							ti->ut_timestamp = 0;
						this->upper_table.push_back(*ti);
						this->lower_table.erase(ti);
					}
				}
			}

			// add the word to the upper table
			if(!done) {
				row.index = nw;
				if(this->my_knowledge != NULL) {
					row.ut_timestamp = this->my_knowledge->get_timestamp();
					row.lt_timestamp = 0;
				} else {
					row.ut_timestamp = 0;
					row.lt_timestamp = 0;
				}
				this->upper_table.push_back(row);
			}

			if(bottom) // no suffixes required, they would be bottom again.
				return;

			// add all suffixes of word to lower table
			for( int i = 0; i < this->get_alphabet_size(); i++ ) {
				nw.push_back(i);
				// normalize word
				if(this->norm)
					word = this->norm->prefix_normal_form(nw, bottom);
				else
					word = nw;
				nw.pop_back();

				if(!bottom) {
					done = false;
					if(check_uniq) {
						// if the suffixed word was in lower table, the word would
						// already have been in the upper table. we only need to check, if
						// the suffixed word is in the upper table.
						done = ( this->search_upper_table(word) != this->upper_table.end() );
					}
					if(!done) {
						row.index = word;
						if(this->my_knowledge != NULL) {
							row.ut_timestamp = 0;
							row.lt_timestamp = this->my_knowledge->get_timestamp();
						} else {
							row.ut_timestamp = 0;
							row.lt_timestamp = 0;
						}
						this->lower_table.push_back(row);
					}
				} else {
					if(this->search_upper_table(word) == this->upper_table.end()) {
						row.index = word;
						if(this->my_knowledge != NULL) {
							row.ut_timestamp = this->my_knowledge->get_timestamp();
							row.lt_timestamp = 0;
						} else {
							row.ut_timestamp = 0;
							row.lt_timestamp = 0;
						}
						this->upper_table.push_back(row);
					}
				}
			}
		}}}

};



// the col-table version handles counter-examples in a different way:
// instead of adding counterexamples and their prefixes to the upper table,
// counterexamples and their suffixes are added to the columns.
// this way the table never can be __inconsistent__, so several
// functions can be reduced in complexity.
template <class answer>
class angluin_col_table : public angluin_simple_table<answer> {
	public:
		angluin_col_table()
		{{{
			this->set_alphabet_size(0);
			this->set_knowledge_source(NULL);
			this->set_logger(NULL);
		}}}
		angluin_col_table(knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(base);
		}}}

		virtual bool add_counterexample(list<int> word)
		{{{
			typename vector< list<int> >::iterator ci;
			list<int>::iterator wi;

			if(this->my_knowledge == NULL) {
				(*this->my_logger)(LOGGER_ERROR, "angluin_col_table: add_counterexample() without knowledgebase!\n");
				return false;
			}

			ci = this->search_columns(word);
			if(ci != this->column_names.end()) {
				string s = word2string(word);
				(*this->my_logger)(LOGGER_ERROR, "angluin_col_table: add_counterexample(): you are trying to add a counterexample (%s) which is already contained in the table. trying to ignore...\n", s.c_str());
				return false;
			}

			// check for increase in alphabet size
			int new_asize = this->get_alphabet_size();
			bool asize_changed = false;
			for(wi = word.begin(); wi != word.end(); wi++) {
				if(*wi >= new_asize) {
					new_asize = *wi+1;
					asize_changed = true;
				}
			}
			if(asize_changed) {
				(*this->my_logger)(LOGGER_ALGORITHM, "angluin_col_table: add_counterexample(): increase of alphabet_size from %d to %d.\nNOTE: it is possible that the next hypothesis does not increase in state-count.\n", this->get_alphabet_size(), new_asize);
				this->increase_alphabet_size(new_asize);
			}

			// add word and all suffixes to the columns
			while(!word.empty()) {
				if(!this->add_column(word))
					break;
				word.pop_front();
			}

			return true;
		}}}

	protected:
		virtual typename vector< list<int> >::iterator search_columns(list<int> &word)
		{{{
			typename vector< list<int> >::iterator ci;

			for(ci = this->column_names.begin(); ci != this->column_names.end(); ++ci) {
				if(*ci == word)
					break;
			}

			return ci;
		}}}
		virtual bool conjecture_ready()
		{{{
			return this->initialized && this->columns_filled() && this->is_closed();
		}}}
		virtual bool complete()
		{{{
			if(!this->initialized)
				this->initialize_table();

			if(this->fill_missing_columns(this->upper_table) && this->fill_missing_columns(this->lower_table)) {
				if(!this->close())
					return complete();

				return true;
			} else {
				return false;
			}
		}}}
};


}; // end of namespace libalf

#endif

