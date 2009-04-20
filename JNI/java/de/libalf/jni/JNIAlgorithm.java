package de.libalf.jni;

/**
 * Simply override the constructor to initialize a certain learning algorithm. All
 * other methods are provided by this class.
 *
 * Also make sure that this class cannot be initialized.
 *
 * @author Daniel Neider, Chair of Computer Science 7, RWTH Aachen University
 * @version 1.0
 */
public abstract class JNIAlgorithm extends LibALFObject implements LearningAlgorithm {

	Knowledgebase knowledgebase;

	BufferedLogger logger;
	
	@Override
	public Knowledgebase get_knowledge_source() {
		return knowledgebase;
	}

	@Override
	public void add_counterexample(int[] counterexample) {
		add_counterexample(counterexample, this.pointer);
	}

	private native void add_counterexample(int[] counterexample, long pointer);

	@Override
	public BasicAutomaton advance() {
		return advance(this.pointer);
	}

	private native BasicAutomaton advance(long pointer);

	@Override
	public boolean conjecture_ready() {
		return conjecture_ready(this.pointer);
	}

	private native boolean conjecture_ready(long pointer);

	@Override
	public int get_alphabet_size() {
		return get_alphabet_size(this.pointer);
	}

	private native int get_alphabet_size(long pointer);

	@Override
	public void increase_alphabet_size(int newSize) {
		increase_alphabet_size(newSize, this.pointer);

	}

	private native void increase_alphabet_size(int newSize, long pointer);

	@Override
	public void set_alphabet_size(int alphabetSize) {
		set_alphabet_size(alphabetSize, this.pointer);

	}

	private native void set_alphabet_size(int alphabetSize, long pointer);

	@Override
	public void set_knowledge_source(Knowledgebase base) {
		if (base == null)
			set_knowledge_source_NULL(this.pointer);
		else
			set_knowledge_source(base.getPointer(), this.pointer);
	}

	private native void set_knowledge_source(long knowledgebase_pointer,
			long pointer);

	private native void set_knowledge_source_NULL(long pointer);

	@Override
	public boolean sync_to_knowledgebase() {
		return sync_to_knowledgebase(this.pointer);
	}

	private native boolean sync_to_knowledgebase(long pointer);

	@Override
	public boolean supports_sync() {
		return supports_sync(this.pointer);
	}

	private native boolean supports_sync(long pointer);

	@Override
	public int[] serialize() {
		return serialize(this.pointer);
	}

	private native int[] serialize(long pointer);
	
	@Override
	public boolean deserialize(int[] serialization) {
		return deserialize(serialization, this.pointer);
	}

	private native boolean deserialize(int[] serialization, long pointer);

	@Override
	public void set_logger(BufferedLogger logger) {
		this.logger = logger;
		set_logger(this.logger.getPointer(), this.pointer);
	}
	
	private native void set_logger(long logger_pointer, long pointer); 
	
	@Override	
	public String toString() {
		return tostring(this.pointer);
	}

	private native String tostring(long pointer);
}
