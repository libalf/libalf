package de.libalf;

import java.util.LinkedList;

/**
 * <p>
 * A knowledgebase manages the information used during the learning process. It
 * stores information about the membership of words <em>w</em> with respect to a
 * formal language <em>L</em>. A word is a sequence of integers represented as
 * an integer array. The knowledgebase stores either {@link Acceptance#ACCEPT}
 * or {@link Acceptance#REJECT} depending on whether <em>w</em> belongs to
 * <em>L</em> or not. If this information is not yet available, the word is
 * marked as {@link Acceptance#UNKNOWN}.
 * </p>
 * <p>
 * Each {@link LearningAlgorithm} is associated with a knowledgebase and
 * retrieves its information exclusively this particular knowledgebase. However,
 * a knowledgebase can be an information source for many learning algorithms
 * simultaneously.<br>
 * <em>Note: Synchronization (e.g. via the Java sync facility) is not yet supported.</em>
 * </p>
 * <p>
 * If the learning algorithm is able to ask <em>membership queries</em> - like
 * Angluin's algorithm (see {@link JNIAlgorithmAngluin}) -, the algorithm add the
 * words to the knowledgebase and marks them as {@link Acceptance#UNKNOWN}. Such
 * unknown words can then be retrieved via the
 * {@link Knowledgebase#getQueries()} -method. A query can be answered by
 * calling {@link Knowledgebase#add_knowledge(int[], boolean)} providing the
 * word and its information.
 * </p>
 * <p>
 * <b>Note:</b>
 * <ul>
 * <li>This is a Java implementation of the <em>knowledgebase</em> C++ class.
 * All method calls are forwarded to the LibALF C++ library via the JNI
 * interface.</li>
 * <li>
 * This JavaDoc is only a rough overview. For a detailed documentation please
 * refer to the original LibALF C++ documentation.</li>
 * </ul>
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 */
public interface Knowledgebase extends LibALFObject {

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

		/**
		 * Indicates that the word belongs to the language.
		 */
		ACCEPT,

		/**
		 * Indicates that the word does not belongs to the language.
		 */
		REJECT,
		
		/**
		 * Doesn't matter if the word is accepted or rejected.
		 */
		@Deprecated
		DONTCARE,

		/**
		 * There is no information about the word.
		 */
		UNKNOWN
	}

	/**
	 * Checks whether there are any unanswered questions, i.e. if there are any
	 * words marked as {@link Acceptance#UNKNOWN}.
	 * 
	 * @return the number of unanswered queries.
	 */
	public abstract boolean is_answered();

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
	public abstract LinkedList<int[]> get_knowledge();

	/**
	 * Checks whether the knowledgebase is empty.
	 * 
	 * @return <em>true</em>, if the knowledgebase is empty and otherwise
	 *         <em>false</em>.
	 */
	public abstract boolean is_empty();

	/**
	 * Counts the queries contained in the knowledgebase, i.e. the words marked
	 * as {@link Acceptance#UNKNOWN}.
	 * 
	 * @return the number of queries.
	 */
	public abstract int count_queries();

	/**
	 * Answer a list of queries in the same order as retrieved by {@link #get_queries()}.
	 */
	public void deserialize_query_acceptance(boolean[] acceptances);

	/**
	 * Retrieves the list of queries stored in the knowledgebase.
	 * 
	 * @return a list of queries. The result is always a valid object even if
	 *         there are no queries.
	 */
	public abstract LinkedList<int[]> get_queries();

	/**
	 * Looks for the word in the knowledgebase. If the word is known, either
	 * {@link Acceptance#ACCEPT} or {@link Acceptance#REJECT} is returned. If
	 * the word is not present, the method returns {@link Acceptance#UNKNOWN}.
	 * 
	 * @param word
	 *            the word to look for
	 * @return the stored information about the word.
	 */
	public abstract Acceptance resolve_query(int[] word);

	/**
	 * Looks for the word in the knowledgebase. If the word is known, either
	 * {@link Acceptance#ACCEPT} or {@link Acceptance#REJECT} is returned. If
	 * the word is not present, it is added to the knowledgebase and marked as
	 * unknown. In this case the method returns {@link Acceptance#UNKNOWN}.
	 * Hence, the {@link Knowledgebase#getQueries()}-method returns the word as
	 * query at the next call.
	 * 
	 * @param word
	 *            the word to look for
	 * @return the stored information about the word.
	 */
	public abstract Acceptance resolve_or_add_query(int[] word);

	/**
	 * Stores the information about the word in the knowledgebase. The
	 * information added by this method is persistent meaning that it cannot be
	 * altered afterwards (except for having been removed by an
	 * {@link Knowledgebase#undo(int)}).
	 * 
	 * @param word
	 *            the word to add to the knowledgebase
	 * @param acceptance
	 *            the information associated with the word
	 * @return <ul>
	 *         <li><em>true</em> if the word is not known to to knowledgebase or
	 *         if the word is known and the provided information is the same as
	 *         stored in the knowledgebase.</li>
	 *         <li><em>false</em> if the word is known but does not match the
	 *         information stored in the knowledgebase.</li>
	 *         </ul>
	 * 
	 */
	public boolean add_knowledge(int[] word, boolean acceptance) throws AlfException;

	/**
	 * Removes all data from the knowledgebase, i.e. the knowledgebase is empty.
	 */
	public abstract void clear();

	/**
	 * Removes all queries from the knowledgebase. All known information is left
	 * unchanged.
	 */
	public abstract void clear_queries();

	/**
	 * Undoes the last operations on the knowledgebase.
	 * 
	 * @param count
	 *            the number of undo operations
	 * @return true, if the operation was successful and false, otherwise.
	 */
	public abstract boolean undo(int count);

	/**
	 * Calculates the memory consumption of the knowledgebase.
	 * 
	 * @return the consumed memory in bytes.
	 */
	public abstract int get_memory_usage();

	/**
	 * Returns the number of membership information stored in the knowledgebase.
	 * 
	 * @return Something
	 */
	public abstract int count_answers();

	/**
	 * Generates a 'dot' representations of the knowledgebase (also see <a
	 * href="http://www.graphviz.org/">Graphviz</a>).
	 * 
	 * @return a dot representation of the knowledgebase.
	 */
	public abstract String generate_dotfile();

	/**
	 * Serializes the data stored in the knowledgebase. This serialization can
	 * be saved and the knowledgebase can be restored using the
	 * {@link Knowledgebase#deserialize(int[])} method.
	 * 
	 * @return an int array that stores the serialization of the knowledgebase.
	 */
	public abstract int[] serialize();

	/**
	 * Restores the data of an a priori serialized knowledgebase. All data
	 * contained in the knowledgebase is dropped before.
	 * 
	 * @param serialization
	 *            a serialization of a knowledgebase
	 * @return true, if the recovery was successful and false, otherwise.
	 */
	public abstract boolean deserialize(int[] serialization);

}