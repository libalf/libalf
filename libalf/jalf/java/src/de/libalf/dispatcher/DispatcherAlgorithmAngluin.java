package de.libalf.dispatcher;

public class DispatcherAlgorithmAngluin extends DispatcherLearningAlgorithm {
	public DispatcherAlgorithmAngluin(DispatcherFactory factory, DispatcherKnowledgebase base, int alphabet_size) {
		super(factory, DispatcherConstants.ALG_ANGLUIN, alphabet_size, base);
	}
}
