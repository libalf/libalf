package de.libalf.jni;

/**
 * <p>
 * A knowledgebase stores information about the membership of words <em>w</em>
 * with respect to a formal language <em>L</em>. A word is a sequence of
 * integers represented as an integer array. The knowledgebase stores either
 * {@link Acceptance#ACCEPT} or {@link Acceptance#REJECT} depending on whether
 * <em>w</em> belongs to <em>L</em> or not. If this information is not yet
 * available, the word is marked as {@link Acceptance#UNKNOWN}.<br>
 * Each {@link LearningAlgorithm} is associated with a knowledgebase and
 * retrieves its exclusively information from it. However, a knowledgebase can
 * be an information source for many learning algorithms simultaneously.
 * </p>
 * <p>
 * If the learning algorithm is able to ask <em>membership queries</em> - like
 * Angluin's algorithm (see {@link AlgorithmAngluin}) -, the algorithm add the
 * words to the knowledgebase and marks them as {@link Acceptance#UNKNOWN}. Such
 * unknown words can then be retrieved via the
 * {@link Knowledgebase#getQueries()} -method. A query can be answered by
 * calling {@link Knowledgebase#add_knowledge(int[], boolean)}.
 * </p>
 * <p>
 * <b>Note:</b><br>
 * <ul>
 * <li>This is a Java implementation of the <em>knowledgebase</em> C++ class.
 * All method calls are forwarded to the LibALF C++ library via the JNI
 * interface.</li>
 * <li>
 * This JavaDoc is only a rough overview. For a detailed documentation please
 * refer to the original LibALF C++ documentation.</li>
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 */
public class Knowledgebase extends LibALFObject {

	private final int ACCEPTANCE_TRUE = 2;
	private final int ACCEPTANCE_FALSE = 0;
	private final int ACCEPTANCE_UNKNOWN = 1;

	/**
	 * Indicates whether a word belongs to a formal language or not or whether
	 * this information is not known.
	 * 
	 * @author Daniel Neider (<a
	 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
	 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
	 *         University
	 * @version 1.0
	 * 
	 */
	public enum Acceptance {
		ACCEPT, REJECT, UNKNOWN
	}

	/**
	 * Creates an empty knowledgebase.
	 */
	public Knowledgebase() {
		this.pointer = init();
	}

	/**
	 * <p>
	 * <em>JNI metod call:</em>
	 * </p>
	 * Initializes a new C++ knowledgebase object without any parameters and
	 * returns the pointer to this object.
	 * 
	 * @return a pointer to the memory location of the new C++ object.
	 */
	private native long init();

	/**
	 * Checks whether there are any unanswered questions, i.e. if there are any
	 * words marked as {@link Acceptance#UNKNOWN}.
	 * 
	 * @return the number of unanswered queries.
	 */
	public boolean is_answered() {
		return is_answered(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#is_answered()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean is_answered(long pointer);

	/**
	 * <p>
	 * Returns all words stored in the knowledgebase.
	 * </p>
	 * <p>
	 * Note that for efficiency reasons the knowledgebase also stores prefixes
	 * of all added words.
	 * <p>
	 * 
	 * @return all words stored in the knowledgebase.
	 */
	public WordList getKnowledge() {
		return getKnowledge(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#getKnowledge()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native WordList getKnowledge(long pointer);

	/**
	 * Checks whether the knowledgebase is empty.
	 * 
	 * @return <em>true</em>, if the knowledgebase is empty and otherwise
	 *         <em>false</em>.
	 */
	public boolean is_empty() {
		return is_empty(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#is_empty()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean is_empty(long pointer);

	/**
	 * Counts the queries contained in the knowledgebase, i.e. the words marked
	 * as {@link Acceptance#UNKNOWN}.
	 * 
	 * @return the number of queries.
	 */
	public int count_queries() {
		return count_queries(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#count_queries()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int count_queries(long pointer);

	/**
	 * Retrieves the list of queries stored in the knowledgebase.
	 * 
	 * @return a list of queries. The result is always a valid object even if
	 *         there are no queries.
	 */
	public WordList getQueries() {
		return getQueries(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#count_queries()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native WordList getQueries(long pointer);

	/**
	 * Looks for the word in the knowledgebase. If the word is known, either
	 * {@link Acceptance#ACCEPT} or {@link Acceptance#REJECT} is returned. If the word is not present, the method 
	 * 
	 * @param word
	 * @return
	 */
	public Acceptance resolve_query(int[] word) {
		int acceptance = resolve_query(word, this.pointer);
		switch (acceptance) {
		case ACCEPTANCE_FALSE:
			return Acceptance.REJECT;
		case ACCEPTANCE_TRUE:
			return Acceptance.ACCEPT;
		case ACCEPTANCE_UNKNOWN:
			return Acceptance.UNKNOWN;
		default:
			System.err.println("Unknown return value '" + acceptance
					+ "' of native mathod resolve_query! Returning 'UNKNOWN'");
			return Acceptance.UNKNOWN;
		}
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#resolve_query(int[])}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int resolve_query(int[] word, long pointer);

	public Acceptance resolve_or_add_query(int[] word) {
		int acceptanceOrExists = resolve_or_add_query(word, this.pointer);
		switch (acceptanceOrExists) {
		case ACCEPTANCE_FALSE:
			return Acceptance.REJECT;
		case ACCEPTANCE_TRUE:
			return Acceptance.ACCEPT;
		case ACCEPTANCE_UNKNOWN:
			return Acceptance.UNKNOWN;
		default:
			System.err
					.println("Unknown return value '"
							+ acceptanceOrExists
							+ "' of native mathod resolve_or_add_query! Returning 'UNKNOWN'");
			return Acceptance.UNKNOWN;
		}
	}

	private native int resolve_or_add_query(int[] word, long pointer);

	public boolean add_knowledge(int[] word, boolean acceptance) {
		return add_knowledge(word, acceptance, this.pointer);
	}

	private native boolean add_knowledge(int[] word, boolean acceptance,
			long pointer);

	public void clear() {
		clear(this.pointer);
	}

	private native void clear(long pointer);

	public void clear_queries() {
		clear_queries(this.pointer);
	}

	private native void clear_queries(long pointer);

	public boolean undo(int count) {
		return undo(count, this.pointer);
	}

	private native boolean undo(int count, long pointer);

	public int get_memory_usage() {
		return get_memory_usage(this.pointer);
	}

	private native int get_memory_usage(long pointer);

	public int get_timestamp() {
		return get_timestamp(this.pointer);
	}

	private native int get_timestamp(long pointer);

	public int count_answers() {
		return count_answers(this.pointer);
	}

	private native int count_answers(long pointer);

	public String generate_dotfile() {
		return generate_dotfile(this.pointer);
	}

	private native String generate_dotfile(long pointer);

	public int[] serialize() {
		return serialize(this.pointer);
	}

	private native int[] serialize(long pointer);

	public boolean deserialize(int[] serialization) {
		return deserialize(serialization, this.pointer);
	}

	private native boolean deserialize(int[] serialization, long pointer);

	public String toString() {
		return tostring(this.pointer);
	}

	private native String tostring(long pointer);
}
