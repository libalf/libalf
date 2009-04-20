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

#ifndef __libalf_algorithm_NLstar_h__
# define __libalf_algorithm_NLstar_h__

#include <list>
#include <vector>
#include <string>
#include <ostream>

#include <libalf/learning_algorithm.h>
#include <libalf/automaton_constructor.h>

namespace libalf {

using namespace std;

// NLstar, angluin-style learning algorithm for RFSA-automata
// see "B. Bollig, P. Habermehl, C. Kern, M. Leucker - Angluin-Style Learning of NFA"
// ( http://www.lsv.ens-cachan.fr/Publis/RAPPORTS_LSV/PDF/rr-lsv-2008-28.pdf )

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
					it++; if(size <= 0 || it == limit) return false;

					// index
					if( ! deserialize_word(this->index, it, limit) )
						goto deserialization_failed;
					size -= this->index.size() + 1;
					if(size <= 0 || it == limit) goto deserialization_failed;

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

				string tostring()
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

					s += sizeof(list<int>) + sizeof(int) * index.size();
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

		virtual void get_memory_statistics(statistics & stats)
		{{{
			// get memory footprint:
			typename columnlist::iterator ci;
			typename table::iterator ti;

			stats.table_size.bytes = sizeof(this);
			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				stats.table_size.bytes += sizeof(list<int>) + sizeof(int) * (ci->size());
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				stats.table_size.bytes += ti->memory_usage();
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++)
				stats.table_size.bytes += ti->memory_usage();
			// members
			stats.table_size.columns = column_names.size();
			stats.table_size.upper_table = upper_table.size();
			stats.table_size.lower_table = lower_table.size();
			stats.table_size.members = stats.table_size.columns * (stats.table_size.upper_table + stats.table_size.lower_table);
			stats.table_size.words = stats.table_size.members;
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
		{
			
		}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
			
		}

		virtual void print(ostream &os)
		{{{
			os << this->tostring();
		}}}

		virtual string tostring()
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
				s += ti->tostring();
			}
			s += "\n\tlower table:\n";
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++) {
				s += "\t\t";
				s += ti->tostring();
			}
			s += "}\n";

			return s;
		}}}

		virtual bool conjecture_ready()
		{{{
			return initialized && columns_filled() && is_closed() && is_consistent();
		}}}

		virtual void add_counterexample(list<int> w)
		{{{
			// add counterexample and all its suffixes to the columns
			int sigma = -1;
			while(!w.empty()) {
				if(!add_column(w)) {
					// if a prefix is already in, we dont need
					// to check the other. column_names is prefix-closed.
					break;
				}
				if(w.front() > sigma)
					sigma = w.front();
				w.pop_front();
			}
			if(sigma+1 > this->get_alphabet_size())
				increase_alphabet_size(sigma+1);
		}}}

	protected:
		// returns true if word was added, false if it was already in column_names
		virtual bool add_column(list<int> word)
		{{{
			typename columnlist::iterator ci;
			list<int> nw;

			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				if(*ci == nw)
					return false;

			column_names.push_back(nw);
			return true;
		}}}

		virtual bool row_is_prime(typename table::iterator & row)
		// TODO: efficiency
		{{{
			table_row merge;
			int cn = column_names.size();
			int i;

			// initialize merge-row
			answer a_false;
			a_false = false;
			for(i = 0; i < cn; i++)
				merge.acceptance.push_back(a_false);

			// join all covered rows into merge-row
			typename table::iterator ti;
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				if(ti != row)
					if(row->covers(*ti))
						merge |= *ti;

			// quick check if we are done
			if(merge == *row)
				return true;

			for(ti = lower_table.begin(); ti != lower_table.end(); ti++)
				if(ti != row)
					if(row->covers(*ti))
						merge |= *ti;

			// if they are equal now, *row is composed from other rows
			return (merge != *row);
		}}}

		virtual void add_word_to_upper_table(list<int> & word, bool check_uniq = true)
		{{{
			table_row row;
			bool done;

			if(check_uniq) {
				if(this->search_upper_table(word) != this->upper_table.end())
					return;

				typename table::iterator ti;
				ti = this->search_lower_table(word);
				if(ti != this->lower_table.end()) {
					// word is already in lower. so move it up.
					done = true;
					this->upper_table.push_back(*ti);
					this->lower_table.erase(ti);
				}
			}

			if(!done) {
				row.index = word;
				this->upper_table.push_back(row);
			}

			// add all suffixes to lower table
			for(int sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
				row.index.push_back(sigma);

				if(check_uniq) {
					if(search_upper_table(word) == upper_table.end())
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
			add_word_to_upper_table(word, false);

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

				typename list<typename table::iterator>::iterator pri;
				for(pri = upper_primes.begin(); pri != upper_primes.end(); pri++)
					if(ti->covers(**pri))
						merge |= *ti;

				if(merge != *ti) {
					// this row is not composed from upper primes!
					return false;
				}
			}

			return true;
		}}}

		// close table: perform operations to close it.
		// returns true if table was closed.
		// returns false if table was changed (and thus needs to be filled)
		virtual bool close()
		{
			bool changed = false;

			

			return changed;
		}

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
					if(uti1->covers(*uti2))
						if(!w1suffixes_cover_w2suffixes(uti1->index, uti2->index))
								return false;
					if(uti2->covers(*uti1))
						if(!w1suffixes_cover_w2suffixes(uti2->index, uti1->index))
								return false;
				}
			}

			return true;
		}}}

		// make table consistent: perform operations to do that.
		// returns true if table was consistent.
		// returns false if table was changed (and thus needs to be filled)
		virtual bool make_consistent()
		{
			bool changed = false;

			

			return false;
		}

		virtual bool complete()
		{{{
			if(!initialized)
				initialize_table();

			if(fill_missing_columns(upper_table) && fill_missing_columns(lower_table)) {
				if(!close())
					return complete();

				if(!make_consistent())
					return complete();

				return true;
			} else {
				return false;
			}
		}}}

		virtual bool derive_automaton(automaton_constructor * automaton)
		{{{
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

			set<int> initial;
			set<int> final;
			transition_set transitions;

			// get all distinguished states
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++)
				if(row_is_prime(ti))
					upper_primes.push_back(ti);

			for(unsigned int i = 0; i < upper_primes.size(); i++) {
				list<int> succ_w;
				typename table::iterator successor;
				transition tr;

				// get initial states
				if(epsilon->covers(*upper_primes[i]))
					initial.insert(i);

				// get final states (column 0 is always epsilon)
				if(upper_primes[i]->acceptance.front() == true)
					final.insert(i);

				// and all transitions from this state
				tr.source = i;
				succ_w = upper_primes[i]->index;
				for(unsigned int sigma = 0; sigma < this->get_alphabet_size(); sigma++) {
					succ_w.push_back(sigma);
					successor = search_tables(succ_w);
					succ_w.pop_back();
					tr.label = sigma;

					for(int dst = 0; dst < upper_primes.size(); dst++) {
						if(successor->covers(*upper_primes[dst])) {
							tr.destination = dst;
							transitions.insert(tr);
						}
					}

				}
			}

			return automaton->construct(false, this->get_alphabet_size(), upper_primes.size(), initial, final, transitions);
		}}}

};

}; // end of namespace libalf

#endif // __libalf_algorithm_NLstar_h__

