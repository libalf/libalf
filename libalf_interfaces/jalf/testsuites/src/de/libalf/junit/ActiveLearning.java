package de.libalf.junit;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.util.LinkedList;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import de.libalf.BasicAutomaton;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.Knowledgebase.Acceptance;
import de.libalf.LibALFFactory.Algorithm;
import de.libalf.dispatcher.DispatcherFactory;
import de.libalf.jni.JNIFactory;
import dk.brics.automaton.Automaton;

/**
 * <p>
 * Class to test jalf's online learning functionality. Both the JNI interface
 * and the dispatcher connectivity are tested simultaneously and the
 * intermediate steps and results are compared.
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
public class ActiveLearning {

	/**
	 * These algorithms are to be tested.
	 */
	static Algorithm[] availableAlgorithms = { Algorithm.ANGLUIN,
			Algorithm.ANGLUIN_COLUMN, Algorithm.KEARNS_VAZIRANI,
			Algorithm.NL_STAR, Algorithm.RIVEST_SCHAPIRE };

	/**
	 * The alphabet size used for testing
	 */
	public static int alphabetSize = 3;

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
		System.out.println("Server: " + server);
System.out.println("Port: " + port);

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
	public void testActiveLearning1() {
		Automaton a = AutomatonProvider.getAutomaton(1);
		System.out.println("=== Automaton 1 ===");
		learnAll(new Teacher(a));
	}

	/**
	 * Second test case.
	 */
	@Test
	public void testActiveLearning2() {
		Automaton a = AutomatonProvider.getAutomaton(2);
		System.out.println("=== Automaton 2 ===");
		learnAll(new Teacher(a));
	}

	/**
	 * Third test case.
	 */
	@Test
	public void testActiveLearning3() {
		Automaton a = AutomatonProvider.getAutomaton(3);
		System.out.println("=== Automaton 3 ===");
		learnAll(new Teacher(a));
	}

	/**
	 * Third fourth case.
	 */
	@Test
	public void testActiveLearning4() {
		Automaton a = AutomatonProvider.getAutomaton(4);
		System.out.println("=== Automaton 4 ===");
		learnAll(new Teacher(a));
	}

	/*
	 * Libalf objects for shared use
	 */
	static JNIFactory jniFactory;
	static DispatcherFactory dispatcherFactory;

	/**
	 * Runs all available active learning algorithms on the given teacher.
	 * 
	 * @param teacher
	 *            the teacher to learn from.
	 */
	private void learnAll(Teacher teacher) {
		for (Algorithm type : availableAlgorithms) {
			learnJNIAndDispatcher(type, teacher);
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
	 * @param teacher
	 *            the teacher to use
	 */
	private void learnJNIAndDispatcher(Algorithm type, Teacher teacher) {

		// Create libalf objects
		Knowledgebase dispatcherBase = dispatcherFactory.createKnowledgebase();
		LearningAlgorithm dispatcherAlgorithm = dispatcherFactory
				.createLearningAlgorithm(type, dispatcherBase, alphabetSize);

		Knowledgebase jniBase = jniFactory.createKnowledgebase();
		LearningAlgorithm jniAlgorithm = jniFactory.createLearningAlgorithm(
				type, jniBase, alphabetSize);

		System.out.print("Running " + jniAlgorithm.get_name() + " (JNI) / ");
		System.out.print(dispatcherAlgorithm.get_name() + " (dispatcher) ...");

		/*
		 * Main loop
		 */
		BasicAutomaton result = null;
		int eq_count = 0;
		do {

			// Advance
			BasicAutomaton jniHypothesis = (BasicAutomaton) jniAlgorithm
					.advance();
			BasicAutomaton dispatcherHypothesis = (BasicAutomaton) dispatcherAlgorithm
					.advance();

			// Check whether result of jni and dispatcher is equal
			if ((jniHypothesis == null && dispatcherHypothesis != null)
					|| (jniHypothesis != null && dispatcherHypothesis == null)) {
				fail("Unequal result of advance!");
			}

			// Membership query
			if (jniHypothesis == null) {

				LinkedList<int[]> jniQueries = jniBase.get_queries();
				LinkedList<int[]> dispatcherQueries = dispatcherBase
						.get_queries();

				// Compare queries
				assertTrue(Teacher.compare_membership_queries(jniQueries,
						dispatcherQueries));

				// Add knowledge to JNI
				for (int[] query : jniQueries) {

					// Check whether knowledge is already existing
					assertTrue(jniBase.resolve_query(query) == Acceptance.UNKNOWN);

					// Add knowledge
					jniBase.add_knowledge(query, teacher
							.membership_query(query));
				}

				// Add knowledge to dispatcher
				for (int[] query : dispatcherQueries) {

					// Check whether knowledge is already existing
					assertTrue(dispatcherBase.resolve_query(query) == Acceptance.UNKNOWN);

					// Add knowledge
					dispatcherBase.add_knowledge(query, teacher
							.membership_query(query));
				}

			}

			// Equivalence query
			else {

				// Increase eq_count
				eq_count++;
				assert (eq_count <= teacher.getAutomaton().getNumberOfStates());

				// Check automata
				Automaton jniBrics = Tools.libalf2brics(jniHypothesis);
				Automaton dispatcherBrics = Tools
						.libalf2brics(dispatcherHypothesis);

				// System.out.println(jniBrics.toDot());

				assertTrue(jniBrics.equals(dispatcherBrics));

				// Compute counter-example
				int[] jniCE = teacher.equivalence_query(jniHypothesis);
				int[] dispatcherCE = teacher
						.equivalence_query(dispatcherHypothesis);

				assertTrue(Tools.compare_arr(jniCE, dispatcherCE));

				// Counter-example found
				if (jniCE != null) {

					jniAlgorithm.add_counterexample(jniCE);
					dispatcherAlgorithm.add_counterexample(dispatcherCE);
				}

				// No counter-example found, return automaton
				else {
					result = jniHypothesis;
				}
			}

		} while (result == null);

		System.out.print(" (" + result.getNumberOfStates() + " states)");

		/*
		 * Destroy libalf objects
		 */
		dispatcherAlgorithm.destroy();
		dispatcherBase.destroy();
	}

}

/**
 * This class implements a teacher in Angluins active learning setup. The target
 * labguage of the learner is given as a brics automaton.
 * 
 * @author Daniel Neider (neider@automata.rwth-aachen.de)
 * @version 1.0
 */
class Teacher {

	/**
	 * The target language.
	 */
	private Automaton a;

	/**
	 * Creates a new Teacher with <code>a</code> as target language.
	 * 
	 * @param a
	 *            the automaton representing the target language
	 */
	public Teacher(Automaton a) {
		this.a = a;
	}

	/**
	 * Returns the automaton for the target language.
	 * 
	 * @return the automaton for the target language.
	 */
	public Automaton getAutomaton() {
		return a;
	}

	/**
	 * Performs a membership query: checks whether a given word belongs to the
	 * target language represented by this teacher or not.
	 * 
	 * @param word
	 *            the input to check
	 * @return wether the word belongs to the target language or not.
	 */
	public boolean membership_query(int[] word) {
		if (word == null)
			throw new java.lang.RuntimeException(
					"Word is 'null' in membership query!");

		return a.run(Tools.arr2str(word));
	}

	/**
	 * Performs an equivalence query on a given hypothesis. The result is either
	 * a counter-example or <code>null</code> if the hypothesis is equivalent to
	 * the target language represented by the teacher.
	 * 
	 * @param hypothesis
	 *            the hypothesis to check
	 * @return either a counter-example or <code>null</code>.
	 */
	public int[] equivalence_query(BasicAutomaton hypothesis) {

		Automaton bricsAutomaton = Tools.libalf2brics(hypothesis);

		Automaton diff = a.minus(bricsAutomaton);
		if (!diff.isEmpty()) {
			return Tools.str2arr(diff.getShortestExample(true));
		} else {

			diff = bricsAutomaton.minus(a);
			if (!diff.isEmpty()) {
				return Tools.str2arr(diff.getShortestExample(true));
			} else {
				return null;
			}
		}
	}

	/**
	 * Checks whether two lists contain the same queries.
	 * 
	 * @param queries1
	 *            the first list
	 * @param queries2
	 *            the second list
	 * @return whether <code>queries1==queries2</code>.
	 */
	public static boolean compare_membership_queries(
			LinkedList<int[]> queries1, LinkedList<int[]> queries2) {
		for (int[] q1 : queries1) {

			boolean found = false;
			for (int[] q2 : queries2) {
				if (Tools.compare_arr(q1, q2)) {
					found = true;
					break;
				}
			}

			if (!found) {
				System.out.println("Could not find "
						+ Tools.arr2readableString(q1));
				return false;
			}
		}

		for (int[] q2 : queries2) {

			boolean found = false;
			for (int[] q1 : queries1) {
				if (Tools.compare_arr(q1, q2)) {
					found = true;
					break;
				}
			}

			if (!found) {
				System.out.println("Could not find "
						+ Tools.arr2readableString(q2));
				return false;
			}
		}

		return true;
	}

}
