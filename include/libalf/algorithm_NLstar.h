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
					acceptances::iterator ai1, ai2;
					for(ai1 = this->acceptance.begin(), ai2 = other.acceptance.begin(); ai1 != this->acceptance.end() && ai2 != other.acceptance.end(); ai1++, ai2++)
						if(*ai1 == false && *ai2 == true)
							return false;
					return ( ai1 == this->acceptance.end() && ai2 == other.acceptance.end() );
				}}}
				bool __attribute__((const)) mutual_noncover(table_row & other)
				{{{
					acceptances::iterator ai1, ai2;
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
						return tC0 && oCt;
					else
						return false;
				}}}
				bool __attribute__((const)) operator==(table_row & other)
				{{{
					acceptances::iterator ai1, ai2;
					for(ai1 = this->acceptance.begin(), ai2 = other.acceptance.begin(); ai1 != this->acceptance.end() && ai2 != other.acceptance.end(); ai1++, ai2++)
						if(*ai1 != *ai2)
							return false;

					return true;
				}}}
				bool __attribute__((const)) operator!=(table_row & other)
				{{{
					return ( ! (*this == other) );
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

				string tostring();
				{{{
					string s;
					list<answer>::iterator acci;

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

					s += sizeof(list<int>) + sizeof(int) * index.length();
					s += sizeof(acceptances) + sizeof(answer) * acceptances.length();

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
			set_alphabet_size(alphabet_size);
			set_logger(log);
			set_knowledge_source(base);
			initialized = false;
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{
			
		}

		virtual void get_memory_statistics(statistics & stats)
		{{{
			// get memory footprint:
			columnlist::iterator ci;
			table::iterator ti;

			stats.bytes = sizeof(this);
			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				stats.bytes += sizeof(list<int>) + sizeof(int) * (ci->length());
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++) {
				stats.bytes += ti->memory_usage();
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++) {
				stats.bytes += ti->memory_usage();
			// members
			stats.columns = column_names.length();
			stats.upper_table = upper_tables.length();
			stats.lower_table = lower_tables.length();
			stats.members = stats.columns * (stats.upper_table + stats.lower_table);
			stats.words = stats.members;
		}}}

		virtual bool sync_to_knowledgebase()
		{{{
			(*this->my_logger)(LOGGER_ERROR, "NL* does not support sync-operation (undo) for now.\n");
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
			columntlist::iterator ci;
			table::iterator ti;

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
		}}}

		virtual bool conjecture_ready()
		{
			
		}

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
			if(sigma+1 > get_alphabet_size())
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

		virtual void add_word_to_upper_table(list<int> word, bool check_uniq = true)
		{
			
		}

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

		// close table: perform operations to close it.
		// returns true if table was closed.
		// returns false if table was changed (and thus needs to be filled)
		virtual bool close()
		{
			
		}

		// make table consistent: perform operations to do that.
		// returns true if table was consistent.
		// returns false if table was changed (and thus needs to be filled)
		virtual bool make_consistent()
		{
			
		}

		virtual bool complete()
		{{{
			if(!initialized)
				initialize_table();

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

		virtual bool derive_automaton(automaton_constructor * automaton)
		{
			
		}

};

}; // end of namespace libalf

#endif // __libalf_algorithm_NLstar_h__

