package de.libalf.jni;

import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.LibALFFactory;
import de.libalf.Logger;
import de.libalf.AlfException;

/**
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 */
public class JNIFactory implements LibALFFactory {
	private static final long serialVersionUID = 1L;

	@Override
	public Knowledgebase createKnowledgebase(Object... args) {
		return new JNIKnowledgebase();
	}

	@Override
	public LearningAlgorithm createLearningAlgorithm(Algorithm algorithm,
			Object... args) {
		
		switch (algorithm) {
		
		/*
		 * Create Angluin learning algorithm.
		 */
		case ANGLUIN:
			if(args.length == 2)
			return new JNIAlgorithmAngluin((JNIKnowledgebase) args[0],
					(Integer) args[1]);
			else if(args.length == 3)
			return new JNIAlgorithmAngluin((JNIKnowledgebase) args[0],
					(Integer) args[1], (JNIBufferedLogger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating Angluin learning algorithm: " + args.length + ".");
		
		/*
		 * Create Angluin (column) learning algorithm.
		 */
		case ANGLUIN_COLUMN:
			if(args.length == 2)
			return new JNIAlgorithmAngluinColumn((JNIKnowledgebase) args[0],
					(Integer) args[1]);
			else if(args.length == 3)
			return new JNIAlgorithmAngluinColumn((JNIKnowledgebase) args[0],
					(Integer) args[1], (JNIBufferedLogger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating Angluin (column) learning algorithm: " + args.length + ".");
	
		/*
		 * Create NL^* learning algorithm.
		 */
		case NL_STAR:
			if(args.length == 2)
				return new JNIAlgorithmNLstar((JNIKnowledgebase) args[0],
					(Integer) args[1]);
			else if(args.length == 3)
				return new JNIAlgorithmNLstar((JNIKnowledgebase) args[0],
					(Integer) args[1], (JNIBufferedLogger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating NL^* learning algorithm: " + args.length + ".");
		
		/*
		 * Create RPNI learning algorithm.
		 */
		case RPNI:
			if(args.length == 2)
				return new JNIAlgorithmRPNI((JNIKnowledgebase) args[0],
					(Integer) args[1]);
			else if(args.length == 3)
				return new JNIAlgorithmRPNI((JNIKnowledgebase) args[0],
					(Integer) args[1], (JNIBufferedLogger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating RPNI learning algorithm: " + args.length + ".");
		
		/*
		 * Create Biermann (MiniSAT) learning algorithm.
		 */		
		case BIERMANN_MINISAT:
			if(args.length == 2)
				return new JNIAlgorithmBiermannMiniSAT(
					(JNIKnowledgebase) args[0], (Integer) args[1]);
			else if(args.length == 3)
				return new JNIAlgorithmBiermannMiniSAT(
					(JNIKnowledgebase) args[0], (Integer) args[1],
					(JNIBufferedLogger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating Biermann (MiniSAT) learning algorithm: " + args.length + ".");
				
		/*
		 * Default switch: Should never happen.
		 */
		default:
			return null;
		}
	}

	@Override
	public Logger createLogger(Object... args) {
		return new JNIBufferedLogger();
	}

	@Override
	public void destroy() {
		// indestructible
	}
	
	@Override
	public boolean isDestroyed() {
		// indestructible
		return false;
	}
}
