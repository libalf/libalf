package de.libalf.dispatcher;

public class DispatcherAlgorithmAngluinColumn extends DispatcherLearningAlgorithm {
	public DispatcherAlgorithmAngluinColumn(DispatcherFactory factory, DispatcherKnowledgebase base, int alphabet_size) {
		super(factory, DispatcherConstants.ALG_ANGLUIN_COLUMN, alphabet_size);
		set_knowledge_source(base);
	}
}
