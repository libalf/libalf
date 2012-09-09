/*
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
 * (c) 2012 Daniel Neider Lehrstuhl, Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 *
 * Author: Daniel Neider (neider@automata.rwth-aachen.de),  Florian Richter
 *
 */

/**
 * @file
 * This file implements a variant of Angluin's learning algorithm that works
 * with unexperienced teacher. Such a teacher answers membership queries with
 * "yes", "no", and "don't care". On an equivalence query, the teacher checks
 * whether a given deterministic finite automaton (DFA) works correct on all
 * words classified as "yes" and "no"; the behavior on "don't cares" is ignored.
 *
 * The result of this algorithm is a minimal DFA that passes the equivalence
 * check. Internally, the algorithm uses an automaton inferring algorithm, which
 * in turn utilizes a SAT or SMT solver. Thus, be prepared of longer runtimes.
 *
 * The algorithm is comprehensively described in
 *
 * - O. Grinchtein, M. Leucker, and N. Piterman. Inferring network invariants
 *   automatically. In IJCAR 2006, LNAI 4130, pp. 483-497. Springer, 2006.
 *
 * - M. Leucker and D. Neider. Learning Minimal Deterministic Finite Automata
 *   from Inexperienced Teachers. In ISoLA 2012, LNCS 7609, pp. 524-538.
 *   Springer, 2012.
 */

#ifndef __libalf_algorithm_angluin_inexperienced_teacher_h__
#define __libalf_algorithm_angluin_inexperienced_teacher_h__

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <ostream>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>
#include <libalf/algorithm_angluin.h>
#include "libalf/algorithm_automata_inferring.h"
#include "libalf/algorithm_deterministic_inferring_minisat.h"

namespace libalf {

/**
 * The following class implements a <em>weak bool</em>. A week bool can be
 * either <em>TRUE</em>, <em>FALSE</em>, or <em>UNKNOWN</em>. It is used to
 * implement the weak version of Angluin's algorithm to work with inexperienced
 * teachers.
 * 
 * A weak bool behaves as follows when the == operator is applied:
 * (FALSE == FALSE) = TRUE
 * (FALSE == TRUE) = FALSE
 * (TRUE == TRUE) = TRUE
 * (FALSE == UNKNOWN) = TRUE
 * (TRUE == UNKNOWN) = TRUE
 * (UNKNOWN == UNKNOWN) = TRUE
 *
 * As the == operator cannot be used to obtain the exact value, i.e.,
 * <em>TRUE</em>, <em>FALSE</em>, or <em>UNKNOWN</em>, this class provides
 * the methods is_true(), is_false(), and is_unknown() for this purpose.
 *
 * Use the static methods create_true(), create_false(), and creat_unknown() to
 * obtain the desired instances of a weak_bool.
 *
 * @author Florian Richter
 * @version 1.0
 */
class weak_bool {
	public:
		
		/**
		 * Enum used to define the value of a weak bool.
		 */
		enum e_weak_bool {
			WBOOL_FALSE = 0,
			WBOOL_UNKNOWN = 1,
			WBOOL_TRUE = 2
		};

		/**
		 * The value of this weak bool.
		 */
		enum e_weak_bool value;

		/**
		 * Constructs a new weak bool with <em>UNKNOWN</em> value.
		 */
		inline weak_bool()
		{ value = WBOOL_UNKNOWN; };

		/**
		 * Constructs a new weak whose value is the given bool.
		 *
		 * @param val A boolean value to initialize the weak bool with
		 */
		inline weak_bool(bool val)
		{ value = (val ? WBOOL_TRUE : WBOOL_FALSE); };

		/**
		 * Copy constructor.
		 *
		 * @param w The weak bool to copy
		 */
		inline weak_bool(const weak_bool & w)
		{ value = w.value; };

		/**
		 * Constructs a new weak bool given an e_weal_bool enum.
		 *
		 * @param w The e_weak_bool
		 */
		inline weak_bool(const enum e_weak_bool w)
		{ value = w; };

		/*
		 * The following methods can be used to conveniently create new
		 * weak_bool objects.
	 	 */

