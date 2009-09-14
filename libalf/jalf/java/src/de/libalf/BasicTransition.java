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
 * @version 1.0
 * 
 */
public class BasicTransition implements Serializable {

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
