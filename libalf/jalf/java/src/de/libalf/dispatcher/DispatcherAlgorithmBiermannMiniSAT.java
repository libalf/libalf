package de.libalf.dispatcher;

public class DispatcherAlgorithmBiermannMiniSAT extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmBiermannMiniSAT(DispatcherFactory factory, DispatcherKnowledgebase base, int alphabet_size) {
		super(factory, DispatcherConstants.ALG_BIERMANN, alphabet_size, base);
	}
}
