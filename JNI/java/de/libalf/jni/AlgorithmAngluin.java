package de.libalf.jni;

public class AlgorithmAngluin extends JNIAlgorithm {

	public AlgorithmAngluin(int alphabet_size, Knowledgebase knowledgebase) {
		this.knowledgebase = knowledgebase;
		this.pointer = init(alphabet_size, knowledgebase.getPointer());
	}

	native int init(int alphabet_size, int knowledgebase_pointer);
}
