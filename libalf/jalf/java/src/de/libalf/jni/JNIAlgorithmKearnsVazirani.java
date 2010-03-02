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

/**
 * <p>
 * Kearns and Vazirani's tree based learning algorithm for regular languages. See
 * <ul>
 * <li>
 * <em>D. Angluin - Learning regular sets from queries and counterexamples</em></li>
 * </ul>
 * </p>
 * <p>
 * By default, a binary search is performed to analyze counter-examples. If desired,
 * you can use an alternative method (a linear search) for this. However, note that
 * the runtime of a linear search is worse.
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 0.1
 * 
 */
public class JNIAlgorithmKearnsVazirani extends JNILearningAlgorithm {
	private static final long serialVersionUID = 2L;

	/**
	 * Creates a new object implementing Kearns and Vazirani's learning algorithm.
	 * 
	 * @param knowledgebase
	 *            the source of information for the algorithm.
	 * @param alphabet_size
	 *            the size of the used alphabet
	 */
	public JNIAlgorithmKearnsVazirani(Knowledgebase knowledgebase, int alphabet_size) {
		this.knowledgebase = (JNIKnowledgebase) knowledgebase;
		this.pointer = init(this.knowledgebase.getPointer(), alphabet_size);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>kearns_vazirani</code> object with the pointer to a
	 * <code>knowledgebase</code> and the size of the alphabet. The pointer to
	 * the new created C++ object is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer, int alphabet_size);

	/**
	 * Creates a new object implementing Kearns and Vazirani's learning algorithm.
	 * 
	 * @param knowledgebase
	 *            the source of information for the algorithm.
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger
	 *            a logger
	 */
	public JNIAlgorithmKearnsVazirani(Knowledgebase knowledgebase, int alphabet_size,
			Logger logger) {
		this.knowledgebase = (JNIKnowledgebase) knowledgebase;
		this.logger = (JNIBufferedLogger) logger;
		this.pointer = init(this.knowledgebase.getPointer(), alphabet_size,
				this.logger.getPointer());
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>kearns_vazirani</code> object with the pointer to a
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
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer, int alphabet_size,
			long logger_pointer);

	/**
	 * Creates a new object implementing Kearns and Vazirani's learning algorithm.
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
	public JNIAlgorithmKearnsVazirani(Knowledgebase knowledgebase, int alphabet_size,
			Logger logger, boolean use_binary_search) {
		this.knowledgebase = (JNIKnowledgebase) knowledgebase;
		this.logger = (JNIBufferedLogger) logger;
		this.pointer = init(this.knowledgebase.getPointer(), alphabet_size,
				this.logger.getPointer(), use_binary_search);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++
	 * <code>kearns_vazirani</code> object with the pointer to a
	 * <code>knowledgebase</code>, the size of the alphabet, a pointer to a
	 * <code>buffered_logger</code>, and whether to use a binary search to analyze
	 * counter-examples. The pointer to the new created C++ object is returned.
	 * 
	 * @param knowledgebase_pointer
	 *            a pointer to a knowledgebase C++ object
	 * @param alphabet_size
	 *            the size of the used alphabet
	 * @param logger_pointer
	 *            a pointer to a buffered_logger C++ object
	 * @param use_binary_search
	 *            decides whether to use a binary search <code>true</code>) or
	 *            a linear search (<code>false</code>) when analyzing counter-
	 *            examples.
	 * @return a pointer to the memory location of the new C++ object.
	 */
	native long init(long knowledgebase_pointer, int alphabet_size,
			long logger_pointer, boolean use_binary_search);
			
	/**
	 * <p>
	 * Returns the number of inner nodes of the internal data tree.
	 * </p>
	 *
	 * @return the number of inner nodes.
	 */
	public int get_inner_node_count() {
		return get_inner_node_count(pointer);
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithmKearnsVazirani#get_inner_node_count()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int get_inner_node_count(long pointer);
	
	/**
	 * <p>
	 * Returns the number of leaf nodes (which equals the number of states of
	 * current conjecture) of the internal data tree.
	 * </p>
	 *
	 * @return the number of leaf nodes.
	 */
	public int get_leaf_node_count() {
		return get_leaf_node_count(pointer);
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithmKearnsVazirani#get_leaf_node_count()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native int get_leaf_node_count(long pointer);
	
	/**
	 * <p>
	 * Returns whether a binary search or a linear search is used to find a bad
	 * prefix of a counter-example.
	 * </p>
	 *
	 * @return <code>true</code> if a binary search is used to process the counter-
	 *         examples and <code>false</code> otherwise.
	 */
	public boolean uses_binary_search() {
		return uses_binary_search(pointer);
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithmKearnsVazirani#uses_binary_search()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean uses_binary_search(long pointer);
	
	
	/**
	 * <p>
	 * Sets the method of analyzing counter-examples: If set to <code>true</code>,
	 * a binary search is performed. Otherwise (if set to <code>false</code>),
	 * then a linear search is performed.
	 * </p>
	 *
	 * @param 
	 */
	public void set_binary_search(boolean use_binary_search) {
		set_binary_search(pointer, use_binary_search);
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See
	 * {@link JNIAlgorithmKearnsVazirani#set_binary_search(boolean)}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native void set_binary_search(long pointer, boolean use_binary_search);	
	
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
			this.pointer = init(this.knowledgebase.getPointer(), alphabet_size,
					this.logger.getPointer());
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
