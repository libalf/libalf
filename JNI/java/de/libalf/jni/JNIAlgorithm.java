package de.libalf.jni;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

/**
 * <p>
 * JNI implementation of the {@link LearningAlgorithm} interface. This class
 * forwards all method calls to the corresponding C++ object. However, since
 * this code is used by all LibALF learning algorithms, this class cannot be
 * instantiated.
 * </p>
 * <p>
 * To provide a class for a (new) learning algorithm, simply derive a new class
 * and override the constructor to initialize a C++ object. All other methods
 * are provided by this class.
 * </p>
 * <p>
 * <b>Note:</b>
 * <ul>
 * <li>This is a Java implementation of the <em>learning_algorithm</em> C++
 * class. All method calls are forwarded to the LibALF C++ library via the JNI
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
public abstract class JNIAlgorithm extends LibALFObject implements
		LearningAlgorithm {
	private static final long serialVersionUID = 1L;

	/**
	 * The knowledge base from which the learning algorithm gets its data.
	 */
	Knowledgebase knowledgebase;

	/**
	 * The logger to log events to.
	 */
	BufferedLogger logger;

	protected transient long pointer;
	
	@Override
	public long getPointer() {
		return this.pointer;
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>angluin_simple_observationtable</code> object with the pointer to a
	 * <code>knowledgebase</code> and the size of the alphabet. The pointer to
	 * the new created C++ object is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @return a pointer to the memory location of the new C++ object.
	 */
	abstract long init(long knowledgebase_pointer, int alphabet_size);

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>angluin_simple_observationtable</code> object with the pointer to a
	 * <code>knowledgebase</code>, the size of the alphabet and a pointer to a
	 * <code>buffered_logger</code>. The pointer to the new created C++ object
	 * is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger_pointer
	 *            a pointer to a buffered_logger C++ object
	 * @return a pointer to the memory location of the new C++ object.
	 */
	abstract long init(long knowledgebase_pointer, int alphabet_size, long logger_pointer);

	@Override
	public Knowledgebase get_knowledge_source() {
		return knowledgebase;
	}

	@Override
	public void add_counterexample(int[] counterexample) {
		add_counterexample(counterexample, this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithm#add_counterexample(int[])}.
	 * </p>
	 * 
	 * @param counterexample
	 *            the new counter-example to add
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native void add_counterexample(int[] counterexample, long pointer);

	@Override
	public BasicAutomaton advance() {
		return advance(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#advance()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native BasicAutomaton advance(long pointer);

	@Override
	public boolean conjecture_ready() {
		return conjecture_ready(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#conjecture_ready()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean conjecture_ready(long pointer);

	@Override
	public int get_alphabet_size() {
		return get_alphabet_size(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#get_alphabet_size()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int get_alphabet_size(long pointer);

	@Override
	public void increase_alphabet_size(int newSize) {
		increase_alphabet_size(newSize, this.pointer);

	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithm#increase_alphabet_size(int)}.
	 * </p>
	 * 
	 * @param new_size
	 *            the new size of the alphabet
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void increase_alphabet_size(int newSize, long pointer);

	@Override
	public void set_alphabet_size(int alphabetSize) {
		set_alphabet_size(alphabetSize, this.pointer);

	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#set_alphabet_size(int)}
	 * .
	 * </p>
	 * 
	 * @param alphabet_size
	 *            the size of the alphabet
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void set_alphabet_size(int alphabetSize, long pointer);

	@Override
	public void set_knowledge_source(Knowledgebase base) {
		if (base == null)
			set_knowledge_source_NULL(this.pointer);
		else
			set_knowledge_source(base.getPointer(), this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithm#set_knowledge_source(Knowledgebase)}.
	 * </p>
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to the knowledgebase to set or replace.
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void set_knowledge_source(long knowledgebase_pointer,
			long pointer);

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithm#set_knowledge_source(Knowledgebase)} where
	 * <code>Knowledgebase</code> is <code>null</code>.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void set_knowledge_source_NULL(long pointer);

	@Override
	public boolean sync_to_knowledgebase() {
		return sync_to_knowledgebase(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithm#sync_to_knowledgebase()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean sync_to_knowledgebase(long pointer);

	@Override
	public boolean supports_sync() {
		return supports_sync(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#supports_sync()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean supports_sync(long pointer);

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#writeObject(ObjectOutputStream)}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int[] serialize(long pointer);
	
	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException
    {
		out.defaultWriteObject();
		out.writeInt(get_alphabet_size());
		out.writeObject(serialize(this.pointer));
    }

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#readObject(ObjectInputStream)}.
	 * </p>
	 * 
	 * @param serialization
	 *            a serialization of a learning algorithm
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean deserialize(int[] serialization, long pointer);
	
	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		int alphabet_size = in.readInt();
		this.pointer = init(knowledgebase.getPointer(), alphabet_size, logger.getPointer());
		int[] serialization = (int[]) in.readObject();
		deserialize(serialization , this.pointer);
	}

	@Override
	public void set_logger(BufferedLogger logger) {
		this.logger = logger;
		set_logger(this.logger.getPointer(), this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithm#set_logger(BufferedLogger)}.
	 * </p>
	 * 
	 * @param logger_pointer
	 *            a pointer to a <code>buffered_logger</code> C++ object
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void set_logger(long logger_pointer, long pointer);

	@Override
	public String toString() {
		return tostring(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#toString()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native String tostring(long pointer);
}
