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


/*
 * RPNI (regular positive, negative inference) is an offline learning algorithm,
 * described in e.g.
 *	* P. Graćıa and J. Oncina. Inferring regular languages in polynomial update time
 *	* D. Neider, Learning Automata for Streaming XML Documents
 */


#ifndef __libalf_algorithm_rpni_h__
# define __libalf_algorithm_rpni_h__

#include <string>
#include <list>
#include <map>

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

// basic biermann functions. do not use this directly, use those derived from it
// (declared below or in other files)
template <class answer>
class RPNI : public learning_algorithm<answer> {
	public:	// types

	protected: // data

	public: // methods
		RPNI()
		{
		}
		virtual ~RPNI()
		{
		}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			this->set_alphabet_size(new_asize);
		}}}

		virtual void get_memory_statistics(statistics & stats)
		{ /* FIXME: maybe keep some stats from last run? */ }

		virtual bool sync_to_knowledgebase()
		{{{
			return true;
		}}}

		virtual bool supports_sync()
		{{{
			return true;
		}}}

		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;

			// we don't have any internal, persistent data
			ret += htonl(1);
			ret += htonl(learning_algorithm<answer>::ALG_RPNI);

			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			if(it == limit) return false;
			if(ntohl(*it) != 1)
				return false;

			it++; if(it == limit) return false;
			if(ntohl(*it) != learning_algorithm<answer>::ALG_RPNI)
				return false;

			it++;

			return true;
		}}}

		virtual void print(ostream &os)
		{
		}
		virtual string tostring()
		{
		}

		// conjecture is always ready if there is a non-empty knowledgebase
		virtual bool conjecture_ready()
		{{{
			return ( (this->my_knowledge != NULL) && (this->my_knowledge->count_answers() > 0) );
		}}}

		// stubs for counterexample will throw a warning to the logger
		virtual void add_counterexample(list<int>)
		{{{
			(*this->my_logger)(LOGGER_ERROR, "algorithm_RPNI does not support counter-examples, as it is an offline-algorithm. please add the counter-example directly to the knowledgebase and rerun the algorithm.\n");
		}}}

	protected:
		virtual bool complete()
		{{{
			// we're offline.
			return true;
		}}}
		// derive an automaton from data structure
		virtual bool derive_automaton(bool & t_is_dfa, int & t_alphabet_size, int & t_state_count, set<int> & t_initial, set<int> & t_final, multimap<pair<int, int>, int> & t_transitions)
		{
		}

};


}; // end namespace libalf

#endif // __libalf_algorithm_rpni_h__

