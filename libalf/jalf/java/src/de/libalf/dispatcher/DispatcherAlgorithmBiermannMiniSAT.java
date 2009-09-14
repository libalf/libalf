package de.libalf.dispatcher;

import de.libalf.Knowledgebase;
import de.libalf.Logger;

public class DispatcherAlgorithmBiermannMiniSAT extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmBiermannMiniSAT(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger) {
		super(factory, DispatcherConstants.ALG_BIERMANN, base, alphabet_size, logger);
	}
}
