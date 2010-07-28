/*
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

package de.libalf.jni;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import de.libalf.Knowledgebase;
import de.libalf.Logger;

/**
 * <p>
 * Biermann and Feldman's original inference algorithm for regular languages. See
 * <ul>
 * <li>
 * <em>A.W. Biermann and J.A. Feldman - On the Synthesis of Finite-State Machines from
 * Samples of their Behavior</em></li>
 * </ul>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 0.1
 * 
 */
public class JNIAlgorithmBiermannOriginal extends JNILearningAlgorithmOffline {
	private static final long serialVersionUID = 1L;

	/**
	 * Creates a new object implementing Biermann and Feldman's original
	 * inference algorithm for regular languages.
	 * 
	 * @param knowledgebase
	 *            the source of information for the algorithm.
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param nondeterminism
	 *            the amount of nondeterminism (the parameter k)
	 */
	public JNIAlgorithmBiermannOriginal(Knowledgebase knowledgebase,
			int alphabet_size, int nondeterminism) {
		this.knowledgebase = (JNIKnowledgebase) knowledgebase;
		this.pointer = init(this.knowledgebase.getPointer(), alphabet_size, nondeterminism);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>original_biermann</code> object with the pointer to a
	 * <code>knowledgebase</code> and the size of the alphabet. The pointer to
	 * the new created C++ object is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param nondeterminism
	 *            the amount of nondeterminism (the parameter k)
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer, int alphabet_size, int nondeterminism);

	/**
	 * Creates a new object implementing Biermann and Feldman's original
	 * inference algorithm for regular languages.
	 * 
	 * @param knowledgebase
	 *            the source of information for the algorithm.
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param nondeterminism
	 *            the amount of nondeterminism (the parameter k)
	 * @param logger
	 *            a logger
	 */
	public JNIAlgorithmBiermannOriginal(Knowledgebase knowledgebase,
			int alphabet_size, Logger logger, int nondeterminism) {
		this.knowledgebase = (JNIKnowledgebase) knowledgebase;
		this.logger = (JNIBufferedLogger) logger;
		this.pointer = init(this.knowledgebase.getPointer(), alphabet_size,
				this.logger.getPointer(), nondeterminism);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>original_biermann</code> object with the pointer to a
	 * <code>knowledgebase</code>, the size of the alphabet and a pointer to a
	 * <code>buffered_logger</code>. The pointer to the new created C++ object
	 * is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger_pointer
	 *            a pointer to a buffered_logger C++ object
	 * @param nondeterminism
	 *            the amount of nondeterminism (the parameter k)
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer, int alphabet_size,
			long logger_pointer, int nondeterminism);

			
	public int get_nondeterminism() {
		return get_nondeterminism(this.pointer);
	}
	
	private native int get_nondeterminism(long pointer);
			
	/**
	 * Performs no action and just returns <code>null</code>.
	 * 
	 * @param data
	 *            some data.
	 * 
	 * @return <code>null</code>.
	 */
	public int[] deserialize_magic(int[] data) {
		return null;
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException,
			ClassNotFoundException {
		in.defaultReadObject();
		int alphabet_size = in.readInt();
		int nondeterminism = in.readInt();
		if (this.logger != null)
			this.pointer = init(this.knowledgebase.getPointer(), alphabet_size,
					this.logger.getPointer(), nondeterminism);
		else
			this.pointer = init(this.knowledgebase.getPointer(), alphabet_size,
					nondeterminism);
		int[] serialization = (int[]) in.readObject();
		deserialize(serialization);
	}

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException {
		out.defaultWriteObject();
		out.writeInt(get_alphabet_size());
		out.writeInt(get_nondeterminism());
		out.writeObject(serialize());
	}
}
