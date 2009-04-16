package de.libalf.jni;
import java.util.HashSet;
import java.util.LinkedList;

/**
 * <p>
 * This class represents a deterministic or nondeterministic finite automaton as
 * it is generated by the LibALF library.
 * </p>
 * 
 * <p>
 * A LibALF automaton consists of:
 * <ul>
 * <li>A set of <em>states</em>, implicitly represented by the number of states.
 * </li>
 * </ul>
 * </p>
 * 
 * <p>
 * Note that this class does only store the automaton, but provides no
 * functionality. To work with this automaton, one has to implement the required
 * operations (e.g. simulating runs on words, Boolean operations etc.) or needs
 * to convert it into an existing automaton library (like the <a
 * href="http://www.brics.dk/automaton/">brics library</a>).
 * </p>
 * 
 * @author Daniel Neider (Chair of Computer Science 7, RWTH Aachen University)
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

	private HashSet<Integer> initialStates;

	private HashSet<Integer> finalStates;

	private LinkedList<BasicTransition> transitions;

	public BasicAutomaton(int numberOfStates, int alphabetSize) {
		this(false, numberOfStates, alphabetSize);
	}

	public BasicAutomaton(boolean isDFA, int numberOfStates, int alphabetSize) {
		this.isDFA = isDFA;
		this.numberOfStates = numberOfStates;
		this.alphabetSize = alphabetSize;
		initialStates = new HashSet<Integer>(numberOfStates);
		finalStates = new HashSet<Integer>(numberOfStates);
		transitions = new LinkedList<BasicTransition>();
	}

	public boolean isDFA() {
		return isDFA;
	}

	public int getNumberOfStates() {
		return numberOfStates;
	}

	public int getAlphabetSize() {
		return alphabetSize;
	}

	public void setAlphabetSize(int alphabetSize) {
		this.alphabetSize = alphabetSize;
	}

	public void setDFA(boolean isDfa) {
		isDFA = isDfa;
	}

	public void setNumberOfStates(int numberOfStates) {
		this.numberOfStates = numberOfStates;
	}

	public HashSet<Integer> getInitialStates() {
		return initialStates;
	}

	public void addInitialState(int stateToAdd) {
		initialStates.add(stateToAdd);
	}

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
	 * Adds the given state to the set of final states.
	 * 
	 * @param stateToAdd
	 *            the state to add
	 */
	public void addFinalState(int stateToAdd) {
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
	 * Adds a new transition to the automaton (if not already present).
	 * 
	 * @param transitionToAdd
	 *            the transition to add
	 */
	public void addTransition(BasicTransition transitionToAdd) {
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

	@Override
	public String toString() {
		String output = "LibALF automaton Java impementation\nNumber of States: "
				+ numberOfStates + "\n";
		output += "Initial states: " + initialStates + "\n";
		output += "Final states: " + finalStates + "\n";
		output += "Transitions:" + transitions;
		return output;
	}
}
