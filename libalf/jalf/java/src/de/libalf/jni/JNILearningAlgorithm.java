package de.libalf.jni;

import de.libalf.AlfException;
import de.libalf.BasicAutomaton;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.Logger;
import de.libalf.Normalizer;

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
public abstract class JNILearningAlgorithm extends JNIObject implements
		LearningAlgorithm {
	private static final long serialVersionUID = 2L;

	/**
	 * The knowledgebase from which the learning algorithm gets its data.
	 */
	protected JNIKnowledgebase knowledgebase;

	/**
	 * The logger to log events to.
	 */
	protected JNIBufferedLogger logger;

	/**
	 * The normalizer of this learning algorithm.
	 */
	protected JNINormalizer normalizer;

	@Override
	public JNIKnowledgebase get_knowledge_source() {
		check();
		return this.knowledgebase;
	}

	@Override
	public void add_counterexample(int[] counterexample) {
		check();
		add_counterexample(counterexample, this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#add_counterexample(int[])}.
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
		check();
		return advance(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNILearningAlgorithm#advance()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native BasicAutomaton advance(long pointer);

	@Override
	public boolean conjecture_ready() {
		check();
		return conjecture_ready(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#conjecture_ready()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean conjecture_ready(long pointer);

	@Override
	public int get_alphabet_size() {
		check();
		return get_alphabet_size(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#get_alphabet_size()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int get_alphabet_size(long pointer);

	@Override
	public void increase_alphabet_size(int newSize) {
		check();
		increase_alphabet_size(newSize, this.pointer);

	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#increase_alphabet_size(int)}.
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
		check();
		set_alphabet_size(alphabetSize, this.pointer);

	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#set_alphabet_size(int)} .
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
		check();
		if (base == null) {
			this.knowledgebase = null;
			set_knowledge_source_NULL(this.pointer);
		} else {
			this.knowledgebase = (JNIKnowledgebase) base;
			set_knowledge_source(this.knowledgebase.getPointer(), this.pointer);

		}
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#set_knowledge_source(Knowledgebase)}.
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
	 * {@link JNILearningAlgorithm#set_knowledge_source(Knowledgebase)} where
	 * <code>Knowledgebase</code> is <code>null</code>.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void set_knowledge_source_NULL(long pointer);

	@Override
	public boolean sync_to_knowledgebase() {
		check();
		return sync_to_knowledgebase(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#sync_to_knowledgebase()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean sync_to_knowledgebase(long pointer);

	@Override
	public boolean supports_sync() {
		check();
		return supports_sync(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#supports_sync()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean supports_sync(long pointer);

	@Override
	public int[] serialize() {
		check();
		return serialize(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNILearningAlgorithm#serialize()}.
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

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#deserialize(int[])}.
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
	public void set_logger(Logger logger) {
		check();
		this.logger = (JNIBufferedLogger) logger;
		set_logger(this.logger.getPointer(), this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#set_logger(BufferedLogger)}.
	 * </p>
	 * 
	 * @param logger_pointer
	 *            a pointer to a <code>buffered_logger</code> C++ object
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void set_logger(long logger_pointer, long pointer);

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

	@Override
	public String toString() {
		check();
		return tostring(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNILearningAlgorithm#toString()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native String tostring(long pointer);

	@Override
	public void remove_logger() throws AlfException {
		check();
		this.logger = null;
	}

	@Override
	public void set_normalizer(Normalizer normalizer) throws AlfException {
		check();
		if (normalizer == null) {
			this.normalizer = null;
			set_normalizer_NULL(this.pointer);
		} else {
			this.normalizer = (JNINormalizer) normalizer;
			set_normalizer(this.normalizer.getPointer(), this.pointer);
		}
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#set_normalizer(Normalizer)}.
	 * </p>
	 * 
	 * @param normalizer_pointer
	 *            the pointer to the normalizer C++ object.
	 * @param pointer
	 *            the pointer to the learning_algorithm C++ object.
	 */
	private native void set_normalizer(long normalizer_pointer, long pointer);

	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNILearningAlgorithm#set_normalizer(Normalizer)}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the learning_algorithm C++ object.
	 */
	private native void set_normalizer_NULL(long pointer);

	@Override
	public JNINormalizer get_normalizer() throws AlfException {
		check();
		return normalizer;
	}

	@Override
	public void remove_normalizer() throws AlfException {
		check();
		this.normalizer = null;
	}
}
