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


public class JNIAlgorithmDeterministicInferringCSPMinisat extends JNILearningAlgorithm {
	private static final long serialVersionUID = 2L;

	/**
	 * Creates a new object implementing inferring learning algorithm.
	 * 
	 * @param knowledgebase
	 *            the source of information for the algorithm.
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger
	 *            a logger
	 */
	public JNIAlgorithmDeterministicInferringCSPMinisat(Knowledgebase knowledgebase, 
			Logger logger, int alphabet_size) {
		this.knowledgebase = (JNIKnowledgebase) knowledgebase;
		this.logger = (JNIBufferedLogger) logger;
		this.pointer = init(this.knowledgebase.getPointer(),
				this.logger.getPointer(), alphabet_size);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>Deterministic Inferring CSP Minisat</code> object with the pointer to a
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
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer,
			long logger_pointer,  int alphabet_size);

	native long init(long knowledgebase_pointer, int alphabet_size);

	/**
	 * Creates a new object implementing inferring learning algorithm.
	 * 
	 * @param knowledgebase
	 *            the source of information for the algorithm.
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger
	 *            a logger
	 * @param use_binary_search
	 *            decides whether to use a binary search <code>true</code>) or
	 *            a linear search (<code>false</code>) when processing counter-
	 *            examples.
	 */
	public JNIAlgorithmDeterministicInferringCSPMinisat(Knowledgebase knowledgebase,
			Logger logger, int alphabet_size, boolean use_binary_search) {
		this.knowledgebase = (JNIKnowledgebase) knowledgebase;
		if(logger != null) {
			this.logger = (JNIBufferedLogger) logger;
			this.pointer = init(this.knowledgebase.getPointer(), this.logger.getPointer(), alphabet_size,
				use_binary_search);
		} else {
			this.logger = null;
			this.pointer = init(this.knowledgebase.getPointer(), alphabet_size,
				use_binary_search);
		}
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>Deterministic Inferring CSP Minisat</code> object with the pointer to a
	 * <code>knowledgebase</code>, a pointer to a
	 * <code>buffered_logger</code>, the size of the alphabet, and whether to use a binary search to analyze
	 * counter-examples. The pointer to the new created C++ object is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param logger_pointer
	 *            a pointer to a buffered_logger C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param use_binary_search
	 *            decides whether to use a binary search <code>true</code>) or
	 *            a linear search (<code>false</code>) when analyzing counter-
	 *            examples.
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer, long logger_pointer, int alphabet_size,
			boolean use_binary_search);

	native long init(long knowledgebase_pointer, int alphabet_size,
			boolean use_binary_search);
	
	/**
	 * <p>
	 * Returns whether unary encoding is used.
	 * </p>
	 *
	 * @return <code>true</code> if unary encoded and <code>false</code> otherwise.
	 */
	public boolean uses_unary_encoding() {
		return uses_unary_encoding(pointer);
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithmDeterministicInferringCSPMinisat#uses_binary_search()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean uses_unary_encoding(long pointer);

	/**
	 * <p>
	 * Sets unary encoding.
	 * </p>
	 *
	 * @return <code>true</code> enables unary encoding and <code>false</code> disables it.
	 */
	public void set_unary_encoding(boolean unary_encoding) {
		set_unary_encoding(pointer, unary_encoding);
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithmDeterministicInferringCSPMinisat#uses_binary_search()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native void set_unary_encoding(long pointer, boolean unary_encoding);

	/**
	 * <p>
	 * Returns whether algorithm is logging
	 * </p>
	 *
	 * @return <code>true</code> if algorithm is logging and <code>false</code> otherwise.
	 */
	public boolean is_logging_model() {
		return is_logging_model(pointer);
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithmDeterministicInferringCSPMinisat#is_logging_model()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean is_logging_model(long pointer);
	
	
	/**
	 * <p>
	 * Enables of disables logging: If set to <code>true</code>,
	 * logging is enabled. Otherwise (if set to <code>false</code>),
	 * then logging is disabled.
	 * </p>
	 *
	 * @param 
	 */
	public void set_log_model(boolean log_model) {
		set_log_model(pointer, log_model);
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithmDeterministicInferringCSPMinisat#set_log_model(boolean)}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native void set_log_model(long pointer, boolean log_model);	
	
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
		if (this.logger != null)
			this.pointer = init(this.knowledgebase.getPointer(),
					this.logger.getPointer(), alphabet_size);
		else
			this.pointer = init(this.knowledgebase.getPointer(), alphabet_size);
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