		/**
		 * Create a weak_bool with value <em>TRUE</em>.
		 *
		 * @return Returns a weak_bool with value <em>TRUE</em>.
		 */
		static weak_bool create_true() {
			return weak_bool(WBOOL_TRUE);
		}

		/**
		 * Create a weak_bool with value <em>FALSE</em>.
		 *
		 * @return Returns a weak_bool with value <em>FALSE</em>.
		 */
		static weak_bool create_false() {
			return weak_bool(WBOOL_FALSE);
		}

		/**
		 * Create a weak_bool with value <em>UNKNOWN</em>.
		 *
		 * @return Returns a weak_bool with value <em>UNKNOWN</em>.
		 */
		static weak_bool create_unknown() {
			return weak_bool(WBOOL_UNKNOWN);
		}

		/*
		inline bool __attribute__((const)) valid() const
		{
			  return (value == WBOOL_FALSE || value == WBOOL_UNKNOWN || value == WBOOL_TRUE);
		}
		*/

		inline void operator=(const weak_bool & other) {
			value = other.value;
		}

		inline bool __attribute__((const)) operator>(const weak_bool & other) const {
			return( ((int)this->value) > ((int)other.value));
		}

		inline bool __attribute__((const)) operator==(const weak_bool & other) const {
			
			if(value == WBOOL_UNKNOWN || other.value == WBOOL_UNKNOWN) {
				return true;
			} else {
				return value == other.value;
			}
			
		}

		inline bool __attribute__((const)) operator!=(const weak_bool & other) const {
			return !(*this == other);
		}
		
		inline bool __attribute__((const)) operator==(bool other) const {
			  if(other)
				  return (value == WBOOL_TRUE);
			  else
				  return (value == WBOOL_FALSE);
		}
		
		inline bool __attribute__((const)) operator>(bool other) const {
			  if(other)
				  return false;
			  else
				  return (value > WBOOL_FALSE);
		}

		inline void operator=(bool other) {
			if(other)
				value = WBOOL_TRUE;
			else
				value = WBOOL_FALSE;
		}

		inline operator int32_t() const {
			return (int32_t)value;
		}

		inline void operator=(int32_t other) {
			value = (enum e_weak_bool)other;
		}
		
		/*
		 * The following three functions are used to determine the value
		 * of a weak_bool. Sometimes you want to differ between UNKNOWN
		 * and another value. 
		 */

		/**
		 * Returns whether this weak_bool represents the value
		 * <em>TRUE</em>.
		 *
		 * @return Returns whether this weak_bool represents the value
		 *         <em>TRUE</em>.
		 */
		inline const bool is_true() const {
			return value == WBOOL_TRUE;
		}

		/**
		 * Returns whether this weak_bool represents the value
		 * <em>FALSE</em>.
		 *
		 * @return Returns whether this weak_bool represents the value
		 *         <em>FALSE</em>.
		 */		
		inline const bool is_false() const {
			return value == WBOOL_FALSE;
		}

