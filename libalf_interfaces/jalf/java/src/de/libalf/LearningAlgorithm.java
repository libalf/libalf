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

package de.libalf;

import de.libalf.Knowledgebase.Acceptance;

/**
 * <p>
 * The root interface of all LibALF learning algorithms.
 * </p>
 * <p>
 * A class implementing the <code>LearningAlgorithm</code> interface provides
 * the code of the learning algorithm. Additionally, in LibALF a learning
 * algorithm is associated with:
 * <ul>
 * <li>An <em>alphabet</em> over which queries are conducted and conjectures are
 * built.</li>
 * <li>A <em>knowledge source</em>, given as a {@link Knowledgebase} object,
 * from which the learning algorithm obtains its information.</li>
 * </ul>
 * </p>
 * <p>
 * A <code>Knowledgebase</code> is used by both <em>offline</em> and
 * <em>online</em> learning algorithms. For offline learning algorithms it is
 * the only source of information. For online learning algorithms it acts as a
 * buffer between the learning algorithm and the teacher:
 * <ul>
 * <li>An learning algorithms requesting new information stores its queries in
 * the <code>Knowledgebase</code> and marks the words as
 * {@link Acceptance#UNKNOWN}.</li>
 * <li>The teacher obtains these queries by calling
 * {@link Knowledgebase#getQueries()} and can answer them one by one using
 * {@link Knowledgebase#add_knowledge(int[], boolean)}.
 * </ul>
 * </p>
 * <p>
 * The learning is invoked by calling {@link LearningAlgorithm#advance()}. For
 * offline learning algorithms, where no interaction is necessary, the
 * <code>advance</code> method returns a {@link BasicAutomaton} as result of the
 * inference.<br>
 * Online learning algorithms may stop because either queries have to be
 * answered (then <code>advance</code> returns <code>null</code>), or because a
 * conjecture is ready. In the latter case a {@link BasicAutomaton} is returned.
 * Note that {@link LearningAlgorithm#conjecture_ready()} can also be used to
 * check whether <code>advance</code> has provided a new conjecture.
 * </p>
 * <p>
 * Some learning algorithms support the undo of operations after they have been
 * synchronized to their associated <code>Knowledgebase</code>. If a learning
 * algorithm supports synchronization with a knowledge base, i.e.
 * {@link LearningAlgorithm#supports_sync()} returns <code>true</code>, it can
 * be synchronized by calling {@link LearningAlgorithm#sync_to_knowledgebase()}.
 * <br>
 * Undo operations can easily be performed by calling
 * {@link Knowledgebase#undo(int)}.
 * </p>
 * <p>
 * <b>Note:</b>
 * <ul>
 * <li>This is a Java implementation of the <em>learning_algorithm</em> C++
 * class.</li>
 * <li>
 * This JavaDoc is only a rough overview. For a detailed documentation please
 * refer to the original LibALF C++ documentation.</li>
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 0.1
 * 
 */
public interface LearningAlgorithm extends LibALFObject {

	/**
	 * Sets the size of the alphabet to a new value.
	 * 
	 * @param alphabet_size
	 *            the size of the alphabet
	 */
	public void set_alphabet_size(int alphabet_size);

	/**
	 * Returns the size of the alphabet.
	 * 
	 * @return the size of the alphabet.
	 */
	public int get_alphabet_size();

	/**
	 * Increases the alphabet's size to a new value.
	 * 
	 * @param new_size
	 *            the new size of the alphabet
	 */
	public void increase_alphabet_size(int new_size);

	/**
	 * Sets or replaces the knowledge source of the learning algorithm.
	 * 
	 * @param base
	 *            the knowledgebase to set or replace.
	 */
	public void set_knowledge_source(Knowledgebase base);

	/**
	 * Returns the current knowledgebase.
	 * 
	 * @return the current knowledgebase.
	 */
	public Knowledgebase get_knowledge_source();

	/**
	 * Checks whether the learning algorithm is able to provide a conjecture.
	 * 
	 * @return <code>true</code>, if the learning algorithm is able to provide a
	 *         conjecture or <code>false</code>, otherwise.
	 */
	public boolean conjecture_ready();

	/**
	 * Advances one step in the learning algorithm.
	 * 
	 * @return <ul>
	 *         <li>A conjecture if the learning algorithm is able to propose
	 *         one, or</li>
	 *         <li><code>null</code>, otherwise.</li>
	 *         </ul>
	 */
	public Conjecture advance();

	/**
	 * <p>
	 * Provides the learning algorithm with a new counter-example.<br>
	 * The algorithm will automatically query for the membership information of
	 * the word. Thus, the counter-example may be processed by the learning
	 * algorithm before it is added to the {@link Knowledgebase}
	 * </p>
	 * <p>
	 * <b>Note:</b> Adding counter-example is only possible in case of
	 * <em>online learning algorithms</em>. For
	 * <em>offline learning algorithms</em> this method is a stub.
	 * </p>
	 * 
	 * @param counterexample
	 *            the new counter-example to add
	 */
	public void add_counterexample(int[] counterexample);

	/**
	 * Synchronizes the learning algorithm with the associated knowledge base.
	 * This has to be done after each undo-operation in the corresponding
	 * {@link Knowledgebase} to remove obsolete information from the algorithm.
	 * 
	 * @return result of the synchronisation with the knowledgebase. If false is
	 *         returned, the algorithm is in an undefined state and should not
	 *         be used anymore.
	 */
	public boolean sync_to_knowledgebase();

	/**
	 * Checks whether the learning algorithm supports synchronization with the
	 * associated {@link Knowledgebase}. If so, undo-operations on the
	 * knowledgebase are allowed. Each undo-operation has to be followed by a
	 * call to sync_to_knowledgebase().
	 * 
	 * @return the result of the check.
	 */
	public boolean supports_sync();

	/**
	 * Serializes the state of the learning algorithm. This serialization can be
	 * saved and the learning algorithm can be restored using the
	 * {@link LearningAlgorithm#deserialize(int[])} method.<br>
	 * Be sure to also serialize the associated {@link Knowledgebase}.
	 * 
	 * @return an int array that stores the serialization of the learning
	 *         algorithm.
	 */
	public int[] serialize();

	/**
	 * Restores the data of an a priori serialized learning algorithm. The
	 * current state of the learning algorithms is discarded.
	 * 
	 * @param serialization
	 *            a serialization of a learning algorithm
	 * @return true, if the recovery was successful and false, otherwise.
	 */
	public boolean deserialize(int[] serialization);

	/**
	 * Performs some magic custom action (useful for easy self-made extensions).
	 * 
	 * @param data some data.
	 * @return some data.
	 */
	public int[] deserialize_magic(int[] data);

	/**
	 * Sets the logger for this learning algorithm.
	 * 
	 * @param logger
	 *            the new logger
	 */
	public void set_logger(Logger logger);

	public Logger get_logger();

	public void remove_logger() throws AlfException;

	/**
	 * Returns a textual representation of the learning algorithm.
	 * 
	 * @return a textual representation of the learning algorithm.
	 */
	public String toString();

	public void set_normalizer(Normalizer normalizer) throws AlfException;

	public Normalizer get_normalizer() throws AlfException;

	public void remove_normalizer() throws AlfException;
}
