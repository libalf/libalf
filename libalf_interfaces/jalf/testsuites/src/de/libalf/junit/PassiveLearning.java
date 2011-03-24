package de.libalf.junit;

import java.util.HashSet;
import java.util.Set;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import de.libalf.BasicAutomaton;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.LibALFFactory.Algorithm;
import de.libalf.dispatcher.DispatcherFactory;
import de.libalf.jni.JNIFactory;
import dk.brics.automaton.Automaton;
import dk.brics.automaton.BasicAutomata;
import dk.brics.automaton.RegExp;

/**
 * <p>
 * Class to test jalf's offline learning functionality. Both the JNI interface
 * and the dispatcher connectivity are tested simultaneously and the results are
 * compared.
 * </p>
 * 
 * <p>
 * Use the system variables <code>server</code> and <code>port</code> using
 * <code>-Dport=...</code> to specify the dispatcher's ip adress and port.
 * If these system variables are not given, then <em>localhost</em> and
 * <em>24940</em> are used. 
 * </p>
 * 
 * Instructions:
 * <ol>
 * <li>Download the brics automaton library: http://www.brics.dk/automaton</li>
 * <li>Download JUnit 4.x</li>
 * <li>Compile libalf, jalf and the dispatcher.</li>
 * <li>Run the dispatcher (server) and make sure that Java can find all
 * necessary libraries (set java.library.path and the classpath). If necessary
 * also specify the server and port (see above).</li>
 * <li>Run the JUnit test cases.</li>
 * </ol>
 * 
 * @author Daniel Neider
 * @version 1.0
 * 
 */
public class PassiveLearning {

	/**
	 * These algorithms are to be tested.
	 */
	static Algorithm[] availableAlgorithms = { Algorithm.RPNI,
	Algorithm.BIERMANN_ORIGINAL, Algorithm.BIERMANN_MINISAT,
	Algorithm.DELETE2 };

	/**
	 * The alphabet size used for testing
	 */
	public static int alphabetSize = 3;

	/**
	 * The maximal length of samples in S_plus / S_minus
	 */
	public static int maxLength = 5;

	/**
	 * Biermann specific value of nondeterminism
	 */
	public static int biermannValue = 2;

	/**
	 * Method executed before all test are performed.
	 * 
	 * @throws Exception
	 */
	@BeforeClass
	public static void setUp() throws Exception {

		// Get server adress as System property
		String server = System.getProperty("server", "localhost");
		int port = new Integer(System.getProperty("port", "24940"));

		// Connect to libalf
		jniFactory = JNIFactory.STATIC;
		dispatcherFactory = new DispatcherFactory(server, port);
	}

	/**
	 * Method executed after all tests are performed.
	 * 
	 * @throws Exception
	 */
	@AfterClass
	public static void tearDown() throws Exception {

		// Disconnect libalf
		jniFactory.destroy();
		dispatcherFactory.destroy();
	}

	/**
	 * First test case.
	 */
	@Test
	public void testPassiveLearning1() {
		Automaton a = AutomatonProvider.getAutomaton(1);
		System.out.println("=== Automaton 1 ===");
		HashSet<int[]> s_plus = new HashSet<int[]>(), s_minus = new HashSet<int[]>();
		createSampleSets(a, maxLength, s_plus, s_minus);
		learnAll(s_plus, s_minus);
	}

	/**
	 * Second test case.
	 */
	@Test
	public void testPassiveLearning2() {
		Automaton a = AutomatonProvider.getAutomaton(2);
		System.out.println("=== Automaton 2 ===");
		HashSet<int[]> s_plus = new HashSet<int[]>(), s_minus = new HashSet<int[]>();
		createSampleSets(a, maxLength, s_plus, s_minus);
		learnAll(s_plus, s_minus);
	}

	/**
	 * Third test case.
	 */
	@Test
	public void testPassiveLearning3() {
		Automaton a = AutomatonProvider.getAutomaton(3);
		System.out.println("=== Automaton 3 ===");
		HashSet<int[]> s_plus = new HashSet<int[]>(), s_minus = new HashSet<int[]>();
		createSampleSets(a, maxLength, s_plus, s_minus);
		learnAll(s_plus, s_minus);
	}

	/**
	 * Fourth test case.
	 */
	@Test
	public void testPassiveLearning4() {
		Automaton a = AutomatonProvider.getAutomaton(4);
		System.out.println("=== Automaton 4 ===");
		HashSet<int[]> s_plus = new HashSet<int[]>(), s_minus = new HashSet<int[]>();
		createSampleSets(a, maxLength, s_plus, s_minus);
		learnAll(s_plus, s_minus);
	}

