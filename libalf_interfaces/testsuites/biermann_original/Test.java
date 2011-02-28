/* $Id$ */
/* test some functionality of dispatcher */

import de.libalf.LibALFFactory.Algorithm;
import de.libalf.dispatcher.DispatcherFactory;
import de.libalf.Logger;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;

public class Test {

	public static void main(String[] args) {
		// Get factory
		DispatcherFactory f = new DispatcherFactory("localhost", 24940);

		// Use factory to get our objects
		Logger log = f.createLogger();
		Knowledgebase base = f.createKnowledgebase();
		LearningAlgorithm alg = f.createLearningAlgorithm(Algorithm.BIERMANN_ORIGINAL, base, 2, 2);
		alg.set_logger(log);

		// Do something

		// Clean up and get logger content
		alg.remove_logger();
		alg = null;
		base = null;
		System.out.println("<<<< Final <<<<\nRemaining data from the logger:\n" + log.receive_and_flush() + "\n<<<< EOF <<<<\n");
		log = null;
		f = null;
	}
}

