package de.libalf.jni;
public class AlgorithmAngluin extends LibALFObject implements LearningAlgorithm {

	private Knowledgebase knowledgebase;

	@SuppressWarnings("unused")
	private AlgorithmAngluin() {
	}

	public AlgorithmAngluin(int alphabet_size, Knowledgebase knowledgebase) {
		this.knowledgebase = knowledgebase;
		this.pointer = init(alphabet_size, knowledgebase.getPointer());
	}

	native int init(int alphabet_size, int knowledgebase_pointer);

	public Knowledgebase getKnowledgebase() {
		return knowledgebase;
	}

	@Override
	public void add_counterexample(int[] counterexample) {
		add_counterexample(counterexample, this.pointer);
	}

	private native void add_counterexample(int[] counterexample, int pointer);

	@Override
	public BasicAutomaton advance() {
		return advance(this.pointer);
	}

	private native BasicAutomaton advance(int pointer);

	@Override
	public boolean conjecture_ready() {
		return conjecture_ready(this.pointer);
	}

	private native boolean conjecture_ready(int pointer);

	@Override
	public int get_alphabet_size() {
		return get_alphabet_size(this.pointer);
	}

	private native int get_alphabet_size(int pointer);

	@Override
	public void increase_alphabet_size(int newSize) {
		increase_alphabet_size(newSize, this.pointer);

	}

	private native void increase_alphabet_size(int newSize, int pointer);

	@Override
	public void set_alphabet_size(int alphabetSize) {
		set_alphabet_size(alphabetSize, this.pointer);

	}

	private native void set_alphabet_size(int alphabetSize, int pointer);

	@Override
	public void set_knowledge_source(Knowledgebase base) {
		if (base == null)
			set_knowledge_source_NULL(this.pointer);
		else
			set_knowledge_source(base.getPointer(), this.pointer);
	}

	private native void set_knowledge_source(int knowledgebase_pointer,
			int pointer);

	private native void set_knowledge_source_NULL(int pointer);

	public boolean sync_to_knowledgebase() {
		return sync_to_knowledgebase(this.pointer);
	}

	private native boolean sync_to_knowledgebase(int pointer);

	public boolean supports_sync() {
		return supports_sync(this.pointer);
	}

	private native boolean supports_sync(int pointer);
}