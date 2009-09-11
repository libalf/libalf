package de.libalf.dispatcher;

public class DispatcherAlgorithmRPNI extends DispatcherLearningAlgorithm {
	public DispatcherAlgorithmRPNI(DispatcherFactory factory, DispatcherKnowledgebase base, int alphabet_size) {
		super(factory, DispatcherConstants.ALG_RPNI, alphabet_size, base);
	}
}
