package de.libalf.junit;

import de.libalf.LibALFFactory.Algorithm;
import de.libalf.dispatcher.DispatcherAlgorithmBiermannOriginal;
import de.libalf.dispatcher.DispatcherFactory;
import de.libalf.dispatcher.DispatcherKnowledgebase;
import de.libalf.dispatcher.DispatcherLearningAlgorithm;

public class Test {

	public static void main(String[] args) {
	
		DispatcherFactory f = new DispatcherFactory("seneca", 24940);
		DispatcherKnowledgebase base = f.createKnowledgebase();
		DispatcherLearningAlgorithm alg = f.createLearningAlgorithm(Algorithm.BIERMANN_ORIGINAL, base, 2, 2);	
	}
}
