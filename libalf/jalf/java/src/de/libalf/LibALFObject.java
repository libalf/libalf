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
 * (c) 2009 by Daniel Neider, Chair of Computer Science 2 and 7, RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 *
 */

package de.libalf;

import java.io.Serializable;

/**
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 0.1
 */
public interface LibALFObject extends Serializable {
	/**
	 * <p>Kills the object by freeing the memory occupied by the C++ object.</p>
	 * <p>After an object is dead, no more operations on this object can be performed.</p>
	 */
	public boolean isDestroyed();

	/**
	 * <p>Kills the object by freeing the memory occupied by the C++ object.</p>
	 * <p>After an object is dead, no more operations on this object can be performed.</p>
	 */
	public void destroy();
}
