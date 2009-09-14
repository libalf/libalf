package de.libalf.dispatcher;

import de.libalf.Knowledgebase;
import de.libalf.Logger;

public class DispatcherAlgorithmNLstar extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmNLstar(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger) {
		super(factory, DispatcherConstants.ALG_NL_STAR, base, alphabet_size, logger);
	}
}
