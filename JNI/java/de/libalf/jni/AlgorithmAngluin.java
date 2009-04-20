package de.libalf.jni;

public class AlgorithmAngluin extends JNIAlgorithm {

	public AlgorithmAngluin(Knowledgebase knowledgebase, int alphabet_size) {
		this.knowledgebase = knowledgebase;
		this.pointer = init(knowledgebase.getPointer(), alphabet_size);
		System.err.println("Ohne logger");
	}

	native long init(long knowledgebase_pointer, int alphabet_size);
	
	public AlgorithmAngluin(Knowledgebase knowledgebase, int alphabet_size, BufferedLogger logger) {
		this.knowledgebase = knowledgebase;
		this.logger = logger;
		this.pointer = init(knowledgebase.getPointer(), alphabet_size, logger.getPointer());
		System.err.println("Mit logger");
	}
	
	native long init(long knowledgebase_pointer, int alphabet_size, long logger);
}
