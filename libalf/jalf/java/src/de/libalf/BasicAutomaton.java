package de.libalf;

import java.util.HashSet;
import java.util.LinkedList;

/**
 * <p>
 * Represents a deterministic or nondeterministic finite automaton as it is
 * generated by the LibALF library.
 * </p>
 * <p>
 * A LibALF automaton consists of:
 * <ul>
 * <li>A set of <em>states</em>, represented by the <code>integers</code>
 * between <code>0</code> and <code>numberOfStates</code>.</li>
 * <li>An <em>alphabet</em> over which the automaton works. An alphabet is the
 * set of all <code>integers</code> between <code>0</code> and
 * <code>alphabetSize</code> .</li>
 * <li>A set of <em>initial states</em>.</li>
 * <li>A set of <em>final states</em>.</li>
 * </ul>
 * Make sure that both <code>numberOfStates</code> and <code>alphabetSize</code>
 * are <code>integers</code> greater than 0.
 * </p>
 * 
 * <p>
 * Note that this class does only store the automaton, but provides no
 * functionality. <code>BasicAutomaton</code> objects are created by the LibALF
 * C++ library via the JNI interface. To work with this automaton, one has to
 * implement the required operations (e.g. simulating runs on words, Boolean
 * operations etc.) or needs to convert it into an existing automaton library
 * (e.g. the <a href="http://www.brics.dk/automaton/">brics library</a>).
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 * 
 */
public class BasicAutomaton {

	/**
	 * Is this object representing a deterministic finite automaton?
	 */
	private boolean isDFA;

	/**
	 * The number of states of the automaton.
	 */
	private int numberOfStates;

	/**
	 * The size of the alphabet used by this automaton.
	 */
	private int alphabetSize;

	/**
	 * The set of initial states.
	 */
	private HashSet<Integer> initialStates;

	/**
	 * The set of final states.
	 */
	private HashSet<Integer> finalStates;

	/**
	 * The transitions of the automaton.
	 */
	private LinkedList<BasicTransition> transitions;

	/**
	 * Creates a new nondeterministic finite automaton with the given number of
	 * states and alphabet.
	 * 
	 * @param numberOfStates
	 *            the number of states
	 * @param alphabetSize
	 *            the size of the alphabet
	 */
	public BasicAutomaton(int numberOfStates, int alphabetSize) {
		this(false, numberOfStates, alphabetSize);
	}

	/**
	 * Creates a new finite automaton with the given number of states and
	 * alphabet. The automaton is deterministic or nondeterministic depending on
	 * the <code>isDFA</code> parameter.
	 * 
	 * @param isDFA
	 *            determines if the automaton is deterministic or
	 *            nondeterministic
	 * @param numberOfStates
	 *            the number of states
	 * @param alphabetSize
	 *            the size of the alphabet
	 */
	public BasicAutomaton(boolean isDFA, int numberOfStates, int alphabetSize) {
		this.isDFA = isDFA;
		this.numberOfStates = numberOfStates;
		this.alphabetSize = alphabetSize;
		initialStates = new HashSet<Integer>(numberOfStates);
		finalStates = new HashSet<Integer>(numberOfStates);
		transitions = new LinkedList<BasicTransition>();
	}

	/**
	 * Returns whether the automaton is deterministic or nondeterministic.
	 * 
	 * @return whether the automaton is deterministic or nondeterministic.
	 */
	public boolean isDFA() {
		return isDFA;
	}

	/**
	 * Returns the number of states of the automaton.
	 * 
	 * @return the number of states of the automaton.
	 */
	public int getNumberOfStates() {
		return numberOfStates;
	}

	/**
	 * Returns the size of the alphabet (i.e. the greatest symbol in the
	 * alphabet).
	 * 
	 * @return the size of the alphabet.
	 */
	public int getAlphabetSize() {
		return alphabetSize;
	}

	/**
	 * Returns the set of initial states.
	 * 
	 * @return the set of initial states.
	 */
	public HashSet<Integer> getInitialStates() {
		return initialStates;
	}

