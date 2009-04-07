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
#include <libalf/automata.h>

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

				void print(ostream &os)
				{{{
					list<answer>::iterator acci;

					print_word(os, index);
					os << ": ";
					for(acci = acceptance.begin(); acci != acceptance.end(); acci++) {
						if(*acci == true)
							os << "+ ";
						else
							if(*acci == false)
								os << "- ";
							else
								os << "? ";
					}
					os << ";\n";
				}}}
		};

		typedef list<table_row> table;

	protected:
		columnlist column_names;

		table upper_table;
		table lower_table;

		bool initialized;

	public:
		NLstar_table(teacher<answer> *teach, knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			set_alphabet_size(alphabet_size);
			set_logger(log);
			set_knowledge_source(teach, base);
			initialized = false;
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{
			
		}

		virtual void get_memory_statistics(statistics & stats)
		{
			
		}

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
		{
			
		}

		virtual string tostring()
		{
			
		}

		virtual bool conjecture_ready()
		{
			
		}

	protected:
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

		virtual bool complete()
		{
			
		}

		virtual bool derive_automaton(finite_language_automaton * automaton)
		{
			
		}

};

}; // end of namespace libalf

#endif // __libalf_algorithm_NLstar_h__

