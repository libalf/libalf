package de.libalf.dispatcher;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

/**
 * <p>
 * Gracıa and Oncina's regular positive, negative inference algorithm (RPNI) for
 * regular languages. See
 * <ul>
 * <li>
 * <em>P. Gracıa and J. Oncina: Inferring regular languages in polynomial update time</em>
 * </li>
 * </ul>
 * </p>
 * <p>
 * Gracıa and Oncina's regular positive, negative inference algorithm is an
 * <em>offline learning algorithm</em>. See {@link AlgorithmBiermannMiniSAT} for
 * an example how to use offline learning algorithms in LibALF.
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 * 
 */
public class AlgorithmRPNI extends JNIAlgorithm {
	private static final long serialVersionUID = 2L;

	/**
	 * Creates a new object implementing the RPNI algorithm.
	 * 
	 * @param knowledgebase
	 *            the source of information for the algorithm.
	 * @param alphabet_size
	 *            the size of the used alphabet
	 */
	public AlgorithmRPNI(Knowledgebase knowledgebase, int alphabet_size) {
		this.knowledgebase = knowledgebase;
		this.id = init(knowledgebase.getID(), alphabet_size);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++ <code>RPNI</code>
	 * object with the pointer to a <code>knowledgebase</code> and the size of
	 * the alphabet. The pointer to the new created C++ object is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer, int alphabet_size);

	/**
	 * Creates a new object implementing the RPNI algorithm.
	 * 
	 * @param knowledgebase
	 *            the source of information for the algorithm.
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger
	 *            a logger
	 */
	public AlgorithmRPNI(Knowledgebase knowledgebase, int alphabet_size,
			BufferedLogger logger) {
		this.knowledgebase = knowledgebase;
		this.logger = logger;
		this.id = init(knowledgebase.getID(), alphabet_size, logger
				.getID());
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++ <code>RPNI</code>
	 * object with the pointer to a <code>knowledgebase</code>, the size of the
	 * alphabet and a pointer to a <code>buffered_logger</code>. The pointer to
	 * the new created C++ object is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger_pointer
	 *            a pointer to a buffered_logger C++ object
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer, int alphabet_size,
			long logger_pointer);

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		int alphabet_size = in.readInt();
		this.id = init(this.knowledgebase.getID(), alphabet_size, this.logger == null ? 0 : this.logger.getID());
		int[] serialization = (int[]) in.readObject();
		deserialize(serialization);
	}

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException {
		out.defaultWriteObject();
		out.writeInt(get_alphabet_size());
		out.writeObject(serialize());
    }
}
