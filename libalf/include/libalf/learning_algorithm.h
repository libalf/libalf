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

#ifndef __libalf_learning_algorithm_h__
# define __libalf_learning_algorithm_h__

#include <sys/time.h>
#ifdef _WIN32
// timersub is copied from linux, sys/time.h!
# ifndef timersub
#  define timersub(a, b, result)                                   \
	do {                                                       \
		(result)->tv_sec = (a)->tv_sec - (b)->tv_sec;      \
		(result)->tv_usec = (a)->tv_usec - (b)->tv_usec;   \
		if ((result)->tv_usec < 0) {                       \
			--(result)->tv_sec;                        \
			(result)->tv_usec += 1000000;              \
		}                                                  \
	} while (0)
# endif
#else
# include <sys/resource.h>
#endif

#include <list>
#include <set>
#include <map>
#include <utility>
#include <string>

#include <libalf/logger.h>
#include <libalf/statistics.h>
#include <libalf/knowledgebase.h>
#include <libalf/normalizer.h>

namespace libalf {

using namespace std;

// basic interface for different implementations (e.g. one table and one tree)
template <class answer>
class learning_algorithm {
	protected: // data
		ignore_logger ignore;
		logger * my_logger;

		bool do_timing; // create timing statistics?
		bool in_timing;	// currently measuring timing information?
		struct timeval start_utime, start_stime; // when measuring did begin
		timing_statistics current_stats;

		knowledgebase<answer> * my_knowledge;

		normalizer * norm;

		int alphabet_size;

	public:	// types
		enum algorithm {
			ALG_NONE = 0,
			ALG_ANGLUIN = 1,
			ALG_ANGLUIN_COLUMN = 2,
			ALG_KVTREE = 3,
			ALG_BIERMANN = 4,
			ALG_BIERMANN_ANGLUIN = 5,
			ALG_NL_STAR = 6,
			ALG_RPNI = 7,
			ALG_DELETE2 = 8,
			ALG_LAST_INVALID = 9
		};

	public: // methods
		learning_algorithm()
		{{{
			my_knowledge = NULL;
			norm = NULL;
			do_timing = false;
			in_timing = false;
			reset_timing();
			set_logger(NULL);
		}}}
		virtual ~learning_algorithm() { };

		// set_alphabet_size() is only for initial setting.
		// once any data is in the structure, use increase_alphabet_size ONLY.
		virtual void set_alphabet_size(int alphabet_size)
		{{{
			this->alphabet_size = alphabet_size;
		}}}
		virtual int get_alphabet_size()
		{{{
			return alphabet_size;
		}}}

		virtual void increase_alphabet_size(int new_asize) = 0;


		virtual void set_logger(logger * l)
		{{{
			if(l)
				my_logger = l;
			else
				my_logger = (&this->ignore);
		}}}

		virtual void set_knowledge_source(knowledgebase<answer> *base)
		// set a source for all membership information.
		{{{
			my_knowledge = base;
		}}}

		virtual knowledgebase<answer> * get_knowledge_source()
		{{{
			return my_knowledge;
		}}}

		virtual void set_normalizer(normalizer * norm)
		{{{
			if(this->norm)
				delete this->norm;
			this->norm = norm;
		}}}
		virtual normalizer * get_normalizer()
		{{{
			  return norm;
		}}}
		virtual void unset_normalizer()
		{{{
			if(norm)
				delete norm;
			norm = NULL;
		}}}

		virtual memory_statistics get_memory_statistics() = 0;

		virtual timing_statistics get_timing_statistics()
		{{{
			return current_stats;
		}}}
		virtual void enable_timing()
		{{{
			do_timing = true;
		}}}
		virtual void disable_timing()
		{{{
			do_timing = false;
		}}}
		virtual void reset_timing()
		{{{
			current_stats.reset();
		}}}

		// knowledgebase supports undo-operations. this callback should be called after
		// an undo operation to inform the algorithm that some knowledge may be obsolete
		// now. the algorithm should check all internal knowledge, remove obsolete and
		// possibly go back in its state (delete rows/columns).
		// if you are implementing a new algorithm, please use
		// knowledgebase->get_timestamp() for book-keeping and to check which changes to revert.
		// you may choose not to support undo operations. then supports_sync must return false.
		virtual bool sync_to_knowledgebase() = 0;

