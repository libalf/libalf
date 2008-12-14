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

#include <arpa/inet.h>

#include <libalf/alphabet.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>
#include <libalf/structured_query_tree.h>
#include <libalf/automata.h>

namespace libalf {

using namespace std;

	namespace algorithm_angluin {
		template <class answer, class table>
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
	 *	basic_string<int32_t> table::iterator->serialize()
	 *	bool table::iterator->deserialize(basic_string<int32_t>::iterator it, basic_string<int32_t>::iterator limit);
	 *	(see implementation notes on serialization members)
	 *
	 *	note: acceptance can e.g. be a member vector<answer> of the *(table::iterator) type
	\*/

	protected:
		typedef vector< list<int> > columnlist;
		columnlist column_names;

		table upper_table;
		table lower_table;

		teacher<answer> * teach;
		logger * log;
		int alphabet_size;

	public:
		angluin_observationtable()
		{{{
			teach = NULL;
			log = NULL;
			alphabet_size = 0;
		}}}

		virtual void set_alphabet_size(int alphabet_size)
		{{{
			this->alphabet_size = alphabet_size;
		}}}

		virtual int get_alphabet_size()
		{{{
			return alphabet_size;
		}}}

		virtual void set_teacher(teacher<answer> * teach)
		{{{
			this->teach = teach;
		}}}

		virtual void unset_teacher()
		{{{
			this->teach = NULL;
		}}}

		virtual teacher<answer> * get_teacher()
		{{{
			return teach;
		}}}

		virtual void set_logger(logger * l)
		{{{
			log = l;
		}}}

		virtual logger * get_logger()
		{{{
			return log;
		}}}

		virtual void undo()
		{{{
			  if(log)
				  (*log)(LOGGER_ERROR, "simple_observationtable::undo() is not implemented.\naborting.\n");

			  // FIXME: undo is not implemented
		}}}

		virtual void redo()
		{{{
			  if(log)
				  (*log)(LOGGER_ERROR, "simple_observationtable::redo() is not implemented.\naborting.\n");

			  // FIXME: redo is not implemented
		}}}

		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;
			basic_string<int32_t> temp;
			typename table::iterator ti;

			// length (filled in later)
			ret += 0;

			// implementation type
			ret += htonl(learning_algorithm<answer>::ALG_ANGLUIN);

			// alphabet size
			ret += htonl(alphabet_size);

			// column list
			ret += htonl(column_names.size());
			for(columnlist::iterator ci = column_names.begin(); ci != column_names.end(); ci++)
				ret += serialize_word(*ci);

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

