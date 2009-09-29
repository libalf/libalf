package de.libalf.jni;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.LinkedList;

import de.libalf.Knowledgebase;

/**
 * JNI implementation of a <code>Knowledgebase</code>.
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 */
public class JNIKnowledgebase extends JNIObject implements Knowledgebase,
		Serializable {

	private static final long serialVersionUID = 2L;

	private static final int ACCEPTANCE_TRUE = 2;
	
	private static final int ACCEPTANCE_FALSE = 0;
	
	private static final int ACCEPTANCE_UNKNOWN = 1;

	/**
	 * Creates an empty knowledgebase.
	 */
	public JNIKnowledgebase() {
		this.pointer = init();
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

	@Override
	public boolean is_answered() {
		check();
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

	@Override
	public LinkedList<int[]> get_knowledge() {
		check();
		return get_knowledge(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#get_knowledge()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native LinkedList<int[]> get_knowledge(long pointer);

	@Override
	public boolean is_empty() {
		check();
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

	@Override
	public int count_queries() {
		check();
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

	@Override
	public LinkedList<int[]> get_queries() {
		check();
		return get_queries(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#get_queries()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native LinkedList<int[]> get_queries(long pointer);

	@Override
	public Acceptance resolve_query(int[] word) {
		check();
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
	 * @param word
	 *            the word to look for
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int resolve_query(int[] word, long pointer);

	@Override
	public Acceptance resolve_or_add_query(int[] word) {
		check();
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

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link Knowledgebase#resolve_or_add_query(int[])}.
	 * </p>
	 * 
	 * @param word
	 *            the word to look for
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int resolve_or_add_query(int[] word, long pointer);

	@Override
	public boolean add_knowledge(int[] word, boolean acceptance) {
		check();
		return add_knowledge(word, acceptance, this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link Knowledgebase#add_knowledge(int[], boolean)}.
	 * </p>
	 * 
	 * @param word
	 *            the word to add to the knowledgebase
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean add_knowledge(int[] word, boolean acceptance,
			long pointer);

	@Override
	public void clear() {
		check();
		clear(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#clear()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void clear(long pointer);

	@Override
	public void clear_queries() {
		check();
		clear_queries(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#clear_queries()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void clear_queries(long pointer);

	@Override
	public boolean undo(int count) {
		check();
		return undo(count, this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#undo(int)}.
	 * </p>
	 * 
	 * @param count
	 *            the number of undo operations
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean undo(int count, long pointer);

	@Override
	public int get_memory_usage() {
		check();
		return get_memory_usage(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#get_memory_usage()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int get_memory_usage(long pointer);

	@Override
	public int count_answers() {
		check();
		return count_answers(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#count_answers()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int count_answers(long pointer);

	@Override
	public String generate_dotfile() {
		check();
		return generate_dotfile(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#generate_dotfile()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native String generate_dotfile(long pointer);

	@Override
	public int[] serialize() {
		check();
		return serialize(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#generate_dotfile()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int[] serialize(long pointer);

	@Override
	public boolean deserialize(int[] serialization) {
		check();
		return deserialize(serialization, this.pointer);
	}

	@Override
	public void destroy() {
		check();
		destroy(this.pointer);
		this.isAlive = false;
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNILearningAlgorithm#destroy()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void destroy(long pointer);
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#deserialize(int[])}.
	 * </p>
	 * 
	 * @param serialization
	 *            a serialization of a knowledgebase
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean deserialize(int[] serialization, long pointer);

	@Override
	public String toString() {
		check();
		return tostring(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link Knowledgebase#toString()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native String tostring(long pointer);

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException {
		check();
		out.defaultWriteObject();
		out.writeObject(serialize());
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException,
			ClassNotFoundException {
		check();
		in.defaultReadObject();
		this.pointer = init();
		int[] serialization = (int[]) in.readObject();
		deserialize(serialization);
	}
}