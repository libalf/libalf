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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * (c) 2010 David R. Piegdon
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_answer_h__
# define __libalf_answer_h__

#include <ostream>
#include <sstream>
#include <string>

#include <sys/types.h>
#ifdef _WIN32
# include <stdint.h>
#endif

#include <libalf/serialize.h>

namespace libalf {

#ifndef MIN
# define MIN(x,y) ( (x) <= (y) ? (x) : (y) )
#endif
#ifndef MAX
# define MAX(x,y) ( (x) >= (y) ? (x) : (y) )
#endif

/*
 * in general, a possible <answer> should implement the following:
 *
 * as members:
 *   bool operator==(&answer)
 *   bool operator!=(&answer)
 *   void operator=(&answer)
 *
 * additional functions:
 *   std::basic_string<int32_t> serialize(answer a)
 *   bool deserialize(answer & a, serial_stretch & serial)
 *   ostream & operator<<(ostream& os, const answer & a)
 *
 * all the above exist for the builtin-type bool.
 *
 *
 * Following are two custom (example-) types that can be used as
 * answers:
 *
 * first an extended boolean, supporting true, false and unknown.
 *
 * then a type that supports answers as chars between 'a' and 'a'+k,
 * where k is fixed by you. this type can be used to learn
 * moore_machines with an output alphabet of size k.
 */



// example implementation of an extended bool, supporting also the state <unknown>:
class extended_bool {
	public:
		enum e_extended_bool {
			EBOOL_FALSE = 0,
			EBOOL_UNKNOWN = 1,
			EBOOL_TRUE = 2
		};

		enum e_extended_bool value;

		inline extended_bool()
		{ value = EBOOL_UNKNOWN; };

		inline extended_bool(bool val)
		{ value = (val ? EBOOL_TRUE : EBOOL_FALSE); };

		inline extended_bool(const extended_bool & e)
		{ value = e.value; };

		inline extended_bool(const enum e_extended_bool e)
		{ value = e; };

		inline bool __attribute__((const)) valid() const
		{{{
			  return (value == EBOOL_FALSE || value == EBOOL_UNKNOWN || value == EBOOL_TRUE);
		}}}

		inline bool __attribute__((const)) operator!=(const extended_bool & other) const
		{{{
			return this->value != other.value;
		}}}

		inline void operator=(const extended_bool & other)
		{{{
			value = other.value;
		}}}

		inline bool __attribute__((const)) operator>(const extended_bool & other) const
		{{{
			return( ((int)this->value) > ((int)other.value));
		}}}

		inline bool __attribute__((const)) operator==(bool other) const
		{{{
			  if(other)
				  return (value == EBOOL_TRUE);
			  else
				  return (value == EBOOL_FALSE);
		}}}

		inline bool __attribute__((const)) operator>(bool other) const
		{{{
			  if(other)
				  return false;
			  else
				  return (value > EBOOL_FALSE);
		}}}

		inline void operator=(bool other)
		{{{
			if(other)
				value = EBOOL_TRUE;
			else
				value = EBOOL_FALSE;
		}}}

		inline operator int32_t() const
		{{{
			return (int32_t)value;
		}}}

		inline void operator=(int32_t other)
		{{{
			value = (enum e_extended_bool)other;
		}}}
};

inline bool __attribute__((const)) operator==(extended_bool a, extended_bool b)
{{{
	return a.value == b.value;
}}}

inline std::basic_string<int32_t> serialize(extended_bool e)
{{{
	std::basic_string<int32_t> ret;
	ret += htonl((int32_t)e);
	return ret;
}}}

inline bool deserialize(extended_bool & e, serial_stretch & serial)
{{{
	int i;
	if(!::deserialize(i, serial)) return false;
	e = ( (int32_t)i );
	return true;
}}}

inline std::ostream & operator<<(std::ostream& os, const extended_bool & a)
{{{
	switch(a.value) {
		case extended_bool::EBOOL_FALSE:
			os << "-";
			break;
		case extended_bool::EBOOL_UNKNOWN:
			os << "?";
			break;
		case extended_bool::EBOOL_TRUE:
			os << "+";
			break;
	}
	return os;
}}}



/* implementation of fixed-alphabet-size answer type (e.g. for learning moore_machines<...>),
 * ranging as a char from 'a' to 'a'+size. (thus, size should be no larger than 26, if i
 * counted right :).
 *
 * NOTE:
 *	as the size of the alphabet has to be fixed for the type, not for an instance,
 *	it needs to be passed as a template. currently, this is only possible by defining
 *	a class that provides a function that returns the alphabet size. so, to get a
 *	fixed_count_answer of 'a' .. 'a'+5, define:
 *
 *		class my_size_definition : public fixed_count_answer__size_definition {
 *			virtual int get_size() { return 5; };
 *		};
 *
 *	and use the fixed_count_answer template with this type:
 *
 *		typedef fixed_count_answer<my_size_definition> my_answer_type;
 */
class fixed_count_answer__size_definition {
	public:
		virtual int get_size() = 0;
};

template<typename size_definition>
class fixed_count_answer {
        private:
		size_definition sizedef; // XXX: WTF does static not work (linker error)

                char value;
        public:
                inline fixed_count_answer()
                { value = 0; }

                inline fixed_count_answer(char c)
                { value = c; }

		inline ~fixed_count_answer()
		{ };


                inline static int get_alphabet_size()
                { size_definition sizedef; return sizedef.get_size(); }

                inline void set(char c)
                { value = MAX( 'a' , MIN(c, ('a' + sizedef.get_size() - 1)) ); }

                inline char get() const
                { return value; }

                inline void operator=(fixed_count_answer<size_definition> other)
                { this->value = other.value; };

                inline void operator=(int32_t i)
                { set( (char)i ); };

                inline bool operator==(fixed_count_answer<size_definition> other) const
                { return this->value == other.value; }

                inline bool operator==(char c) const
                { return this->value == c; }

                inline operator int32_t() const
                { return (int32_t)value; }
};

template<typename size_definition>
inline std::basic_string<int32_t> serialize(fixed_count_answer <size_definition> a)
{{{
        std::basic_string<int32_t> ret;
        ret += htonl( (int32_t)a );
        return ret;
}}};

template<typename size_definition>
inline bool deserialize(fixed_count_answer<size_definition> & a, serial_stretch & serial)
{{{
        int i;
        if(!::deserialize(i, serial)) return false;
        a = (int32_t)i;
        return true;
}}};

template<typename size_definition>
inline std::ostream & operator<<(std::ostream& os, fixed_count_answer<size_definition> a)
{{{
        os << a.get();
        return os;
}}};


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


}; // enf of namespace libalf

#endif // __libalf_answer_h__

