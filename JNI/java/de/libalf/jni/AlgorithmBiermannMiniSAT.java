package de.libalf.jni;

public class AlgorithmBiermannMiniSAT extends JNIAlgorithm {

	public AlgorithmBiermannMiniSAT(Knowledgebase knowledgebase, int alphabet_size) {
		this.knowledgebase = knowledgebase;
		this.pointer = init(alphabet_size, knowledgebase.getPointer());
	}

	native int init(int knowledgebase_pointer, int alphabet_size);
}
