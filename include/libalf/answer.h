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

#ifndef __libalf_answer_h__
# define __libalf_answer_h__

namespace libalf {

// a possible <answer> has to implement the following:
//
// bool operator==(&answer)
// bool operator!=(&answer)
// bool operator>(&answer) , where true > possibly true > unknown > possibly false > false
// void operator=(&answer)
//
// bool operator==(bool)
// bool operator!=(bool)
// bool operator>(bool)
// void operator=(bool)
//
// all the above are valid for the builtin-type bool.


// an example implementation:

enum e_extended_bool {
	EBOOL_FALSE = 0,
	EBOOL_UNKNOWN = 1,
	EBOOL_TRUE = 1
};

class extended_bool {
	public:
		enum e_extended_bool value;

		bool operator==(extended_bool &other)
		{{{
			return this->value == other.value;
		}}}

		bool operator!=(extended_bool &other)
		{{{
			return this->value != other.value;
		}}}

		void operator=(extended_bool &other)
		{{{
			value = other.value;
		}}}

		bool operator>(extended_bool &other)
		{{{
			return( ((int)this->value) > ((int)other.value));
		}}}

		bool operator==(bool other)
		{{{
			  if(other)
				  return (value == EBOOL_TRUE);
			  else
				  return (value == EBOOL_FALSE);
		}}}

		bool operator>(bool other)
		{{{
			  if(other)
				  return false;
			  else
				  return (value > EBOOL_FALSE);
		}}}

		void operator=(bool other)
		{{{
			if(other)
				value = EBOOL_TRUE;
			else
				value = EBOOL_FALSE;
		}}}
};

}

#endif // __libalf_answer_h__

