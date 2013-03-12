/*
 * This file is part of libAMoRE++
 *
 * libAMoRE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libAMoRE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libAMoRE++.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008-2011 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

#ifndef __amore_monoid_h__
# define __amore_monoid_h__

#include <list>
#include <map>

#include <amore/mon.h>

namespace amore {

class amore_monoid {

	private:
		monoid monoid_p;

	public:
		amore_monoid();
		amore_monoid(monoid m);
		~amore_monoid();

		/**
		 * Returns the number of states of the unterlying DFA.
		 */
		virtual unsigned int get_highest_state() const;

		/**
		 * Returns the alphabet size (of the unterlying DFA).
		 */
		virtual unsigned int get_alphabet_size() const;

		/**
		 * Returns the number of elements of this monoid.
		 */
		virtual unsigned int get_element_count() const;

		/**
		 * Returns the number of generators of this monoid.
		 */
		virtual unsigned int get_generator_count() const;

		/**
		 * Returns the zero element
		 */
		virtual unsigned int get_zero() const;

		/**
		 * Checks whether the syntactic monoid equals the syntactic semi-group.
		 */
		virtual bool mequals() const;

		/**
		 * Checks whether the D classes (Green's relation) are computed.
		 */
		virtual bool dclass_iscomputed() const;

		/**
		 * Computes the D classes (Green's relation).
		 */
		virtual void compute_dclass();

		/**
		 * Checks whether the defining relation is computed.
		 */
		virtual bool relation_iscomputed() const;

		/**
		 * Computes the defining relation.
		 */
		virtual void compute_relation();

		/**
		 * Multiplies the elements <code>a</code> and <code>b</code>.
		 */
		virtual unsigned int multiplicate(unsigned int, unsigned int) const;

		/**
		 * Returns the (smallest?) representative of the given monoid's element as <code>char *</code>.
		 * Thereby, the symbols are translated using the \em itoc array.
		 *
		 * @param no The element
		 * @param with If true, the representative is preceeded by a * if it is an idempotent element.
		 * @param zeroone If true, the representative of the 0 and 1 element not computed by 0 or 1 is returned.
		 */
		virtual char * get_representative(unsigned int no, bool with, bool zeroone) const;

		/**
		 * Returns the (smallest?) representative of the given monoid's element.
		 */
		virtual std::list<int> get_representative(unsigned int) const;

		/**
		 * Returns the multiplication table.
		 */
		virtual std::map<unsigned int, std::map<unsigned int, unsigned int> > get_table() const;

		/**
		 * Check whether the given element is idempotent.
		 */
		virtual bool is_idempotent(unsigned int) const;

		/**
		 * Check whether the language represented by this monoid is piecewise testable.
		 */
		virtual bool is_pwt() const;

		/**
		 * Check whether the language represented by this monoid has dot-depth one (or less).
		 */
		virtual bool is_dd1() const;

		/**
		 * Check whether the language represented by this monoid is starfree.
		 */
		virtual bool is_sf() const;

		/**
		 * Check whether the monoid is groupfree.
		 */
		virtual bool is_groupfree() const;

		/**
		 * Returns the AMoRE monoid structure encapsulated by this class.
		 */
		virtual monoid get_monoid() const;
};

}; // end namespace amore

#endif

