package de.libalf;

/**
 * Implements an abstract factory to create libalf objects. Each factory
 * implementing this interface has to provide methods to create concrete
 * impklementations of libalf objects.
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 */
public interface LibALFFactory extends LibALFObject {

	public static enum Algorithm {
		ANGLUIN, ANGLUIN_COLUMN, NL_STAR, RPNI, BIERMANN_MINISAT;
	}

	/**
	 * Creates a new <code>Knowledgebase</code>.
	 * 
	 * @return a new <code>Knowledgebase</code>.
	 */
	public abstract Knowledgebase createKnowledgebase(Object... args);


	public abstract LearningAlgorithm createLearningAlgorithm(Algorithm algorithm, Object... args);

	/**
	 * Creates a new <code>Logger</code>.
	 * 
	 * @return a new <code>Logger</code>.
	 */
	public abstract Logger createLogger(Object... args);

	/**
	 * Creates a new <code>Normalizer</code>.
	 * 
	 * @return a new <code>Normalizer</code>.
	 */
	public abstract Normalizer createNormalizer(Normalizer.Type normType, Object... args);

}