		/**
		 * Returns whether this weak_bool represents the value
		 * <em>UNKNOWN</em>.
		 *
		 * @return Returns whether this weak_bool represents the value
		 *         <em>UNKNOWN</em>.
		 */		
		inline const bool is_unknown() const {
			return value == WBOOL_UNKNOWN;
		}
};

/**
 * Serializes a weak_bool according to libALF's serialization scheme.
 *
 * @param w The weak bool to serialize
 *
 * @return Returns the serialized weak_bool.
 */
inline std::basic_string<int32_t> serialize(weak_bool w) {
	std::basic_string<int32_t> ret;
	ret += htonl((int32_t)w);
	return ret;
}

/**
 * Deserializes a weak_bool according to libALF's serialization scheme.
 *
 * @param w The weak bool to deserialize to
 * @param serial The serialization of a weak_bool
 *
 * @return Returns true if the deserialization was successful.
 */
inline bool deserialize(weak_bool & w, serial_stretch & serial) {
	int i;
	if(!::deserialize(i, serial)) return false;
	w = ( (int32_t)i );
	return true;
}

inline std::ostream & operator<<(std::ostream& os, const weak_bool & a) {

	switch(a.value) {
		case weak_bool::WBOOL_FALSE:
			os << "-";
			break;
		case weak_bool::WBOOL_UNKNOWN:
			os << "?";
			break;
		case weak_bool::WBOOL_TRUE:
			os << "+";
			break;
	}

	return os;

}

/**
 * This class implements an Angluin-based version of a learning algorithm that
 * works with inexperienced teachers. The result is a (non necessarily unique)
 * minimal DFA (with respect to the number of states) that passes the teacher's
 * equivalence check.
 *
 * This algorithm changes Angluin's original algorithm in two aspects: 
 * - First, the algorithm works with weak_bool values encoding the values "yes",
 *   "no", and "don't care". By using weak_bools, the closedness and consistency
 *   checks become a check for weak-closedness and weak-consistency (see the
 *   cited papers above).
 *
 * - Second, a (passive) DFA inferring algorithms is utilized to derive a
 *   conjecture. 
 *
 * Thanks to the implementation of weak_bool, this algorithm does only need to
 * replace the derive_conjecture() method to work with inexperienced teacher.
 * Using weak_bools, the original is_closed() and is_consistent() methods are
 * directly turned into the the weak versions.
 *
 * @author Daniel Neider (neider@automata.rwth-aachen.de)
 * @version 1.0
 */
template <class automata_inferring>
class angluin_inexperienced_teacher : public angluin_simple_table<weak_bool> {

	public:
	
	angluin_inexperienced_teacher() : angluin_simple_table<weak_bool>() {}

	angluin_inexperienced_teacher(knowledgebase<weak_bool> * base, logger * log, int alphabet_size) : angluin_simple_table<weak_bool>(base, log, alphabet_size) {}

	/**
	 * Derives a smallest deterministic finite automaton from a (not
	 * necessarily weakliy closed and weakly consistent) observation table
	 * using a DFA inferring algorithm.
	 *
	 * The idea is to use the non-unknown entries in the table as a finite
	 * sample and run a passive DFA inferring algorithm to obtain a smallest
	 * DFA (with respect to the number of states) that is consistent with
	 * this sample and, hence, with the data in the table.
	 *
	 * @return The derived conjecture.
	 */
	virtual conjecture * derive_conjecture() {
		
		knowledgebase<bool> base;
		
		/*
		 * Copy knowledge from Angluin table to internal knowledgebase
		 */
		typename std::list<algorithm_angluin::simple_row<weak_bool, std::vector<weak_bool> > >::const_iterator ti;
		typename std::vector<std::list<int> >::const_iterator ci;
		typename std::vector<weak_bool>::const_iterator acci;

		// Copy upper table
		for(ti = this->upper_table.begin(); ti != this->upper_table.end(); ti++) {
		
			for(acci = ti->acceptance.begin(), ci = column_names.begin(); acci != ti->acceptance.end() && ci != column_names.end(); acci++, ci++) {
				
				assert(!(acci == ti->acceptance.end() || ci == column_names.end()));			

				if(!acci->is_unknown()) {
				
					std::list<int> word = ti->index;
					word.insert(word.end(), ci->begin(), ci->end());

					if(acci->is_true()) {
						base.add_knowledge(word, true);
					} else {
						base.add_knowledge(word, false);
					}
				
				}
				
			}
		
		}
		// Copy lower table
		for(ti = this->lower_table.begin(); ti != this->lower_table.end(); ti++) {
		
			for(acci = ti->acceptance.begin(), ci = column_names.begin(); acci != ti->acceptance.end() && ci != column_names.end(); acci++, ci++) {
				
				assert(!(acci == ti->acceptance.end() || ci == column_names.end()));			

				if(!acci->is_unknown()) {
				
					std::list<int> word = ti->index;
					word.insert(word.end(), ci->begin(), ci->end());

					if(acci->is_true()) {
						base.add_knowledge(word, true);
					} else {
						base.add_knowledge(word, false);
					}
				
				}
				
			}
		
		}
		
		/*
		 * Invoke inferring algorithm
		 */
		automata_inferring inferring_algorithm(&base, this->my_logger, this->alphabet_size);
		conjecture * result = inferring_algorithm.derive_conjecture();
		
		return result;
		
	}

};

}; // end of namespace libalf

#endif

