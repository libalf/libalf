package de.libalf.dispatcher;

import de.libalf.Knowledgebase;
import de.libalf.Logger;

public class DispatcherAlgorithmAngluin extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmAngluin(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger) {
		super(factory, DispatcherConstants.ALG_ANGLUIN, base, alphabet_size, logger);
	}
}
