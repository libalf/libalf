package de.libalf.jni;

import de.libalf.AlfException;

public abstract class JNILearningAlgorithmOffline extends JNILearningAlgorithm {
	private static final long serialVersionUID = 1L;

	@Override
	public void add_counterexample(int[] counterexample) {
		throw new AlfException("Offline learning does not support giving counterexamples");
	}
}
