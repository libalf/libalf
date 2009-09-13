package de.libalf.dispatcher;

public class DispatcherAlgorithmAngluin extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmAngluin(DispatcherFactory factory, DispatcherKnowledgebase base, int alphabet_size) {
		super(factory, DispatcherConstants.ALG_ANGLUIN, alphabet_size, base);
	}
}
