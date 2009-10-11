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
 * (c) 2009 by Daniel Neider, i7 Informatik RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 *
 */

package de.libalf;

import java.io.Serializable;

/**
 * A transition of a {@link BasicAutomaton}. It consists of:
 * <ul>
 * <li>A source state</li>
 * <li>A destination state</li>
 * <li>A label</li>
 * </ul>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 0.1
 * 
 */
public class BasicTransition implements Serializable {
	private static final long serialVersionUID = 1L;

	/**
	 * The source state of the transition.
	 */
	public int source;

	/**
	 * The destination state of the transition.
	 */
	public int destination;

	/**
	 * The label of the transition.
	 */
	public int label;

	/**
	 * Creates a new transition pointing from <code>source</code> to
	 * <code>destination</code> labeled by <code>label</a>.
	 * 
	 * @param source
	 *            the source state of this transition
	 * @param label
	 *            the label of this transition
	 * @param destination
	 *            the destination state of this transition
	 */
	public BasicTransition(int source, int label, int destination) {
		this.source = source;
		this.label = label;
		this.destination = destination;
	}

	@Override
	public boolean equals(Object obj) {
		if (obj == this)
			return true;
		else if (!(obj instanceof BasicTransition))
			return false;
		else {
			BasicTransition otherTransition = (BasicTransition) obj;
			return (source == otherTransition.source
					&& destination == otherTransition.destination && label == otherTransition.label);
		}
	}

	@Override
	public String toString() {
		return "(" + source + ", " + label + ", " + destination + ")";
	}
}
