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
	public LearningAlgorithm createLearningAlgorithm() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Logger createLogger() {
		// TODO Auto-generated method stub
		return null;
	}

}
