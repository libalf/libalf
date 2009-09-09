package de.libalf.dispatcher;

public class DispatcherAlgorithmNLstar extends DispatcherLearningAlgorithm {
	public DispatcherAlgorithmNLstar(DispatcherFactory factory, DispatcherKnowledgebase base, int alphabet_size) {
		super(factory, DispatcherConstants.ALG_NL_STAR, alphabet_size);
		set_knowledge_source(base);
	}
}
