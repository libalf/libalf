package de.libalf.dispatcher;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;


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
 * Angluin's algorithm (see {@link AlgorithmAngluin}) -, the algorithm add the
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
 * <li>This JavaDoc is only a rough overview. For a detailed documentation
 * please refer to the original LibALF C++ documentation.</li>
 * </ul>
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 */
public class Knowledgebase extends LibALFObject implements Serializable {
	private static final long serialVersionUID = 2L;

	private static final int ACCEPTANCE_TRUE = 2;
	private static final int ACCEPTANCE_FALSE = 0;
	private static final int ACCEPTANCE_UNKNOWN = 1;

	/**
	 * Indicates whether a word belongs to a formal language or not or whether
	 * this information is not known.
	 * 
	 * @author Daniel Neider (<a
	 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
	 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
	 *         University
	 * @version 1.0
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
		 * There is no information about the word.
		 */
		UNKNOWN
	}

	/**
	 * Creates an empty knowledgebase.
	 */
	Knowledgebase(DispatcherSession session) {
		super(session, ObjectType.KNOWLEDGEBASE);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++ knowledgebase object
	 * without any parameters and returns the pointer to this object.
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
		return is_answered(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#is_answered()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
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
		return getKnowledge(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#getKnowledge()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
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
		return is_empty(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#is_empty()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
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
		return count_queries(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#count_queries()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
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
		return getQueries(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#count_queries()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native WordList getQueries(long pointer);

	/**
	 * Looks for the word in the knowledgebase. If the word is known, either
	 * {@link Acceptance#ACCEPT} or {@link Acceptance#REJECT} is returned. If
	 * the word is not present, the method returns {@link Acceptance#UNKNOWN}.
	 * 
	 * @param word the word to look for
	 * @return the stored information about the word.
	 */
	public Acceptance resolve_query(int[] word) {
		int acceptance = resolve_query(word, this.id);
		switch (acceptance) {
		case ACCEPTANCE_FALSE:
			return Acceptance.REJECT;
		case ACCEPTANCE_TRUE:
			return Acceptance.ACCEPT;
		case ACCEPTANCE_UNKNOWN:
			return Acceptance.UNKNOWN;
		default:
			System.err.println("Unknown return value '" + acceptance + "' of native mathod resolve_query! Returning 'UNKNOWN'");
			return Acceptance.UNKNOWN;
		}
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#resolve_query(int[])}.
	 * </p>
	 * 
	 * @param word the word to look for
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int resolve_query(int[] word, long pointer);

	/**
	 * Looks for the word in the knowledgebase. If the word is known, either
	 * {@link Acceptance#ACCEPT} or {@link Acceptance#REJECT} is returned. If
	 * the word is not present, it is added to the knowledgebase and marked as
	 * unknown. In this case the method returns {@link Acceptance#UNKNOWN}.
	 * Hence, the {@link Knowledgebase#getQueries()}-method returns the word as
	 * query at the next call.
	 * 
	 * @param word the word to look for
	 * @return the stored information about the word.
	 */
	public Acceptance resolve_or_add_query(int[] word) {
		int acceptanceOrExists = resolve_or_add_query(word, this.id);
		switch (acceptanceOrExists) {
		case ACCEPTANCE_FALSE:
			return Acceptance.REJECT;
		case ACCEPTANCE_TRUE:
			return Acceptance.ACCEPT;
		case ACCEPTANCE_UNKNOWN:
			return Acceptance.UNKNOWN;
		default:
			System.err.println("Unknown return value '" + acceptanceOrExists + "' of native mathod resolve_or_add_query! Returning 'UNKNOWN'");
			return Acceptance.UNKNOWN;
		}
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link Knowledgebase#resolve_or_add_query(int[])}.
	 * </p>
	 * 
	 * @param word the word to look for
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int resolve_or_add_query(int[] word, long pointer);

	/**
	 * Stores the information about the word in the knowledgebase. The
	 * information added by this method is persistent meaning that it cannot be
	 * altered afterwards (except for having been removed by an
	 * {@link Knowledgebase#undo(int)}).
	 * 
	 * @param word the word to add to the knowledgebase
	 * @param acceptance the information associated with the word
	 * @return <ul>
	 *         <li><em>true</em> if the word is not known to to knowledgebase or
	 *         if the word is known and the provided information is the same as
	 *         stored in the knowledgebase.</li>
	 *         <li><em>false</em> if the word is known but does not match the
	 *         information stored in the knowledgebase.</li>
	 *         </ul>
	 */
	public boolean add_knowledge(int[] word, boolean acceptance) {
		return add_knowledge(word, acceptance, this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link Knowledgebase#add_knowledge(int[], boolean)}.
	 * </p>
	 * 
	 * @param word the word to add to the knowledgebase
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean add_knowledge(int[] word, boolean acceptance, long pointer);

	/**
	 * Removes all data from the knowledgebase, i.e. the knowledgebase is empty.
	 */
	public void clear() {
		clear(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#clear()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
	 */
	private native void clear(long pointer);

	/**
	 * Removes all queries from the knowledgebase. All known information is left
	 * unchanged.
	 */
	public void clear_queries() {
		clear_queries(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#clear_queries()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
	 */
	private native void clear_queries(long pointer);

	/**
	 * Undoes the last operations on the knowledgebase.
	 * 
	 * @param count the number of undo operations
	 * @return true, if the operation was successful and false, otherwise.
	 */
	public boolean undo(int count) {
		return undo(count, this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#undo(int)}.
	 * </p>
	 * 
	 * @param count the number of undo operations
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean undo(int count, long pointer);

	/**
	 * Calculates the memory consumption of the knowledgebase.
	 * 
	 * @return the consumed memory in bytes.
	 */
	public int get_memory_usage() {
		return get_memory_usage(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#get_memory_usage()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int get_memory_usage(long pointer);

	/**
	 * Returns the number of membership information stored in the knowledgebase.
	 * 
	 * @return Something
	 */
	public int count_answers() {
		return count_answers(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#count_answers()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int count_answers(long pointer);

	/**
	 * Generates a 'dot' representations of the knowledgebase (also see <a
	 * href="http://www.graphviz.org/">Graphviz</a>).
	 * 
	 * @return a dot representation of the knowledgebase.
	 */
	public String generate_dotfile() {
		return generate_dotfile(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#generate_dotfile()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native String generate_dotfile(long pointer);

	/**
	 * Serializes the data stored in the knowledgebase. This serialization can
	 * be saved and the knowledgebase can be restored using the
	 * {@link Knowledgebase#deserialize(int[])} method.
	 * 
	 * @return an int array that stores the serialization of the knowledgebase.
	 */
	public int[] serialize() {
		return serialize(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#generate_dotfile()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int[] serialize(long pointer);

	/**
	 * Restores the data of an a priori serialized knowledgebase. All data
	 * contained in the knowledgebase is dropped before.
	 * 
	 * @param serialization a serialization of a knowledgebase
	 * @return true, if the recovery was successful and false, otherwise.
	 */
	public boolean deserialize(int[] serialization) {
		return deserialize(serialization, this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#deserialize(int[])}.
	 * </p>
	 * 
	 * @param serialization a serialization of a knowledgebase
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean deserialize(int[] serialization, long pointer);

	@Override
	public String toString() {
		return tostring(this.id);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#toString()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native String tostring(long pointer);

	public static void main(String[] args) {
		Knowledgebase base = new Knowledgebase();
		base.add_knowledge(new int[] {}, true);
		base.add_knowledge(new int[] { 2, 1, 4 }, false);
		base.add_knowledge(new int[] { 2, 4, 5 }, false);
		base.add_knowledge(new int[] { 2, 4 }, true);
		base.add_knowledge(new int[] { 2, 4, 22 }, true);
		base.add_knowledge(new int[] { 2, 4, 22, 33 }, false);
		System.out.println(base.generate_dotfile());
	}

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException {
		out.defaultWriteObject();
		out.writeObject(serialize());
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		this.id = init();
		int[] serialization = (int[]) in.readObject();
		deserialize(serialization);
	}
}
