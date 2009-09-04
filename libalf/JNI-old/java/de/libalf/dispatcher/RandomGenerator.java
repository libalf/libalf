package de.libalf.dispatcher;

/**
 * <p>
 * Random generator for deterministic finite automata (DFA). The random
 * generator can be parameterized by the <em>number of states</em> and the
 * <em>size of the alphabet</em> used. See
 * <ul>
 * <li><em>TODO: Some paper</em></li>
 * </ul>
 * </p>
 * <p>
 * <b>Note:</b>
 * <ul>
 * <li>This is a Java implementation of the <em>dfa_random_generator</em> C++
 * class. All method calls are forwarded to the C++ library via the JNI
 * interface.</li>
 * <li>
 * This JavaDoc is only a rough overview. For a detailed documentation please
 * refer to the original C++ documentation.</li>
 * </ul>
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 * 
 */
public class RandomGenerator {

	static {
		System.loadLibrary("alf_jni");
	}

	/**
	 * Computes a random deterministic finite automaton with the given alphabet
	 * and number of states. Both parameter have to be greater than 1. If
	 * invalid parameters are given, the method returns <code>null</code>.
	 * 
	 * @param numberOfStates
	 *            an arbitrary positive number of states
	 * @param alphabetSize
	 *            an arbitrary positive alphabet size
	 * @return a randomly generated DFA
	 */
	public static BasicAutomaton createRandomDFA(int numberOfStates,
			int alphabetSize) {
		if (numberOfStates < 1 || alphabetSize < 1)
			return null;
		else
			return createRandomDFA_private(numberOfStates, alphabetSize);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link RandomGenerator#createRandomDFA(int, int)}.
	 * </p>
	 * 
	 * @param numberOfStates
	 *            an arbitrary positive number of states
	 * @param alphabetSize
	 *            an arbitrary positive alphabet size
	 * @return the result of the JNI call.
	 */
	private static native BasicAutomaton createRandomDFA_private(
			int numberOfStates, int alphabetSize);

	/**
	 * Discards the computed table and frees the memory.
	 */
	public static native void discardTables();

	/**
	 * Test the stuff ...
	 * 
	 * @param args
	 *            of no use
	 */
	public static void main(String[] args) {
		BasicAutomaton automaton = RandomGenerator.createRandomDFA(7, 3);
		System.out.println(automaton.toDot());
	}

}
