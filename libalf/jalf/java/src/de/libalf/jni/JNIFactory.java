package de.libalf.jni;

import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.LibALFFactory;
import de.libalf.Logger;

public class JNIFactory implements LibALFFactory {

	@Override
	public Knowledgebase createKnowledgebase() {
		return new JNIKnowledgebase();
	}

	@Override
	public LearningAlgorithm createLearningAlgorithm(Algorithm algorithm,
			Object[] parameter) {
		switch (algorithm) {
		case ANGLUIN:
			return new JNIAlgorithmAngluin((JNIKnowledgebase) parameter[0],
					(Integer) parameter[1]);
		case ANGLUIN_COLUMN:
			return new JNIAlgorithmAngluinColumn(
					(JNIKnowledgebase) parameter[0], (Integer) parameter[1]);
		default:
			return null;
		}
	}

	@Override
	public Logger createLogger() {
		// TODO Auto-generated method stub
		return null;
	}

}
