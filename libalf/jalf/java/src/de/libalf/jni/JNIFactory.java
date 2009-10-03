package de.libalf.jni;

import de.libalf.AlfException;
import de.libalf.LibALFFactory;
import de.libalf.Logger;
import de.libalf.Normalizer;

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
	public JNIKnowledgebase createKnowledgebase(Object... args) {
		return new JNIKnowledgebase();
	}

	@Override
	public JNILearningAlgorithm createLearningAlgorithm(Algorithm algorithm,
			Object... args) {

		switch (algorithm) {

		/*
		 * Create Angluin learning algorithm.
		 */
		case ANGLUIN:
			if (args.length == 2)
				return new JNIAlgorithmAngluin((JNIKnowledgebase) args[0],
						(Integer) args[1]);
			else if (args.length == 3)
				return new JNIAlgorithmAngluin((JNIKnowledgebase) args[0],
						(Integer) args[1], (JNIBufferedLogger) args[2]);
			else
				throw new AlfException(
						"Invalid parameters for creating Angluin learning algorithm: "
								+ args.length + ".");

			/*
			 * Create Angluin (column) learning algorithm.
			 */
		case ANGLUIN_COLUMN:
			if (args.length == 2)
				return new JNIAlgorithmAngluinColumn(
						(JNIKnowledgebase) args[0], (Integer) args[1]);
			else if (args.length == 3)
				return new JNIAlgorithmAngluinColumn(
						(JNIKnowledgebase) args[0], (Integer) args[1],
						(JNIBufferedLogger) args[2]);
			else
				throw new AlfException(
						"Invalid parameters for creating Angluin (column) learning algorithm: "
								+ args.length + ".");

			/*
			 * Create NL^* learning algorithm.
			 */
		case NL_STAR:
			if (args.length == 2)
				return new JNIAlgorithmNLstar((JNIKnowledgebase) args[0],
						(Integer) args[1]);
			else if (args.length == 3)
				return new JNIAlgorithmNLstar((JNIKnowledgebase) args[0],
						(Integer) args[1], (JNIBufferedLogger) args[2]);
			else
				throw new AlfException(
						"Invalid parameters for creating NL^* learning algorithm: "
								+ args.length + ".");

			/*
			 * Create RPNI learning algorithm.
			 */
		case RPNI:
			if (args.length == 2)
				return new JNIAlgorithmRPNI((JNIKnowledgebase) args[0],
						(Integer) args[1]);
			else if (args.length == 3)
				return new JNIAlgorithmRPNI((JNIKnowledgebase) args[0],
						(Integer) args[1], (JNIBufferedLogger) args[2]);
			else
				throw new AlfException(
						"Invalid parameters for creating RPNI learning algorithm: "
								+ args.length + ".");

			/*
			 * Create Biermann (MiniSAT) learning algorithm.
			 */
		case BIERMANN_MINISAT:
			if (args.length == 2)
				return new JNIAlgorithmBiermannMiniSAT(
						(JNIKnowledgebase) args[0], (Integer) args[1]);
			else if (args.length == 3)
				return new JNIAlgorithmBiermannMiniSAT(
						(JNIKnowledgebase) args[0], (Integer) args[1],
						(JNIBufferedLogger) args[2]);
			else
				throw new AlfException(
						"Invalid parameters for creating Biermann (MiniSAT) learning algorithm: "
								+ args.length + ".");

			/*
			 * Default switch: Should never happen.
			 */
		default:
			throw new AlfException("Cannot create algorithm " + algorithm
					+ ", since there is no JNI support so far.");
		}
	}

	@Override
	public JNINormalizer createNormalizer(Normalizer.Type normType,
			Object... args) {
		switch (normType) {

		/*
		 * MSC normalize
		 */
		case MSC:
			return new JNINormalizer();

			/*
			 * Default switch: Should never happen.
			 */
		default:
			throw new AlfException("Cannot create normalizer " + normType
					+ ", since there is no JNI support so far.");
		}
	}

	@Override
	public Logger createLogger(Object... args) {
		return new JNIBufferedLogger();
	}

	@Override
	public void destroy() {
		/*
		 * A JNI factory need not to be destroyed since it is an ordinary Java
		 * object.
		 */
	}

	@Override
	public boolean isDestroyed() {
		/*
		 * A JNI factory need not to be destroyed since it is an ordinary Java
		 * object.
		 */
		return false;
	}
}