	/**
	 * Adds the given state to the set of initial states. This method does only
	 * allow to add states that are states of the automaton.
	 * 
	 * @param stateToAdd
	 *            the state to add to the set of initial states
	 */
	public void addInitialState(int stateToAdd) {
		if (stateToAdd >= 0 && stateToAdd <= numberOfStates)
			initialStates.add(stateToAdd);
	}

	/**
	 * Removes the given state from the set of initial states.
	 * 
	 * @param stateToRemove
	 *            the state to remove from the set of initial states
	 */
	public void removeInitialState(int stateToRemove) {
		initialStates.remove(stateToRemove);
	}

	/**
	 * Returns the automaton's set of final states.
	 * 
	 * @return the automaton's set of final states.
	 */
	public HashSet<Integer> getFinalStates() {
		return finalStates;
	}

	/**
	 * Adds the given state to the set of final states. This method does only
	 * allow to add states that are states of the automaton.
	 * 
	 * @param stateToAdd
	 *            the state to add
	 */
	public void addFinalState(int stateToAdd) {
		if (stateToAdd >= 0 && stateToAdd <= numberOfStates)
			finalStates.add(stateToAdd);
	}

	/**
	 * Checks whether the given state is a final state.
	 * 
	 * @param state
	 *            the state to check
	 * @return true if the state is a final state and false otherwise.
	 */
	public boolean isFinalState(int state) {
		return finalStates.contains(state);
	}

	/**
	 * Removes a state from the set of final states.
	 * 
	 * @param stateToRemove
	 *            the state to remove
	 */
	public void removeFinalState(int stateToRemove) {
		finalStates.remove(stateToRemove);
	}

	/**
	 * Returns the transitions of this automaton.
	 * 
	 * @return the transitions of this automaton.
	 */
	public LinkedList<BasicTransition> getTransitions() {
		return transitions;
	}

	/**
	 * Adds a new transition to the automaton. This method does only allow to
	 * add valid transitions, i.e. transitions that are labeled correctly and
	 * where the source and the destination state are states of the automaton.
	 * 
	 * @param transitionToAdd
	 *            the transition to add
	 */
	public void addTransition(BasicTransition transitionToAdd) {
		if ((transitionToAdd.source >= 0 && transitionToAdd.source <= numberOfStates)
				&& (transitionToAdd.destination >= 0 && transitionToAdd.destination <= numberOfStates)
				&& (transitionToAdd.label >= 0 && transitionToAdd.label <= alphabetSize))
			transitions.add(transitionToAdd);
	}

	/**
	 * Removes the given transition from the automaton.
	 * 
	 * @param transitionToRemove
	 *            the transition to remove.
	 */
	public void removeTransition(BasicTransition transitionToRemove) {
		transitions.remove(transitionToRemove);
	}

	public String toDot() {
		String lineSeparator = System.getProperty("line.separator");
		String dot = "digraph Automaton {" + lineSeparator;
		dot += "  rankdir = LR;" + lineSeparator;
		for (int i = 0; i < numberOfStates; i++)
			dot += "  " + i + " [shape="
					+ (finalStates.contains(i) ? "double" : "")
					+ "circle, label=\"" + i + "\"];" + lineSeparator;
		for (BasicTransition t : transitions)
			dot += "  " + t.source + " -> " + t.destination + " [label=\""
					+ t.label + "\"];" + lineSeparator;
		dot += "}";
		return dot;
	}

	@Override
	public String toString() {
		String output = "LibALF automaton Java impementation\nNumber of States: "
				+ numberOfStates + "\n";
		output += "Alphabet size: " + alphabetSize + "\n";
		output += "Automaton is deterministic: " + isDFA + "\n";
		output += "Initial states: " + initialStates + "\n";
		output += "Final states: " + finalStates + "\n";
		output += "Transitions:" + transitions;
		return output;
	}
}