package de.libalf.jni;

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
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 */
public abstract class JNIAlgorithm extends LibALFObject implements
		LearningAlgorithm {

	/**
	 * The knowledgebase from which the learning algorithm gets its data.
	 */
	Knowledgebase knowledgebase;

	/**
	 * The logger to log events to.
	 */
	BufferedLogger logger;

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

	@Override
	public int[] serialize() {
		return serialize(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#serialize()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int[] serialize(long pointer);

	@Override
	public boolean deserialize(int[] serialization) {
		return deserialize(serialization, this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIAlgorithm#deserialize(int[])}.
	 * </p>
	 * 
	 * @param serialization
	 *            a serialization of a learning algorithm
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean deserialize(int[] serialization, long pointer);

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
