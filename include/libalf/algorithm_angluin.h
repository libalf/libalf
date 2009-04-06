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

#ifndef __libalf_algorithm_angluin_h__
# define __libalf_algorithm_angluin_h__

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <ostream>
#include <sstream>

#include <arpa/inet.h>

#include <libalf/alphabet.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>
#include <libalf/automata.h>

namespace libalf {

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
 * observation table for angluin learning algorithm
 */
template <class answer, class table, class acceptances>
class angluin_observationtable : public learning_algorithm<answer> {
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

	protected:
		typedef vector< list<int> > columnlist;
		columnlist column_names;
		list<unsigned int> column_timestamps;

		table upper_table;
		table lower_table;

		bool initialized;

	public:
		angluin_observationtable()
		{{{
			this->set_knowledge_source(NULL, NULL);
			this->set_logger(NULL);
			this->set_normalizer(NULL);
			this->set_alphabet_size(0);
			initialized = false;
		}}}

		virtual void get_memory_statistics(statistics & stats) = 0;

		virtual bool sync_to_knowledgebase()
		{{{
			if(this->my_knowledge == NULL) {
				(*this->my_logger)(LOGGER_WARN, "angluin_observationtable: sync-operation is only supported in combination with a knowledgebase!\n");
				return false;
			}

			// we will check all knowledge back with the knowledgebase.
			bool ret = true;
			if(!sync_columns())
				ret = false;
			if(!sync_tables())
				ret = false;

			return false;
		}}}

		virtual bool supports_sync()
		{ return true; }


		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;
			basic_string<int32_t> temp;
			typename table::iterator ti;
			typename columnlist::iterator ci;
			typename list<unsigned int>::iterator tsi;

			// length (filled in later)
			ret += 0;

			// implementation type
			ret += htonl(learning_algorithm<answer>::ALG_ANGLUIN);

			// alphabet size
			ret += htonl(this->get_alphabet_size());

			// column list
			ret += htonl(column_names.size());
			for(ci = column_names.begin(), tsi = column_timestamps.begin();
					ci != column_names.end() && tsi != column_timestamps.end();
					ci++, tsi++) {
				// column label
				ret += serialize_word(*ci);
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
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		virtual void print(ostream &os)
		{{{
			typename columnlist::iterator ci;
			typename list<unsigned int>::iterator tsi;
			typename table::iterator ti;
			typename acceptances::iterator acci;
			char buf[32];

			os << "simple_observationtable {\n";
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
					if(*acci == true)
						os << "+ ";
					else
						if(*acci == false)
							os << "- ";
						else
							os << "? ";
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
					if(*acci == true)
						os << "+ ";
					else
						if(*acci == false)
							os << "- ";
						else
							os << "? ";
				}
				os << ";\n";
			}

			os << "}\n";
		}}}

		virtual string tostring()
		{{{
			stringstream str;
			this->print(str);
			return str.str();
		}}}

		virtual bool conjecture_ready()
		{{{
			return initialized && columns_filled() && is_closed() && is_consistent();
		}}}

		// FIXME: warn if bottom and true
		virtual void add_counterexample(list<int> word, answer a)
		{{{
			bool bottom = false;
			list<int> prefix;

			list<int>::iterator wi;
			int new_asize;
			bool asize_changed = false;

			// check for increase in alphabet size
			for(wi = word.begin(); wi != word.end(); wi++) {
				if(*wi >= this->get_alphabet_size()) {
					new_asize = *wi+1;
					asize_changed = true;
				}
			}
			if(asize_changed)
				increase_alphabet_size(new_asize);

			// add word and all prefixes to upper table
			prefix = word;
			while(!prefix.empty()) {
				add_word_to_upper_table(prefix);
				prefix.pop_back();
			}

			// add answer for counterexample
			if(this->norm)
				word = this->norm->prefix_normal_form(word, bottom);
			if(!bottom) {
				typename table::iterator uti = search_upper_table(word);
				if(uti->acceptance.size() == 0) {
					uti->acceptance.push_back(a);
				} else {
					string s = word2string(word);
					if((answer)uti->acceptance[0] == (answer)a) {
						(*this->my_logger)(LOGGER_ERROR, "angluin_observationtable: you are trying to add a counterexample (%s) which is already contained in the table. at least you try to set the same acceptance for it. trying to ignore.\n", s.c_str());
					} else {
						(*this->my_logger)(LOGGER_ERROR, "angluin_observationtable: you are trying to add a counterexample (%s) which is already contained in the table. you even try to set a different acceptance for it! trying to ignore.\n", s.c_str());
					}
				}
			} else {
				// bottom is filled automatically
				if(a == true)
					(*this->my_logger)(LOGGER_ERROR, "angluin_observationtable: counterexample is bottom and answer is even true\n");
				else
					(*this->my_logger)(LOGGER_ERROR, "angluin_observationtable: counterexample is bottom\n");
			}
		}}}

