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
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
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


public class JNIAlgorithmCounterexampleLearning extends JNILearningAlgorithm {
	private static final long serialVersionUID = 2L;

	long inferring_algorithm;

	/**
	 * Creates a new object implementing learning algorithm.
	 * 
	 * @param knowledgebase
	 *            the source of information for the algorithm.
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger
	 *            a logger
	 * @param ptr_inferring_algorithm
	 *            a pointer to an inferring algorithm
	 */
	public JNIAlgorithmCounterexampleLearning(Knowledgebase knowledgebase, 
			Logger logger, int alphabet_size, long ptr_inferring_algorithm) {
		this.knowledgebase = (JNIKnowledgebase) knowledgebase;
		this.inferring_algorithm = ptr_inferring_algorithm;
		if(logger != null) {
			this.logger = (JNIBufferedLogger) logger;
			this.pointer = init(this.knowledgebase.getPointer(), this.logger.getPointer(), alphabet_size,
				ptr_inferring_algorithm);
		} else {
			this.logger = null;
			this.pointer = init(this.knowledgebase.getPointer(), alphabet_size,
				ptr_inferring_algorithm);
		}
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>Counterexample Learning</code> object with the pointer to a
	 * <code>knowledgebase</code>, a pointer to a
	 * <code>buffered_logger</code> and the size of the alphabet. The pointer to the new created C++ object
	 * is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger_pointer
	 *            a pointer to a buffered_logger C++ object
	 * @param inferring_algorithm_pointer
	 *            a pointer to an inferring algorithm C++ object
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer,
			long logger_pointer,  int alphabet_size, long inferring_algorithm_pointer);

	native long init(long knowledgebase_pointer, int alphabet_size, long inferring_algorithm_pointer);

	/**
	 * <p>
	 * Derives a conjecture and returns pointer to it.
	 * </p>
	 *
	 * @return <code>true</code> Pointer to conjecture.
	 */
	public long derive_conjecture() {
		return derive_conjecture(pointer);
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithmCounterexampleLearning#derive_conjecture()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native long derive_conjecture(long pointer);

	/**
	 * Performs no action and just returns <code>null</code>.
	 *
	 * @param data some data.
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
		long inferring_algorithm_pointer = in.readLong();
		if (this.logger != null)
			this.pointer = init(this.knowledgebase.getPointer(),
					this.logger.getPointer(), alphabet_size, inferring_algorithm_pointer);
		else
			this.pointer = init(this.knowledgebase.getPointer(), alphabet_size, inferring_algorithm_pointer);
		int[] serialization = (int[]) in.readObject();
		deserialize(serialization);
	}

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException {
		out.defaultWriteObject();
		out.writeInt(get_alphabet_size());
		out.writeLong(inferring_algorithm);
		out.writeObject(serialize());
	}
}
