/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libmVCA.
 *
 * libmVCA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libmVCA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libmVCA.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 *           and David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libmvca_pushdown_h__
# define __libmvca_pushdown_h__

#include <list>
#include <string>
#include <vector>

#include <libmVCA/serialize.h>

namespace libmVCA {

enum pushdown_direction {
	DIR_UP = +1,			// add to stack
	DIR_CALL = +1,
	DIR_STAY = 0,			// don't touch stack
	DIR_INTERNAL = 0,
	DIR_DOWN = -1,			// remove stack-top
	DIR_RETURN = -1,
	DIR_INDEFINITE = -100		// out of bounds, etc
};

inline bool deserialize(enum pushdown_direction & into, serial_stretch & serial)
{
	if(serial.empty()) return false;
	into = (enum pushdown_direction)ntohl(*serial);
	serial.current++;
	return true;
}

class pushdown_alphabet {
	private:
		int alphabet_size;
		std::vector<enum pushdown_direction> directions;
	public:
		pushdown_alphabet();	// will initialise alphabet_size to 0.
		pushdown_alphabet(int alphabet_size);
		~pushdown_alphabet();

		// set_alphabet_size will default to 0 for <= 0.
		// it will also remove obsolete direction-data and
		// initialise new directions to "stay".
		void set_alphabet_size(int alphabet_size);

		int get_alphabet_size() const;

		void clear()
		{ this->set_alphabet_size(0); directions.clear(); };

		enum pushdown_direction get_direction(int sigma) const;

		// set direction for an alphabet-member
		// (will return false if direction was "indefinite" or
		//  sigma was out of bound)
		bool set_direction(int sigma, enum pushdown_direction direction);

		// get the countervalue of this prefix. returns -1 for an invalid prefix under this alphabet.
		int prefix_countervalue(std::list<int>::const_iterator word, std::list<int>::const_iterator limit, int initial_countervalue = 0) const;
		// get countervalue of this word
		int countervalue(const std::list<int> & word) const
		{ return this->prefix_countervalue(word.begin(), word.end(), 0); }

		// format for serialization:
		// all values in NETWORK BYTE ORDER!
		// <serialized derivate-data>
		//	string length (not in bytes but in int32_t; excluding this length field)
		//	alphabet_size
		//	pushdown-directions[] (alphabet-size times)
		// </serialized automaton>
		std::basic_string<int32_t> serialize() const;
		bool deserialize(::serial_stretch serial);

		std::string to_string() const;

		bool operator==(const pushdown_alphabet & other) const
		{ return ( (this->alphabet_size == other.alphabet_size) && (this->directions == other.directions) ); };
		bool operator!=(const pushdown_alphabet & other) const
		{ return ! (*this == other); };
};

}; // end of namespace libmVCA.

#endif // __libmvca_pushdown_h__