		virtual void add_counterexample(list<int> word)
		{{{
			typename table::iterator ti;
			list<int>::iterator wi;
			int new_asize;
			bool asize_changed = false;

			ti = search_tables(word);
			if(ti != lower_table.end()) {
				string s = word2string(word);
				(*this->my_logger)(LOGGER_WARN, "angluin_observationtable: angluin_observationtable: you are trying to add a counterexample (%s) which is already contained in the table. trying to ignore...\n", s.c_str());
				return;
			}

			// check for increase in alphabet size
			for(wi = word.begin(); wi != word.end(); wi++) {
				if(*wi >= this->get_alphabet_size()) {
					new_asize = *wi+1;
					asize_changed = true;
				}
			}
			if(asize_changed)
				increase_alphabet_size(new_asize);

			// add word and all prefixes to upper table
			while(!word.empty()) {
				add_word_to_upper_table(word);
				word.pop_back();
			}
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
		virtual typename table::iterator search_upper_table(list<int> &word)
		{{{
			typename table::iterator uti;

			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				if(word == uti->index)
					return uti;
			}
			return upper_table.end();
		}}}

		// this expects a NORMALIZED word!
		virtual typename table::iterator search_upper_table(list<int> &word, int &index)
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
		virtual typename table::iterator search_lower_table(list<int> &word)
		{{{
			typename table::iterator lti;

			for(lti = lower_table.begin(); lti != lower_table.end(); lti++)
				if(word == lti->index)
					return lti;
			return lower_table.end();
		}}}

		// this expects a NORMALIZED word!
		virtual typename table::iterator search_lower_table(list<int> &word, int &index)
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
		virtual typename table::iterator search_tables(list<int> &word)
		{{{
			typename table::iterator it;

			it = search_upper_table(word);
			if(it != upper_table.end())
				return it;

			return search_lower_table(word);
		}}}

		// this expects a NORMALIZED word!
		virtual typename table::iterator search_tables(list<int> &word, bool &upper_table, int&index)
		{{{
			typename table::iterator it;

			it = search_upper_table(word, index);
			if(index != -1) {
				return it;
			}

			return search_lower_table(word, index);
		}}}

		virtual void add_column(list<int> word)
		{{{
			typename columnlist::iterator ci;
			list<int> nw;

			bool bottom;
			if(this->norm)
				nw = this->norm->suffix_normal_form(word, bottom);
			else
				nw = word;

			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				if(*ci == nw)
					return;

			if(this->my_knowledge != NULL)
				column_timestamps.push_back(this->my_knowledge->get_timestamp());
			else
				column_timestamps.push_back(0);

			column_names.push_back(nw);
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
								ti->acceptance.push_back(a);
							} else {
								list<int> *w;
								w = concat(ti->index, *ci);
								if(this->my_teacher) {
									ti->acceptance.push_back(this->my_teacher->membership_query(*w));
								} else {
									if(this->my_knowledge) {
										answer a;
										if(this->my_knowledge->resolve_or_add_query(*w, a)) {
											if(!column_skipped)
												ti->acceptance.push_back(a);
										} else {
											column_skipped = true;
											complete = false;
										}
									}
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
				// FIXME: <= or < ?
				if(column_timestamps.back() < this->my_knowledge->get_timestamp()) {
					timestamp_ok = true;
				} else {
					column_names.pop_back();
					column_timestamps.pop_back();
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

			sync_table_acceptances(upper_table, true);
			sync_table_acceptances(lower_table, false);

			return true;
		}}}

		virtual void sync_table_acceptances(table & t, bool upper)
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
							printf("ERROR: undo: acceptances differ between revisions\n");
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
			bool not_changed = true;
			typename table::iterator uti, lti, tmplti;

			for(lti = lower_table.begin(); lti != lower_table.end(); /* -- */) {
				bool match_found = false;

				for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
					if(*lti == *uti) {
						match_found = true;
						break;
					}
				}
				if(!match_found) {
					// create entry in upper table
					add_word_to_upper_table(lti->index, false);
					typename table::iterator last = upper_table.end();
					last--;
					// copy acceptance status for that row
					swap(last->acceptance, lti->acceptance);
					// go to next and delete old lower entry
					tmplti = lti;
					tmplti++;
					//lti.erase();
					lower_table.erase(lti);
					lti = tmplti;

					not_changed = false;
				} else {
					// go to next
					lti++;
				}
			}

			return not_changed;
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
			bool not_changed = true;

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
									not_changed = false;

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
											add_column(newsuffix);
											ci = column_names.begin();
											// when changing the column list, the last iterator may change.
											// if so, using the old one results in a segfault.
											for(int j = 0; j < cindex; j++)
												ci++;
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


			return not_changed;
		}}}

