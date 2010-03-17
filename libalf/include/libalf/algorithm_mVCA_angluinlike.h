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

		class sample_list : public list<list<int> > {
			// list of all samples (suffixes) for a given cv.
			public:
				typedef typename list<list<int> >::iterator iterator;
				typedef typename list<list<int> >::reverse_iterator reverse_iterator;

				bool add_sample(const list<int> & sample)
				{{{
					reverse_iterator li;
					for(li = this->rbegin(); li != this->rend(); ++li)
						if(*li == sample)
							return false;
					this->push_back(sample);
					return true;
				}}}

				int get_dynamic_memory_consumption()
				{{{
					int ret = 0;
					iterator li;
					list<int>::iterator i;
					for(li = this->begin(); li != this->end(); ++li) {
						ret += sizeof(list<int>);
						for(i = li->begin(); i != li->end(); ++i)
							ret += sizeof(int);
					}

					return ret;
				}}}
		};

		class equivalence_approximation : public triple<list<int>, int, vector<answer> > {
			public:
				inline list<int>	& prefix()		{ return this->first; };
				inline int		& cv()			{ return this->second; };
				inline vector<answer>	& acceptances()		{ return this->third; };

				inline equivalence_approximation()
				{ };

				inline equivalence_approximation(const list<int> & prefix, int cv)
				{ this->prefix() = prefix; this->cv() = cv; };

				pair<int, vector<answer> > footprint()
				{
					pair<int, vector<answer> > ret;
					ret.first = cv();
					ret.second = acceptances();
					return ret;
				};

				inline bool equivalent(equivalence_approximation & other)
				{ return cv() == other.cv() && acceptances() == other.acceptances(); }

				inline bool equivalent(equivalence_approximation & other, typename sample_list::iterator suffix_it)
				{{{
					typename vector<answer>::iterator vi1, vi2;
					vi1 = acceptances().begin();
					vi2 = other.acceptances().begin();

					while(vi1 != acceptances().end() && vi2 != other.acceptances().end()) {
						if(*vi1 != *vi2)
							return false;

						++vi1;
						++vi2;
						++suffix_it;
					}
					return (vi1 == acceptances().end() && vi2 == other.acceptances().end());
				}}}

				bool fill(sample_list & samples, knowledgebase<answer> * base)
				{{{
					typename vector<answer>::iterator acci;
					typename sample_list::iterator sui;

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

					os << "\t\t";
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

				int get_dynamic_memory_consumption()
				{{{
					int ret = 0;

					typename list<int>::iterator li;
					typename vector<answer>::iterator acci;

					for(li = prefix().begin(); li != prefix().end(); ++li)
						ret += sizeof(int);

					for(acci = acceptances().begin(); acci != acceptances().end(); ++acci)
						ret += sizeof(answer);

					return ret;
				}}}
		};

		class equivalence_table : public list<equivalence_approximation> {
			public:
				typedef typename list<equivalence_approximation>::iterator iterator;

				iterator find_prefix(const list<int> & prefix)
				{{{
					iterator r;
					r = this->begin();
					while(r != this->end()) {
						if(r->prefix() == prefix)
							break;
						++r;
					}
					return r;
				}}}

				iterator find_or_insert_prefix(const list<int> & prefix, int cv)
				{{{
					iterator r;

					r = find_prefix(prefix);
					if(r != this->end()) {
						if(r->cv() != cv)
							return this->end();
						else
							return r;
					}

					this->push_back(equivalence_approximation(prefix, cv));
					r = this->end();
					--r;
					return r;
				}}}

				iterator find_equivalence_class(equivalence_approximation & representative)
				{{{
					iterator equi;
					for(equi = this->begin(); equi != this->end(); ++equi)
						if(equi->equivalent(representative))
							break;
					return equi;
				}}}

				bool fill(sample_list & samples, knowledgebase<answer> * base)
				{{{
					iterator equi;
					bool complete = true;

					for(equi = this->begin(); equi != this->end(); ++equi)
						if(!equi->fill(samples, base))
							complete = false;;

					return complete;
				}}}

				void print(ostream & os)
				{{{
					iterator it;
					for(it = this->begin(); it != this->end(); ++it) {
						it->print(os);
					}
				}}}

				int get_dynamic_memory_consumption()
				{{{
					int ret = 0;
					iterator equi;
					for(equi = this->begin(); equi != this->end(); ++equi) {
						ret += sizeof(equivalence_approximation);
						ret += equi->get_dynamic_memory_consumption();
					}
					return ret;
				}}}
		};

		class m_representatives : public triple<sample_list, equivalence_table, triple<equivalence_table, equivalence_table, equivalence_table> > {
			public: // methods
				inline sample_list & samples()			{ return this->first; }; // suffixes
				inline equivalence_table & representatives()	{ return this->second; }; // prefixes
				inline equivalence_table & returning_tr()	{ return this->third.first; }; // returning transitions
				inline equivalence_table & internal_tr()	{ return this->third.second; }; // internal transitions
				inline equivalence_table & calling_tr()		{ return this->third.third; }; // calling transitions

				int get_dynamic_memory_consumption()
				{{{
					int ret;

					ret += samples().get_dynamic_memory_consumption();

					ret += representatives().get_dynamic_memory_consumption();

					ret += returning_tr().get_dynamic_memory_consumption();
					ret += internal_tr().get_dynamic_memory_consumption();
					ret += calling_tr().get_dynamic_memory_consumption();

					return ret;
				}}}
		};

		class stratified_observationtable : public vector<m_representatives> {
			public:
				typedef typename vector<m_representatives>::iterator iterator;
				typedef pair<equivalence_table *, typename equivalence_table::iterator> location;

				bool fill(knowledgebase<answer> * base)
				{{{
					bool complete = true;
					iterator previous, current, next;

					// initialize iterators
					previous = this->end();
					current = this->begin();
					next = this->begin();
					if(next != this->end())
						++next;

					while(current != this->end()) {
						// fill current table:
						if(!current->representatives().fill(current->samples(), base))
							complete = false;
						// and transition-tables:
						if(previous != this->end())
							if(!current->returning_tr().fill(previous->samples(), base))
								complete = false;
						if(!current->internal_tr().fill(current->samples(), base))
							complete = false;
						if(next != this->end())
							if(!current->calling_tr().fill(next->samples(), base))
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
					iterator vi;

					os << "stratified_observationtable {\n";

					for(vi = this->begin(), cv = 0; vi != this->end(); ++cv, ++vi) {
						// samples
						os << "\tcv = " << cv << ":\n";
						os << "\t  Samples:";
						typename sample_list::iterator si;
						for(si = vi->samples().begin(); si != vi->samples().end(); ++si) {
							os << " ";
							print_word(os, *si);
						}
						os << " ;\n";

						// representatives
						if(!vi->representatives().empty()) {
							os << "\t  Representatives:\n";
							vi->representatives().print(os);
						}

						os << "\t  Transitions:\n";
						// returning tr
						if(!vi->returning_tr().empty()) {
							os << "\t    returning (with true cv=" << cv - 1 << "):\n";
							vi->returning_tr().print(os);
						}

						// internal tr
						if(!vi->internal_tr().empty()) {
							os << "\t    internal:\n";
							vi->internal_tr().print(os);
						}

						// calling tr
						if(!vi->calling_tr().empty()) {
							os << "\t    calling (with true cv=" << cv + 1 << "):\n";
							vi->calling_tr().print(os);
						}
					}

					os << "};\n";
				}}}

				int get_dynamic_memory_consumption()
				{{{
					int ret = 0;
					iterator vi;

					for(vi = this->begin(); vi != this->end(); ++vi) {
						ret += sizeof(m_representatives);
						ret += vi->get_dynamic_memory_consumption();
					}

					return ret;
				}}}

				int count_words()
				{{{
					int words = 0;

					iterator previous, current, next;

					// initialize iterators
					previous = this->end();
					current = this->begin();
					next = this->begin();
					if(next != this->end())
						++next;

					while(current != this->end()) {
						// current table:
						words += current->representatives().size() * current->samples().size();

						// transition-tables:
						if(previous != this->end())
							words += current->returning_tr().size() * previous->samples().size();
						words += current->internal_tr().size() * current->samples().size();
						if(next != this->end())
							words += current->calling_tr().size() * next->samples().size();

						// increment iterators
						if(previous == this->end())
							previous = this->begin();
						else
							++previous;
						++current;
						if(next != this->end())
							++next;
					}

					return words;
				}}}

				location find_representative(const list<int> & rep, int cv, bool & exists)
				{{{
					location ret;

					ret.first = & ( this->operator[](cv).representatives() );
					ret.second = ret.first->find_prefix(rep);
					exists = (ret.second != ret.first->end());

					return ret;
				}}}

				location find_transition(const list<int> & transition, int cv_without_transition, int sigma_direction, bool & exists)
				{{{
					location ret;


					switch(sigma_direction) {
						case -1:
							ret.first = & ( this->operator[](cv_without_transition).returning_tr() );
							break;
						case 0:
							ret.first = & ( this->operator[](cv_without_transition).internal_tr() );
							break;
						case +1:
							ret.first = & ( this->operator[](cv_without_transition).calling_tr() );
							break;
					}
					ret.second = ret.first->find_prefix(transition);
					exists = (ret.second != ret.first->end());

					if(!exists) {
						ret.first = & ( this->operator[](cv_without_transition + sigma_direction).representatives() );
						ret.second = ret.first->find_prefix(transition);
						exists = (ret.second != ret.first->end());
					}

					return ret;
				}}}
		};

	protected: // data
		bool initialized;

		map<int, int> pushdown_directions; // maps a label to its pushdown direction:
		// -1 == down == return ; 0 == stay == internal ; +1 == up == call

		int known_equivalence_bound; // the m for which the current data is isomorphic to the model
		int tested_equivalence_bound; // the m for which we tested all corresponding mVCAs
		int full_eq_test_current_m;

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
			tested_equivalence_bound = -1;
			full_eq_test_current_m = -1;
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
		{{{
			stat["initialized"] = initialized;
			if(initialized) {
				int words = 0; // words that are stored in the tables
				int bytes = 0; // bytes this algorithm consumes over all

				bytes = sizeof(*this);
				bytes += table.get_dynamic_memory_consumption();
				words = table.count_words();

				stat["words"] = words;
				stat["bytes"] = bytes;
				stat["table bound"] = (int)table.size() - 1;
				stat["known equivalence bound"] = known_equivalence_bound;
			}
		}}}

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
			ret += ::serialize(tested_equivalence_bound);
			ret += ::serialize(full_eq_test_current_m);
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
			if(!::deserialize(tested_equivalence_bound, serial)) goto deserialization_failed;
			if(!::deserialize(full_eq_test_current_m, serial)) goto deserialization_failed;
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
		{ return complete(); };

		virtual bool indicate_partial_equivalence(int bound)
		{{{
			if(bound < known_equivalence_bound) {
				(*this->my_logger)(LOGGER_ERROR, "mVCA_angluinlike: you indicated a partial equivalence with m=%d, but equivalence was already indicated for m=%d!", bound, known_equivalence_bound);
				return false;
			} else {
				known_equivalence_bound = bound;
				return true;
			}
		}}}

		virtual bool add_counterexample(list<int> counterexample)
		{{{
			if(tested_equivalence_bound == known_equivalence_bound) {
				// counterexample is for latest partial equivalence query
				return add_partial_counterexample(counterexample);
			} else {
				// counterexample is for latest full equivalence query
				return add_full_counterexample(counterexample);
			}
		}}}

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

		typename equivalence_table::iterator insert_representative(list<int> rep, bool & success)
		{{{
			typename equivalence_table::iterator new_rep = table[0].representatives().end();
			int cv = countervalue(rep);

			typename equivalence_table::iterator ti;
			typename stratified_observationtable::location loc;

			if(cv < 0) {
				success = false;
				return new_rep;
			}
			if(cv >= (int)table.size()) {
				// FIXME: possibly add prefixes along empty tables?
				table.resize(cv+1);
			}

			// fail if it already is in the table.
			bool found;
			loc = table.find_representative(rep, cv, found);
			if(found) {
				success = false;
				return new_rep;
			}

			// check if there is a matching transition
			found = false;
			if(rep.size() > 0) {
				int cvd;
				cvd = pushdown_directions[rep.back()];
				loc = table.find_transition(rep, cv - cvd, cvd, found);
			}

			new_rep = table[cv].representatives().find_or_insert_prefix(rep, cv);

			if(found) {
				// if there is a matching transition, copy data from it and delete it
				new_rep->acceptances() = loc.second->acceptances();
				loc.first->erase(loc.second);
			}

			for(int sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
				int ncv = cv;
				ncv += pushdown_directions[sigma];
				if(ncv < 0)
					continue;
				if(ncv > (int)table.size()) // NOTE: if == we insert so we don't loose the transition if the table is increased at some point.
					continue;
				rep.push_back(sigma);
				switch(pushdown_directions[sigma]) {
					case -1:
						table[cv].returning_tr().find_or_insert_prefix(rep, ncv);
						break;
					case 0:
						table[cv].internal_tr().find_or_insert_prefix(rep, ncv);
						break;
					case 1:
						table[cv].calling_tr().find_or_insert_prefix(rep, ncv);
						break;
				};
				rep.pop_back();
			}
			success = true;
			return new_rep;
		}}}

		bool add_partial_counterexample(list<int> & counterexample)
		{{{
			list<int> suffix;
			int cv = countervalue(counterexample);
			typename stratified_observationtable::location loc;
			bool success;

			if(cv != 0) {
				(*this->my_logger)(LOGGER_ERROR, "mVCA_angluinlike: bad counterexample to partial equivalence query:"
								" %s has countervalue %d. should be 0.\n",
								word2string(counterexample).c_str(), cv);
				return false;
			}

			loc = table.find_representative(counterexample, cv, success);
			if(success) {
				(*this->my_logger)(LOGGER_ERROR, "mVCA_angluinlike: counterexample to partial equivalence query"
								" %s is already contained in tables!\n",
								word2string(counterexample).c_str());
				return false;
			}

			cv = 0;
			suffix.swap(counterexample); // we have to do this in length-increasing order, otherwise the prefix-closedness is violated

			while(!suffix.empty()) {
				insert_representative(counterexample, success);
				table[cv].samples().add_sample(suffix);

				int sigma = suffix.front();
				suffix.pop_front();
				cv += pushdown_directions[sigma];
				counterexample.push_back(sigma);
			}

			insert_representative(counterexample, success);

			return true;
		}}}

		bool add_full_counterexample(list<int> & counterexample)
		{
			if(full_eq_test_current_m < 0) {
				(*this->my_logger)(LOGGER_ERROR, "mVCA_angluinlike: giving counterexample to full eq query, but the latest m tested was -1 ?!"
						" either you gave a counterexample right after indicating a new partial equivalence,"
						" or this is an internal bug. in the latter case, please create a testcase and send it to the developers.\n");
				return false;
			}

			// FIXME
			
			full_eq_test_current_m = -1;
			return false;
		}

		virtual void initialize_table()
		{{{
			if(!initialized) {
				m_representatives mr;
				table.push_back(mr);
				list<int> epsilon;
				table[0].samples().add_sample(epsilon);
				bool success;
				insert_representative(epsilon, success);
				initialized = true;
			}
		}}}

		bool fill_missing_columns()
		{ return table.fill(this->my_knowledge); }

		bool close()
		// close() checks that all transitions have a corresponding equivalence class in the representatives
		{{{
			bool no_changes = true;

			typename stratified_observationtable::iterator vi;
			int cv;
			for(cv = 0, vi = table.begin(); vi != table.end(); ++vi, ++cv) {
				typename equivalence_table::iterator equi, repi;
				bool success;
				// returning transitions
				if(cv-1 >= 0) {
					for(equi = vi->returning_tr().begin(); equi != vi->returning_tr().end(); ++equi) {
						if(table[equi->cv()].representatives().find_equivalence_class(*equi) == table[equi->cv()].representatives().end()) {
							insert_representative(equi->prefix(), success)->acceptances() = equi->acceptances();
							no_changes = false;
							break;
						}
					}
				}
				// internal transitions
				{
					for(equi = vi->internal_tr().begin(); equi != vi->internal_tr().end(); ++equi) {
						if(table[equi->cv()].representatives().find_equivalence_class(*equi) == table[equi->cv()].representatives().end()) {
							insert_representative(equi->prefix(), success)->acceptances() = equi->acceptances();
							no_changes = false;
							break;
						}
					}
				}
				// calling transitions
				if(cv + 1 < (int)table.size()) {
					for(equi = vi->calling_tr().begin(); equi != vi->calling_tr().end(); ++equi) {
						if(table[equi->cv()].representatives().find_equivalence_class(*equi) == table[equi->cv()].representatives().end()) {
							insert_representative(equi->prefix(), success)->acceptances() = equi->acceptances();
							no_changes = false;
							break;
						}
					}
				}
			}

			return no_changes;
		}}}

		bool make_consistent()
		// make_consistent() checks that all transitions of two equivalent representatives a, b are equivalent as well
		{{{
			bool no_changes = true;

			typename stratified_observationtable::iterator vi;
			for(vi = table.begin(); vi != table.end(); ++vi) {
				typename equivalence_table::iterator a, b;
				for(a = vi->representatives().begin(); a != vi->representatives().end(); ++a) {
					b = a;
					++b;
					while(b != vi->representatives().end()) {
						if(a->equivalent(*b)) {
							list<int> wa, wb;
							wa = a->prefix();
							wb = b->prefix();
							// check that all transitions are equivalent as well
							for(int sigma = 0; sigma < this->get_alphabet_size(); ++sigma) {
								int cvd, ncv;
								cvd = pushdown_directions[sigma];
								ncv = a->cv() + cvd;
								if(ncv < 0 || ncv >= (int)table.size())
									continue;

								wa.push_back(sigma);
								wb.push_back(sigma);

								typename stratified_observationtable::location as, bs;

								bool success;
								as = table.find_transition(wa, a->cv(), cvd, success);
								bs = table.find_transition(wb, b->cv(), cvd, success);

								typename sample_list::iterator bad_suffix;
								bad_suffix = table[ncv].samples().begin();
								if(!as.second->equivalent(*bs.second, bad_suffix)) {
									list<int> new_suffix;
									new_suffix = *bad_suffix;
									new_suffix.push_front(sigma);
									table[ncv].samples().push_back(new_suffix);
									no_changes = false;
								}

								wa.pop_back();
								wb.pop_back();
							}
						}
						++b;
					}
				}
			}

			return no_changes;
		}}}

		virtual bool complete()
		{{{
			if(!initialized)
				initialize_table();

			if(!fill_missing_columns())
				return false;

			(*this->my_logger)(LOGGER_ALGORITHM, "%s", to_string().c_str());

			if(!close()) {
				(*this->my_logger)(LOGGER_ALGORITHM, "closing...\n");
				return complete();
			}

			if(!make_consistent()) {
				(*this->my_logger)(LOGGER_ALGORITHM, "making consistent...\n");
				return complete();
			}

			return true;
		}}}

		conjecture * create_partial_equivalence_query()
		{{{
			bounded_mVCA * cj = new bounded_mVCA;

			cj->valid = true;
			cj->is_deterministic = true;
			cj->alphabet_size = this->get_alphabet_size();
			cj->state_count = 0; // done on the fly

			cj->m_bound = known_equivalence_bound + 1;

			map<pair<int, vector<answer> >, int> states; // this is not really good. something better anyone?

			// generate statemap and mark initial and final states
			typename stratified_observationtable::iterator vi;
			typename equivalence_table::iterator equi;

			for(vi = table.begin(); vi != table.end(); ++vi) {
				for(equi = vi->representatives().begin(); equi != vi->representatives().end(); ++equi) {
					pair<int, vector<answer> > footprint = equi->footprint();
					if(states.find(footprint) == states.end()) {
						states[footprint] = cj->state_count;
						if(equi->prefix().empty())
							cj->initial.insert(cj->state_count);
						if((footprint.first == 0) && (true == (bool)(footprint.second[0])))
							cj->final.insert(cj->state_count);
						++cj->state_count;

					}

				}
			}

			// list all transitions
			pair<pair<int, int>, int> new_transition; // state, sigma -> state
			for(vi = table.begin(); vi != table.end(); ++vi) {
				for(equi = vi->representatives().begin(); equi != vi->representatives().end(); ++equi) {
					new_transition.first.first = states[equi->footprint()];
					list<int> rep;
					rep = equi->prefix();
					for(int sigma = 0; sigma < this->get_alphabet_size(); ++sigma) {
						int dcv = pushdown_directions[sigma];
						int ncv = equi->cv() + dcv;
						if(ncv < 0 || ncv >= (int)table.size())
							continue;

						rep.push_back(sigma);
						new_transition.first.second = sigma;

						pair<int, vector<answer> > footprint = equi->footprint();
						bool found;

						footprint = table.find_transition(rep, equi->cv(), dcv, found).second->footprint();

						new_transition.second = states[footprint];
						cj->transitions.insert(new_transition);
						rep.pop_back();
					}
				}
			}

			return cj;
		}}}

		conjecture * create_full_equivalence_query()
		{
			// FIXME
			simple_mVCA * cj = new simple_mVCA;

			cj->valid = true;
			cj->is_deterministic = true;
			cj->alphabet_size = this->get_alphabet_size();
			cj->alphabet_directions = pushdown_directions;
			cj->initial_state = 0;
//			cj->state_count =   
//			cj->final_states   
//			cj->m_bound =   
//			cj->transitions   

			// try to find a repeating strucure in the BG, i.e. identify an isomorphism between levels
			

			// if there is one, create mVCA
			

			// if there is none, try the whole BG as an automaton without repeating structure
			


			return /* cj */ NULL;
		}

		virtual conjecture * derive_conjecture()
		{{{
			if(tested_equivalence_bound == known_equivalence_bound)
				return create_partial_equivalence_query();
			else
				return create_full_equivalence_query();
		}}}
};


}; // end of namespace libalf

#endif

