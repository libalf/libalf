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

/**
 * Implements an abstract factory to create libalf objects. Each factory
 * implementing this interface has to provide methods to create concrete
 * impklementations of libalf objects.
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 0.1
 */
public interface LibALFFactory extends LibALFObject {
	static enum Algorithm {
		ANGLUIN, ANGLUIN_COLUMN, NL_STAR, RPNI, BIERMANN_MINISAT,
		BIERMANN_ORIGINAL, DELETE2, KEARNS_VAZIRANI;
	}

	/**
	 * Creates a new <code>Knowledgebase</code>.
	 * 
	 * @return a new <code>Knowledgebase</code>.
	 */
	abstract Knowledgebase createKnowledgebase(Object... args);


	abstract LearningAlgorithm createLearningAlgorithm(Algorithm algorithm, Object... args);

	/**
	 * Creates a new <code>Logger</code>.
	 * 
	 * @return a new <code>Logger</code>.
	 */
	abstract Logger createLogger(Object... args);

	/**
	 * Creates a new <code>Normalizer</code>.
	 * 
	 * @return a new <code>Normalizer</code>.
	 */
	abstract Normalizer createNormalizer(Normalizer.Type normType, Object... args);
}