		// complete table, so an automaton can be derived
		// returns true, if automaton may be derived.
		// returns false if new knowledge is required (in case of knowledgebase).
		virtual bool complete()
		{{{
			if(!initialized) {
				initialize_table();
				initialized = true;
			}

			if(fill_missing_columns(upper_table) && fill_missing_columns(lower_table)) {
				if(!close()) {
					return complete();
				}

				if(!make_consistent()) {
					return complete();
				}

				return true;
			} else {
				return false;
			}
		}}}

		virtual bool derive_automaton(finite_language_automaton * automaton)
		{{{
			// derive deterministic finite automaton from this table
			typename table::iterator uti, ti;

			algorithm_angluin::automaton_state<table> state;
			list<algorithm_angluin::automaton_state<table> > states;
			state.id = 0;
			typename list<algorithm_angluin::automaton_state<table> >::iterator state_it, state_it2;

			list<int> initial;

			list<int> final;

			list<transition> transitions;

			// list of states of automaton: each different acceptance-row
			// in the upper table represents one DFA state
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

				state.id++;
			}

			// q0 is row(epsilon)
			// as epsilon is the first row in uti, it will have id 0.
			initial.push_back( 0 );

			for(state_it = states.begin(); state_it != states.end(); state_it++) {
				// the final, accepting states are the rows with
				// acceptance in the epsilon-column
				if(state_it->tableentry->acceptance.front() == true)
					final.push_back(state_it->id);

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
							transition tr;

							tr.source = state_it->id;
							tr.label = i;
							tr.destination = state_it2->id;
							transitions.push_back(tr);
							break;
						}
					}
				}
			}

			return automaton->construct(this->get_alphabet_size(), states.size(),
						initial, final, transitions);
		}}}

};

	namespace algorithm_angluin {
		template <class answer, class acceptances>
		class simple_row {
			public:
				list<int> index;
				acceptances acceptance;
				unsigned int ut_timestamp;
				unsigned int lt_timestamp;

				bool __attribute__((const)) operator==(simple_row<answer, acceptances> &other)
				{{{
					return (acceptance == other.acceptance);
				}}}

				bool __attribute__((const)) operator!=(simple_row<answer, acceptances> &other)
				{{{
					return (acceptance != other.acceptance);
				}}}

				bool __attribute__((const)) operator>(simple_row<answer, acceptances> &other)
				{{{
					typename acceptances::iterator ai;
					typename acceptances::iterator oai;

					ai = acceptance.begin();
					oai = other.acceptance.begin();

					for(/* -- */; ai < acceptance.end() && oai < other.acceptance.end(); ai++, oai++) {
						if(*ai > *oai)
							return true;
					}

					return false;
				}}}

				basic_string<int32_t> serialize()
				{{{
					basic_string<int32_t> ret;
					basic_string<int32_t> temp;
					typename acceptances::iterator acci;

					// length (filled in later)
					ret += 0;

					// index
					ret += serialize_word(this->index);

					// ut timestamp
					ret += htonl(ut_timestamp);

					// lt timestamp
					ret += htonl(lt_timestamp);

					// accumulate acceptances
					for(acci = this->acceptance.begin(); acci != this->acceptance.end(); acci++) {
						temp += htonl((int32_t)(*acci));
					}

					// number of acceptances
					ret += htonl(temp.length());
					// acceptances
					ret += temp;

					ret[0] = htonl(ret.length() - 1);

					return ret;
				}}}

				bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
				{{{
					int size;
					int count;

					index.clear();
					acceptance.clear();

					if(it == limit)
						return false;
					size = ntohl(*it);
					it++; if(size <= 0 || it == limit) return false;

					// index
					if( ! deserialize_word(this->index, it, limit) )
						goto deserialization_failed;
					size -= this->index.size() + 1;
					if(size <= 0 || it == limit) goto deserialization_failed;

					// ut timestamp
					ut_timestamp = ntohl(*it);
					it++, size--;
					if(it == limit || size <= 0)
						goto deserialization_failed;

					// lt timestamp
					lt_timestamp = ntohl(*it);
					it++, size--;
					if(it == limit || size <= 0)
						goto deserialization_failed;

					// number of acceptances
					count = ntohl(*it);
					it++, size--;
					if(size != count)
						goto deserialization_failed;

					// acceptances
					for(/* -- */; count > 0 && it != limit; count--, it++) {
						if(it == limit)
							goto deserialization_failed;
						answer a;
						a = (int32_t)(ntohl(*it));
						acceptance.push_back(a);
					}

					if(count)
						goto deserialization_failed;

					return true;

				deserialization_failed:
					index.clear();
					acceptance.clear();
					return false;
				}}}

		};
	};


