package de.libalf.dispatcher;

import de.libalf.Knowledgebase;
import de.libalf.Logger;

public class DispatcherAlgorithmDeLeTe2 extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmDeLeTe2(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger) {
		super(factory, DispatcherConstants.ALG_DELETE2, base, alphabet_size, logger);
	}
}