		// supports_sync() must return true, if undo/sync is supported. false otherwise.
		virtual bool supports_sync() = 0;

		/*
		 * format for serialization:
		 * all values in NETWORK BYTE ORDER!
		 * <serialized learning algorithm data>
		 *	length of string (excluding this length field; not in bytes but in int32_t)
		 *	type of learning algorithm (see enum learning_algorithm::algorithm)
		 *	algorithm-specific data
		 * </serialized learning algorithm data>
		 */
		virtual basic_string<int32_t> serialize() = 0;
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		// for algorithm-specific commands (e.g. parameter passing via dispatcher)
		virtual bool deserialize_magic(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{ return false; };

		virtual void print(ostream &os) = 0;
		virtual string tostring() = 0;

		// check if a hypothesis can be constructed without any further queries
		virtual bool conjecture_ready() = 0;

		// complete internal data structures and then derive automaton:
		//
		// this will resolve required knowledge from knowledgebase. if the knowledge
		// was unknown it will be marked as required and false will be returned.
		// knowledgebase needs to be updated by you, then.
		//
		// if all knowledge was found and an automaton is ready, it will be stored into
		// automaton and true will be returned.
		virtual bool advance(bool & t_is_dfa, int & t_alphabet_size, int & t_state_count, set<int> & t_initial, set<int> & t_final, multimap<pair<int, int>, int> & t_transitions)
		{{{
			bool ret = false;

			start_timing();

			if(complete()) {
				t_alphabet_size = 0;
				t_state_count = 0;
				t_initial.clear();
				t_final.clear();
				t_transitions.clear();

				if(!derive_automaton(t_is_dfa, t_alphabet_size, t_state_count, t_initial, t_final, t_transitions)) {
					(*my_logger)(LOGGER_ERROR, "learning_algorithm::advance(): derive from completed data structure failed! possibly internal error.\n");
				} else {
					ret = true;
				}
			}

			stop_timing();

			return ret;
		}}};

		// in case the hypothesis is wrong, use this function to give a counter-example
		virtual void add_counterexample(list<int>) = 0;

	protected:
		// complete table in such a way that an automaton can be derived
		// return true if table is complete.
		// return false if table could not be completed due to missing knowledge
		virtual bool complete() = 0;
		// derive an automaton from data structure
		virtual bool derive_automaton(bool & is_dfa, int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions) = 0;

// FIXME: is _LINUX defined on linux?
#ifdef _LINUX
# define USAGE_SPECIFIER RUSAGE_THREAD
#else
# define USAGE_SPECIFIER RUSAGE_SELF
#endif
		virtual void start_timing()
		{{{
			if(do_timing && !in_timing) {
#ifdef _WIN32
				if(0 != gettimeofday(&start_utime, NULL))
					return;

#else
				struct rusage ru;

				if(0 != getrusage(USAGE_SPECIFIER, &ru))
					return;

				start_utime = ru.ru_utime;
				start_stime = ru.ru_stime;
#endif
				in_timing = true;
			}
		}}}
		virtual void stop_timing()
		{{{
			if(do_timing && in_timing) {
				in_timing = false;
#ifdef _WIN32
				struct timeval tmp1, tmp2;
				if(0 != gettimeofday(&tmp1, NULL))
					return;
				timersub(&tmp1, &start_utime, &tmp2);
				current_stats.user_sec += tmp2.tv_sec;
				current_stats.user_usec += tmp2.tv_usec;
#else
				struct rusage ru;
				struct timeval tmp;

				if(0 != getrusage(USAGE_SPECIFIER, &ru))
					return;

				timersub(&(ru.ru_utime), &start_utime, &tmp);
				current_stats.user_sec += tmp.tv_sec;
				current_stats.user_usec += tmp.tv_usec;

				timersub(&(ru.ru_stime), &start_stime, &tmp);
				current_stats.sys_sec += tmp.tv_sec;
				current_stats.sys_usec += tmp.tv_usec;
#endif
			}
		}}}
};

}; // end namespace libalf

#endif

