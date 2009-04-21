package de.libalf.jni;

public class AlgorithmBiermannMiniSAT extends JNIAlgorithm {

	public AlgorithmBiermannMiniSAT(Knowledgebase knowledgebase, int alphabet_size) {
		this.knowledgebase = knowledgebase;
		this.pointer = init(knowledgebase.getPointer(), alphabet_size);
	}

	native long init(long knowledgebase_pointer, int alphabet_size);

	public AlgorithmBiermannMiniSAT(Knowledgebase knowledgebase, int alphabet_size, BufferedLogger logger) {
		this.knowledgebase = knowledgebase;
		this.logger = logger;
		this.pointer = init(knowledgebase.getPointer(), alphabet_size, logger.getPointer());
	}
	
	native long init(long knowledgebase_pointer, int alphabet_size, long logger_pointer);
}