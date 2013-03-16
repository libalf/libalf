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
 * (c) 2008,2009,2010,2011 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *                     and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
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
#include <sys/types.h>

#include <list>
#include <string>
#include <ostream>
#include <sstream>

#include <libalf/serialize.h>
#include <libalf/logger.h>
#include <libalf/statistics.h>
#include <libalf/knowledgebase.h>
#include <libalf/normalizer.h>
#include <libalf/conjecture.h>

namespace libalf {

enum learning_algorithm_type {
	// if you add anything here, you should also add the
	// name in learning_algorithm.cpp::learning_algorithm_typename_list.
	ALG_NONE = 0,
	// BEGIN

	ALG_ANGLUIN = 1,
	ALG_ANGLUIN_COLUMN = 2,
	ALG_RIVEST_SCHAPIRE = 3,	// Rivest and Schapire: reduced angluin
	ALG_NL_STAR = 4,
	ALG_MVCA_ANGLUINLIKE = 5,	// angluin-style learning of m-bounded visible 1counter automata
	ALG_BIERMANN = 6,               // (obsolete)
	ALG_RPNI = 7,
	ALG_DELETE2 = 8,
	ALG_BIERMANN_ORIGINAL = 9,
	ALG_KEARNS_VAZIRANI = 10,
	ALG_CEGAR = 11,
	ALG_CEGAR_ANGLUIN = 12,
	ALG_INFERRING_MINISAT = 13,
	ALG_INFERRING_Z3 = 14,
	ALG_INFERRING_CSP_MINISAT = 15,
	ALG_INFERRING_CSP_Z3 = 16,


	// END
	ALG_LAST_INVALID = 15

//	ALG_BIERMANN_ANGLUIN = ...,	//
};

// resolve type to its name
const char * learning_algorithm_name(enum learning_algorithm_type type);


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

		// get type of algorithm
		virtual enum learning_algorithm_type get_type() const = 0;

		// get the least complex type of algorithm (in terms of derivation)
		// that is compatible with this one (can use the same table / serialized data)
		virtual enum learning_algorithm_type get_basic_compatible_type() const = 0;

		// give a string representation of the algorithm type
		virtual const char * get_name()
		{ return learning_algorithm_name(this->get_type()); }

		// set_alphabet_size() is only for initial setting.
		// once any data is in the structure, use increase_alphabet_size ONLY.
// FIXME: increase + decrease alphabet-size, check initialized.
		virtual void set_alphabet_size(int alphabet_size)
		{{{
			this->alphabet_size = alphabet_size;
		}}}
		virtual int get_alphabet_size() const
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
		virtual logger * get_logger()
		{{{
			if(my_logger == (&this->ignore))
				return NULL;
			else
				return my_logger;
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
			this->norm = norm;
		}}}
		virtual normalizer * get_normalizer()
		{{{
			  return norm;
		}}}
		virtual void unset_normalizer()
		{{{
			norm = NULL;
		}}}

		virtual void receive_generic_statistics(generic_statistics & stat) const = 0;

		virtual memory_statistics get_memory_statistics() const __attribute__((deprecated)) = 0;

		virtual timing_statistics get_timing_statistics() const __attribute__((deprecated))
		{{{
			return current_stats;
		}}} __attribute__((deprecated));

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
		virtual bool supports_sync() const = 0;

		/*
		 * format for serialization:
		 * all values in NETWORK BYTE ORDER!
		 * <serialized learning algorithm data>
		 *	length of string (excluding this length field; not in bytes but in int32_t)
		 *	type of learning algorithm (see enum learning_algorithm::algorithm)
		 *	algorithm-specific data
		 * </serialized learning algorithm data>
		 */
		virtual std::basic_string<int32_t> serialize() const = 0;
		virtual bool deserialize(serial_stretch & serial) = 0;

		// for algorithm-specific commands (e.g. parameter passing via dispatcher)
		virtual bool deserialize_magic(serial_stretch & serial, std::basic_string<int32_t> & result)
		{{{
			(*my_logger)(LOGGER_WARN, "learning_algorithm::deserialize_magic(): called but not implemented by algorithm. Ignoring content.\n");
			serial.empty(); /* <- this is just so we don't get a not-used warning */
			result.clear();
			return false;
		}}};

		virtual void print(std::ostream &os) const = 0;
		virtual std::string to_string() const
		{{{
			std::stringstream str;
			this->print(str);
			return str.str();
		}}}

		// check if a hypothesis can be constructed without any further queries
		virtual bool conjecture_ready() = 0;

		// complete internal data structures and then derive automaton:
		//
		// this will resolve required knowledge from knowledgebase. if the knowledge
		// was unknown it will be marked as required and false will be returned.
		// knowledgebase needs to be updated by you, then.
		//
		// if all knowledge was found and a conjecture is ready, it will be returned.
		// otherwise, NULL will be returned.
		virtual conjecture * advance()
		{{{
			conjecture * ret = NULL;

			if(my_knowledge == NULL) {
				(*my_logger)(LOGGER_ERROR, "learning_algorithm::advance(): no knowledgebase was set!\n");
				return NULL;
			}

			start_timing();

			if(complete()) {
				ret = derive_conjecture();
				if(!ret)
					(*my_logger)(LOGGER_ERROR, "learning_algorithm::advance(): derive from completed data structure failed! possibly internal error.\n");
			}

			stop_timing();

			return ret;
		}}};

		// in case the hypothesis is wrong, use this function to give a counter-example
		virtual bool add_counterexample(std::list<int>) = 0;

	protected:
		// complete table in such a way that an automaton can be derived
		// return true if table is complete.
		// return false if table could not be completed due to missing knowledge
		virtual bool complete() = 0;
		// derive an automaton from data structure
		virtual conjecture * derive_conjecture() = 0;

// FIXME: is _LINUX defined on linux?
#ifdef _LINUX
#error OK!
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

/**
 * Defines the << operator for learning algorithms, i.e., writes a string
 * representation of the learning algorithm to the given output stream. Calls
 * the print(std::ostream &os) method internally.
 *
 * @param out The output stream to write the string representation to
 * @param alg The algorithm to print
 *
 * @return Returns the given output stream as usual.
 */
template <class answer>
std::ostream & operator<<(std::ostream & out, const libalf::learning_algorithm<answer> & alg) {
	alg.print(out);
	return out;
}

#endif