	/**
	 * Creates a samples set of accepting and rejecting samples that contain all
	 * words up to length <code>len</code>.
	 * 
	 * @param a
	 *            the automaton to derive the sample set from
	 * @param len
	 *            the maximal length of a sample
	 * @param S_plus
	 *            the set to store the positive samples
	 * @param S_minus
	 *            the set to store the positive samples
	 */
	public static void createSampleSets(Automaton a, int len,
			Set<int[]> S_plus, Set<int[]> S_minus) {

		// Positive words
		for (int i = 0; i < len; i++) {

			Set<String> words = a.getStrings(i);

			for (String s : words) {
				S_plus.add(Tools.str2arr(s));
			}
		}

		// Negative words
		Automaton tmp = (a.complement()).intersection(BasicAutomata
				.makeCharRange((char) 0, (char) (alphabetSize - 1)).repeat());
		for (int i = 0; i < len; i++) {

			Set<String> words = tmp.getStrings(i);

			for (String s : words) {
				S_minus.add(Tools.str2arr(s));
			}
		}

	}

	/*
	 * Libalf objects for shared use
	 */
	static JNIFactory jniFactory;
	static DispatcherFactory dispatcherFactory;

	/**
	 * Runs all available active learning algorithms on the given teacher.
	 * 
	 * @param S_plus
	 *            the set of positive samples
	 * @param S_minus
	 *            the set of negative samples
	 */
	private void learnAll(Set<int[]> S_plus, Set<int[]> S_minus) {
		for (Algorithm type : availableAlgorithms) {
			learnJNIAndDispatcher(type, S_plus, S_minus);
			System.out.println(" done.");
		}
	}

	/**
	 * Runs the learning algorithm given by <code>type</code> using the
	 * <code>teacher</code>. Thereby, both JNI and the dispatcher are used and
	 * their results are compared (which have to be the same).
	 * 
	 * @param type
	 *            the learning algorithm to use
	 * @param S_plus
	 *            the set of positive samples
	 * @param S_minus
	 *            the set of negative samples
	 */
	private void learnJNIAndDispatcher(Algorithm type, Set<int[]> S_plus,
			Set<int[]> S_minus) {

		// Create libalf objects
		Knowledgebase dispatcherBase = dispatcherFactory.createKnowledgebase();
		LearningAlgorithm dispatcherAlgorithm;
		if (type == Algorithm.BIERMANN_ORIGINAL) {
			dispatcherAlgorithm = dispatcherFactory.createLearningAlgorithm(
					type, dispatcherBase, alphabetSize, biermannValue);
		} else {
			dispatcherAlgorithm = dispatcherFactory.createLearningAlgorithm(
					type, dispatcherBase, alphabetSize);
		}

		Knowledgebase jniBase = jniFactory.createKnowledgebase();
		LearningAlgorithm jniAlgorithm;
		if (type == Algorithm.BIERMANN_ORIGINAL) {
			jniAlgorithm = jniFactory.createLearningAlgorithm(type, jniBase,
					alphabetSize, biermannValue);
		} else {
			jniAlgorithm = jniFactory.createLearningAlgorithm(type, jniBase,
					alphabetSize);
		}

		System.out.print("Running " + jniAlgorithm.get_name() + " (JNI) / ");
		System.out.print(dispatcherAlgorithm.get_name() + " (dispatcher) ...");

		// Add knowledge to Knowledgebase
		for (int[] w : S_plus) {
			jniBase.add_knowledge(w, true);
			dispatcherBase.add_knowledge(w, true);
		}
		for (int[] w : S_minus) {
			jniBase.add_knowledge(w, false);
			dispatcherBase.add_knowledge(w, false);
		}

		// Run the learning algorithm
		BasicAutomaton resultJNI = (BasicAutomaton) jniAlgorithm.advance();
		BasicAutomaton resultDispatcher = (BasicAutomaton) dispatcherAlgorithm
				.advance();

		// Make sure that some automaton has been derived
		assert (resultJNI != null);
		assert (resultDispatcher != null);

		// Check whether both automata are equal
		Automaton r1 = Tools.libalf2brics(resultJNI);
		Automaton r2 = Tools.libalf2brics(resultDispatcher);
		assert (r1.equals(r2));

		// Check whether both automata classify the sample set correctly
		for (int[] w : S_plus) {
			assert (r1.run(Tools.arr2str(w)));
			assert (r2.run(Tools.arr2str(w)));
		}
		for (int[] w : S_minus) {
			assert (!r1.run(Tools.arr2str(w)));
			assert (!r2.run(Tools.arr2str(w)));
		}

		// Destroy libalf objects
		dispatcherAlgorithm.destroy();
		dispatcherBase.destroy();
	}

	public static void main(String[] args) {
		Automaton a = new RegExp("(\u0000\u0000)+|(\u0001)*").toAutomaton();
		a.determinize();
		Set<int[]> S_plus = new HashSet<int[]>();
		Set<int[]> S_minus = new HashSet<int[]>();
		createSampleSets(a, 5, S_plus, S_minus);

		System.out.println("S_plus: " + S_plus.size() + ", S_minus: "
				+ S_minus.size());
		for (int[] w : S_plus) {
			System.out.println(Tools.arr2readableString(w));
		}
		System.out.println();
		for (int[] w : S_minus) {
			System.out.println(Tools.arr2readableString(w));
		}
	}
}
