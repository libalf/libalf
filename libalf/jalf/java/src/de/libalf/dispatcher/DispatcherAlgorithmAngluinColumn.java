package de.libalf.dispatcher;

public class DispatcherAlgorithmAngluinColumn extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmAngluinColumn(DispatcherFactory factory, DispatcherKnowledgebase base, int alphabet_size) {
		super(factory, DispatcherConstants.ALG_ANGLUIN_COLUMN, alphabet_size, base);
	}
}
