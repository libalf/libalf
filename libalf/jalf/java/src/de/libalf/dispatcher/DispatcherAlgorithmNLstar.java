package de.libalf.dispatcher;

public class DispatcherAlgorithmNLstar extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmNLstar(DispatcherFactory factory, DispatcherKnowledgebase base, int alphabet_size) {
		super(factory, DispatcherConstants.ALG_NL_STAR, alphabet_size, base);
	}
}
