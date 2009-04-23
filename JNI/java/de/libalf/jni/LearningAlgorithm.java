package de.libalf.jni;

/**
 * <p>
 * The root interface of all learning algorithms.
 * </p>
 * <p>
 * In general, each learning algorithm 
 * <ul>
 * <li>Alphabet</li>
 * <li>knowledge source</li>
 * </ul>
 * 
 * ADVANCE
 * 
 * UNDO operations
 * 
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 */
public interface LearningAlgorithm {

	/**
	 * Sets the size of the alphabet to a new value.
	 * 
	 * @param alphabet_size
	 *            the size of the alphabet
	 */
	public void set_alphabet_size(int alphabet_size);

	/**
	 * Returns the size of the alphabet.
	 * 
	 * @return the size of the alphabet.
	 */
	public int get_alphabet_size();

	/**
	 * Increases the alphabet's size to a new value.
	 * 
	 * @param new_size
	 *            the new size of the alphabet
	 */
	public void increase_alphabet_size(int new_size);

	/**
	 * Sets or replaces the knowledge source of the learning algorithm.
	 * 
	 * @param base
	 *            the knowledgebase to set or replace.
	 */
	public void set_knowledge_source(Knowledgebase base);

	/**
	 * Returns the current knowledgebase.
	 * 
	 * @return the current knowledgebase.
	 */
	public Knowledgebase get_knowledge_source();

	/**
	 * Checks whether the learning algorithm is able to provide a conjecture.
	 * 
	 * @return <code>true</code>, if the learning algorithm is able to provide a
	 *         conjecture or <code>false</code>, otherwise.
	 */
	public boolean conjecture_ready();

	/**
	 * Advances one step in the learning algorithm.
	 * 
	 * @return <ul>
	 *         <li>A conjecture if the learning algorithm is able to propose
	 *         one, or</li>
	 *         <li><code>null</code>, otherwise.</li>
	 *         </ul>
	 */
	public BasicAutomaton advance();

	/**
	 * <p>
	 * Provides the learning algorithm with a new counter-example.<br>
	 * TODO: (Stimmt das?) The counter-example may be processed by the learning
	 * algorithm before it is added to the knowledge base.
	 * </p>
	 * <p>
	 * <b>Note:</b> Adding counter-example is only possible in case of
	 * <em>online learning algorithms</em>. For
	 * <em>offline learning algorithms</em> this method is a stub.
	 * </p>
	 * 
	 * @param counterexample
	 *            the new counter-example to add
	 */
	public void add_counterexample(int[] counterexample);

	/**
	 * Synchronizes the learning algorithm with the associated knowledge base.
	 * 
	 * @return TODO: Was passiert wann?
	 */
	public boolean sync_to_knowledgebase();

	/**
	 * Checks whether the learning algorithm supports synchronization with the
	 * associated {@link Knowledgebase}.
	 * 
	 * @return the result of the check.
	 */
	public boolean supports_sync();

	/**
	 * Serializes the state of the learning algorithm. This serialization can be
	 * saved and the learning algorithm can be restored using the
	 * {@link LearningAlgorithm#deserialize(int[])} method.<br>
	 * Be sure to also serialize the associated {@link Knowledgebase}.
	 * 
	 * @return an int array that stores the serialization of the learning
	 *         algorithm.
	 */
	public int[] serialize();

	/**
	 * Restores the data of an a priori serialized learning algorithm. The
	 * current state of the learning algorithms is discarded.
	 * 
	 * @param serialization
	 *            a serialization of a learning algorithm
	 * @return true, if the recovery was successful and false, otherwise.
	 */
	public boolean deserialize(int[] serialization);

	/**
	 * Sets the logger for this learning algorithm.
	 * 
	 * @param logger
	 *            the new logger
	 */
	public void set_logger(BufferedLogger logger);

	/**
	 * Returns a textual representation of the learning algorithm.
	 * 
	 * @return a textual representation of the learning algorithm.
	 */
	@Override
	public String toString();
}
