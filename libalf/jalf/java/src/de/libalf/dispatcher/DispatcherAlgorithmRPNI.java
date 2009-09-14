package de.libalf.dispatcher;

import de.libalf.Knowledgebase;
import de.libalf.Logger;

public class DispatcherAlgorithmRPNI extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmRPNI(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger) {
		super(factory, DispatcherConstants.ALG_RPNI, base, alphabet_size, logger);
	}
}
