package de.libalf.jni;

public class AlgorithmAngluin extends JNIAlgorithm {

	public AlgorithmAngluin(Knowledgebase knowledgebase, int alphabet_size) {
		this.knowledgebase = knowledgebase;
		this.pointer = init(knowledgebase.getPointer(), alphabet_size);
	}

	native long init(long knowledgebase_pointer, int alphabet_size);
}