		// (implementation specific:)
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		virtual void print(ostream &os)
		{{{
			typename columnlist::iterator ci;
			typename table::iterator ti;
			typename acceptances::iterator acci;

			os << "simple_observationtable {\n";
			os << "\tcolumns:";

			for(ci = column_names.begin(); ci != column_names.end(); ci++) {
				os << " ";
				print_word(os, *ci);
			}
			os << " ;\n";

			os << "\tupper table:\n";
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++) {
				os << "\t\t";
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

		// searches column first, then row
/* superficial
		virtual pair<bool, answer> check_entry(list<int> word)
		{{{
			columnlist::iterator ci;
			unsigned int col_index;
			typename table::iterator uti, lti;

			pair<bool, answer> ret;

			// find possible suffixes in table columns
			for(ci = column_names.begin(), col_index = 0; ci < column_names.end(); ci++, col_index++) {
				if(is_suffix_of(*ci, word)) {
					// find possible prefix as column index
					// then return truth value
					list<int> prefix;
					list<int>::iterator prefix_end;
					prefix_end = word.begin();
					for(int n = word.size() - ci->size(); n > 0; n--)
						prefix_end++;
					//prefix_end += word.size() - ci->size();
					prefix.assign(word.begin(), prefix_end);

					uti = search_upper_table(prefix);
					if(uti != upper_table.end()) {
						ret.first = true;
						ret.second = uti->acceptance[col_index];
						return ret;
					}
					lti = search_lower_table(prefix);
					if(lti != lower_table.end()) {
						ret.first = true;
						ret.second = lti->acceptance[col_index];
						return ret;
					}
				}
			}

			// word is not in table
			ret.first = false;
			ret.second = false;
			return ret;
		}}}
*/

		virtual bool conjecture_ready()
		{{{
			return is_closed() && is_consistent();
		}}}

		virtual structured_query_tree<answer> * derive_hypothesis(finite_language_automaton * automaton)
		{{{
			structured_query_tree<answer> * ret;

			ret = complete();

			if(!ret)
				if( ! derive_automaton(automaton) )
					(*log)(LOGGER_ERROR, "angluin_observationtable::derive_hypothesis : derive of complete tabled failed!\n");

			return ret;
		}}}

		virtual bool learn_from_structured_query(structured_query_tree<answer> & sqt)
		{{{
			typename table::iterator uti, lti;

			// upper table
			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				if(uti->acceptance.size() < column_names.size()) {
					// fill in missing acceptance information
					columnlist::iterator ci;
					ci = column_names.begin();
					ci += uti->acceptance.size();
					for(/* -- */; ci != column_names.end(); ci++) {
						list<int> *w;
						answer a;
						w = concat(uti->index, *ci);
						if( ! sqt.resolve_query(*w, a) ) {
							return false;
						}

						uti->acceptance.push_back(a);

						delete w;
					}
				}
			}
			// lower table
			for(lti = lower_table.begin(); lti != lower_table.end(); lti++) {
				if(lti->acceptance.size() < column_names.size()) {
					// fill in missing acceptance information
					columnlist::iterator ci;
					ci = column_names.begin();
					ci += lti->acceptance.size();
					for(/* -- */; ci != column_names.end(); ci++) {
						list<int> *w;
						answer a;
						w = concat(lti->index, *ci);
						if( ! sqt.resolve_query(*w, a) ) {
							return false;
						}

						lti->acceptance.push_back(a);

						delete w;
					}
				}
			}

			return true;
		}}}

		virtual void add_counterexample(list<int> word, answer a)
		{{{
			list<int> prefix = word;
			int ps;

			list<int>::iterator wi;
			int new_asize;
			bool asize_changed = false;

			// check for increase in alphabet
			for(wi = word.begin(); wi != word.end(); wi++) {
				if(*wi >= alphabet_size) {
					new_asize = *wi;
					asize_changed = true;
				}
			}
			if(asize_changed)
				increase_alphabet_size(new_asize);

			// add word and all prefixes to upper table
			for(ps = prefix.size(); ps > 0; ps--) {
				add_word_to_upper_table(prefix);
				prefix.pop_back();
			}

			typename table::iterator uti = search_upper_table(word);
			if(uti->acceptance.size() == 0) {
				uti->acceptance.push_back(a);
			} else {
				// XXX SHOULD NEVER HAPPEN
				uti->acceptance[0] = a;
			}
		}}}

		virtual void add_counterexample(list<int> word)
		{{{
			list<int> prefix = word;

			list<int>::iterator wi;
			int new_asize;
			bool asize_changed = false;

			// check for increase in alphabet
			for(wi = word.begin(); wi != word.end(); wi++) {
				if(*wi >= alphabet_size) {
					new_asize = *wi;
					asize_changed = true;
				}
			}
			if(asize_changed)
				increase_alphabet_size(new_asize);

			// add word and all prefixes to upper table
			while(!prefix.empty()) {
				add_word_to_upper_table(prefix);
				prefix.pop_back();
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

	protected:
		virtual void increase_alphabet_size(int new_asize)
		{
			// add all new suffixes etc
			// FIXME: increase_alphabet_size is not implemented
		}

		virtual void add_word_to_upper_table(list<int> word, bool check_uniq = true) = 0;

		virtual typename table::iterator search_upper_table(list<int> &word)
		{{{
			typename table::iterator uti;

			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				if(word == uti->index)
					return uti;
			}
			return upper_table.end();
		}}}

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

		virtual typename table::iterator search_lower_table(list<int> &word)
		{{{
			typename table::iterator lti;

			for(lti = lower_table.begin(); lti != lower_table.end(); lti++)
				if(word == lti->index)
					return lti;
			return lower_table.end();
		}}}

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

		virtual typename table::iterator search_tables(list<int> &word)
		{{{
			typename table::iterator it;

			it = search_upper_table(word);
			if(it != upper_table.end())
				return it;

			return search_lower_table(word);
		}}}

		virtual typename table::iterator search_tables(list<int> &word, bool &upper_table, int&index)
		{{{
			typename table::iterator it;

			it = search_upper_table(word, index);
			if(index != -1) {
				return it;
			}

			return search_lower_table(word, index);
		}}}

		virtual void add_column(list<int>word)
		{{{
			typename columnlist::iterator ci;

			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				if(*ci == word)
					return;

			column_names.push_back(word);
		}}}

		virtual structured_query_tree<answer> * fill_missing_columns()
		{{{
			typename table::iterator uti, lti;
			structured_query_tree<answer> * sqt = NULL;
			bool complete = true;

			if(!teach)
				sqt = new structured_query_tree<answer>();

			// upper table
			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				if(uti->acceptance.size() < column_names.size()) {
					// fill in missing acceptance information
					columnlist::iterator ci;
					ci = column_names.begin();
					ci += uti->acceptance.size();
					for(/* -- */; ci != column_names.end(); ci++) {
						list<int> *w;
						w = concat(uti->index, *ci);
						if(teach) {
							uti->acceptance.push_back(teach->membership_query(*w));
						} else {
							sqt->add_query(*w, 0);
							complete = false;
						}

						delete w;
					}
				}
			}
			// lower table
			for(lti = lower_table.begin(); lti != lower_table.end(); lti++) {
				if(lti->acceptance.size() < column_names.size()) {
					// fill in missing acceptance information
					columnlist::iterator ci;
					ci = column_names.begin();
					ci += lti->acceptance.size();
					for(/* -- */; ci != column_names.end(); ci++) {
						list<int> *w;
						w = concat(lti->index, *ci);
						if(teach) {
							lti->acceptance.push_back(teach->membership_query(*w));
						} else {
							sqt->add_query(*w, 0);
							complete = false;
						}
						delete w;
					}
				}
			}

			if(complete) {
				delete sqt;
				return NULL;
			} else {
				return sqt;
			}
		}}}

		// sample implementation only:
		//  all possible answer-rows in
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
		// returns false if table was closed before,
		//         true if table was changed (and thus needs to be filled)
		virtual bool close()
		{{{
			bool changed = false;
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

					changed = true;
				} else {
					// go to next
					lti++;
				}
			}

			return changed;
		}}}

		// sample implementation only:
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
						for(int sigma = 0; sigma < alphabet_size; sigma++) {
							word1.push_back(sigma);
							w1succ = search_tables(word1);

							word2.push_back(sigma);
							w2succ = search_tables(word2);

							if(*w1succ != *w2succ)
								return false;

							word1.pop_back();
							word2.pop_back();
						}
					}
				}
			}
			return true;
		}}}

		// make table consistent: perform operations to do that.
		// returns false if table was consistent before,
		//         true if table was changed (and thus needs to be filled)
		virtual bool make_consistent()
		{{{
			bool changed = false;

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
						for(int sigma = 0; sigma < alphabet_size; sigma++) {
							word1.push_back(sigma);
							w1_succ = search_tables(word1);

							word2.push_back(sigma);
							w2_succ = search_tables(word2);

							if(*w1_succ != *w2_succ) {
								if(w1_succ->acceptance.size() == w2_succ->acceptance.size()) {
									// add suffixes resulting in different states to column_names
									changed = true;

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

							word1.pop_back();
							word2.pop_back();
						}
					}
				}
			}

			return changed;
		}}}

		virtual structured_query_tree<answer> * complete()
		{{{
			structured_query_tree<answer> * ret;

			ret = fill_missing_columns();

			if(!ret) {
				if(close()) {
					return complete();
				}

				if(make_consistent()) {
					return complete();
				}
			}

			return ret;
		}}}

		virtual bool derive_automaton(finite_language_automaton * automaton)
		{{{
			// derive deterministic finite automaton from this table
			typename table::iterator uti, ti;

			algorithm_angluin::automaton_state<answer, table> state;
			list<algorithm_angluin::automaton_state<answer, table> > states;
			state.id = 0;
			typename list<algorithm_angluin::automaton_state<answer, table> >::iterator state_it, state_it2;

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
				for(int i = 0; i < alphabet_size; i++) {
					// find successor in table
					index.push_back(i);
					ti = search_tables(index);

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

					index.pop_back();
				}
			}

			return automaton->construct(alphabet_size, states.size(),
						initial, final, transitions);
		}}}

};

	namespace algorithm_angluin {
		template <class answer, class acceptances>
		class simple_row {
			public:
				list<int> index;
				acceptances acceptance;

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
					it++;
					if(size <= 0 || it == limit) return false;

					// index
					if( ! deserialize_word(this->index, it, limit) )
						goto deserialization_failed;
					size -= this->index.length() + 1;
					if(size <= 0 || it == limit) goto deserialization_failed;

					// number of acceptances
					count = ntohl(*it);
					it++, count--;
					if(it == limit || count <= 0 || size != count)
						goto deserialization_failed;

					// acceptances
					for(/* -- */; count > 0 && it != limit; count--, it++) {
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
		angluin_simple_observationtable(teacher<answer> *teach, logger *log, int alphabet_size)
		{{{
			this->alphabet_size = alphabet_size;

			this->set_teacher(teach);
			this->set_logger(log);

			list<int> word; // empty word!

			// add epsilon as column
			this->column_names.push_back(word);

			// add epsilon to upper table
			// and all suffixes to lower table
			this->add_word_to_upper_table(word, false);
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
			// FIXME: deserialize is not implemented
			return false;
		}

	protected:
		virtual void add_word_to_upper_table(list<int> word, bool check_uniq = true)
		{{{
			algorithm_angluin::simple_row<answer, vector<answer> > row;
			bool done = false;

			if(check_uniq) {
				if(this->search_upper_table(word) != this->upper_table.end()) {
					return;
				}
				typename list< algorithm_angluin::simple_row<answer, vector<answer> > >::iterator ti;
				ti = this->search_lower_table(word);
				if(ti != this->lower_table.end()) {
					done = true;
					this->upper_table.push_back(*ti);
					this->lower_table.erase(ti);
				}
			}

			// add the word to the upper table
			if(!done) {
				row.index = word;
				this->upper_table.push_back(row);
			}

			// add all suffixes of word to lower table
			for( int i = 0; i < this->alphabet_size; i++ ) {
				word.push_back(i);
				done = false;
				if(check_uniq)
					if(this->search_upper_table(word) != this->upper_table.end()) {
						// can't be in lower table, as its prefix would be in upper then
						done = true;
					}
				if(!done) {
					row.index = word;
					this->lower_table.push_back(row);
				}
				word.pop_back();
			}
		}}}

};


}; // end of namespace libalf

#endif