template <class answer>
class angluin_simple_observationtable : public angluin_observationtable<answer, list< algorithm_angluin::simple_row<answer, vector<answer> > >, vector<answer> > {
	public:
		angluin_simple_observationtable()
		{{{
			this->set_alphabet_size(0);
			this->set_knowledge_source(NULL, NULL);
			this->set_logger(NULL);
		}}}
		angluin_simple_observationtable(teacher<answer> *teach, knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(teach, base);
		}}}

		virtual void get_memory_statistics(statistics & stats)
		{{{
			typename angluin_observationtable<answer, list< algorithm_angluin::simple_row<answer, vector<answer> > >, vector<answer> >::columnlist::iterator ci;
			typename list< algorithm_angluin::simple_row<answer, vector<answer> > >::iterator ti;

			stats.table_size.columns = this->column_names.size();
			stats.table_size.upper_table = this->upper_table.size();
			stats.table_size.lower_table = this->lower_table.size();

			stats.table_size.members = stats.table_size.columns * ( stats.table_size.upper_table + stats.table_size.lower_table );
			stats.table_size.words = stats.table_size.members;

			// approx. memory usage:
			stats.table_size.bytes = sizeof(this);
			// columns
			stats.table_size.bytes += sizeof(vector<int>);
			for(ci = this->column_names.begin(); ci != this->column_names.end(); ci++)
				stats.table_size.bytes += sizeof(int) * ci->size() + sizeof(list<int>);
			// upper table bare rows
			for(ti = this->upper_table.begin(); ti != this->upper_table.end(); ti++)
				stats.table_size.bytes += sizeof(algorithm_angluin::simple_row<answer, vector<answer> >) + sizeof(int) * ti->index.size();
			// lower table bare rows
			for(ti = this->lower_table.begin(); ti != this->lower_table.end(); ti++)
				stats.table_size.bytes += sizeof(algorithm_angluin::simple_row<answer, vector<answer> >) + sizeof(int) * ti->index.size();
			// table fields
			stats.table_size.bytes += sizeof(answer) * stats.table_size.members;
		}}}

		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			int size;
			enum learning_algorithm<answer>::algorithm type;
			int count;

			this->column_names.clear();
			this->upper_table.clear();
			this->lower_table.clear();

			if(it == limit) goto deserialization_failed;

			// size
			size = ntohl(*it);

			// check implementation type
			it++; if(size <= 0 || it == limit) goto deserialization_failed;
			type = (enum learning_algorithm<answer>::algorithm) ntohl(*it);
			if(type != learning_algorithm<answer>::ALG_ANGLUIN)
				goto deserialization_failed;

			// alphabet size
			it++; size--; if(size <= 0 || it == limit) goto deserialization_failed;
			this->set_alphabet_size(ntohl(*it));
			if(this->get_alphabet_size() < 0)
				goto deserialization_failed;

			// column count
			it++; size--; if(size <= 0 || it == limit) goto deserialization_failed;
			count = ntohl(*it);
			if(count < 0)
				goto deserialization_failed;

			// column list
			it++; size--; if(size <= 0 || it == limit) goto deserialization_failed;
			for(/* -- */; count > 0; count--) {
				// column label
				list<int> column_label;
				if(!deserialize_word(column_label, it, limit))
					goto deserialization_failed;
				size -= column_label.size();
				this->column_names.push_back(column_label);

				// column timestamp
				if(it == limit)
					goto deserialization_failed;
				this->column_timestamps.push_back(ntohl(*it));
				it++; size--;
			}

			// upper table
			if(it == limit)
				goto deserialization_failed;
			count = ntohl(*it);
			if(count < 0)
				goto deserialization_failed;
			// rows for upper table
			it++; size--; if(size <= 0 || it == limit) goto deserialization_failed;
			for(/* -- */; count > 0; count--) {
				algorithm_angluin::simple_row<answer, vector<answer> > row;
				// peek size
				if(it == limit) goto deserialization_failed;
				size -= ntohl(*it);
				if(size < 0) goto deserialization_failed;

				if(!row.deserialize(it, limit))
					goto deserialization_failed;

				this->upper_table.push_back(row);
			}

			// lower table
			if(it == limit)
				goto deserialization_failed;
			count = ntohl(*it);
			if(count < 0)
				goto deserialization_failed;
			// rows for lower table
			it++; size--; if(size <= 0 || it == limit) goto deserialization_failed;
			for(/* -- */; count > 0; count--) {
				algorithm_angluin::simple_row<answer, vector<answer> > row;
				// peek size
				if(it == limit) goto deserialization_failed;
				size -= ntohl(*it);
				if(size < 0) goto deserialization_failed;

				if(!row.deserialize(it, limit))
					goto deserialization_failed;

				this->lower_table.push_back(row);
			}

			if(it != limit) goto deserialization_failed;

			return true;

		deserialization_failed:
			this->set_alphabet_size(0);
			this->column_names.clear();
			this->upper_table.clear();
			this->lower_table.clear();
			return false;
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			if(new_asize <= this->get_alphabet_size())
				return;

			typename list< algorithm_angluin::simple_row<answer, vector<answer> > >::iterator uti;
			algorithm_angluin::simple_row<answer, vector<answer> > row;

			// for all words in the upper table,
			for(uti = this->upper_table.begin(); uti != this->upper_table.end(); uti++) {
				list<int> word;
				row.index = uti->index;

				// add them suffixed with the new characters into the lower table.
				for(int new_suffix = this->get_alphabet_size(); new_suffix < new_asize; new_suffix++) {
					row.index.push_back(new_suffix);
					this->lower_table.push_back(row);
					row.index.pop_back();
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
			this->add_word_to_upper_table(word, false);
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

			if(bottom) // no suffixed required, they would be bottom again.
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
							row.ut_timestamp = 0;
							row.lt_timestamp = this->my_knowledge->get_timestamp();
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


}; // end of namespace libalf

#endif

