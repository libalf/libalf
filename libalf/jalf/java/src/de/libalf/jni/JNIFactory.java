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

import de.libalf.AlfException;
import de.libalf.LibALFFactory;
import de.libalf.Logger;
import de.libalf.Normalizer;

/**
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 0.1
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
			 * Create RPNI inference algorithm.
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
						"Invalid parameters for creating RPNI inference algorithm: "
								+ args.length + ".");

			/*
			 * Create Biermann (MiniSAT) inference algorithm.
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
						"Invalid parameters for creating Biermann (MiniSAT) inference algorithm: "
								+ args.length + ".");

			/*
			 * Create DeLeTe2 inference algorithm.
			 */
		case DELETE2:
			if (args.length == 2)
				return new JNIAlgorithmDeLeTe2((JNIKnowledgebase) args[0],
						(Integer) args[1]);
			else if (args.length == 3)
				return new JNIAlgorithmDeLeTe2((JNIKnowledgebase) args[0],
						(Integer) args[1], (JNIBufferedLogger) args[2]);
			else
				throw new AlfException(
						"Invalid parameters for creating DeLeTe2 inference algorithm: "
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
		 * MSC normalizer
		 */
		case MSC:
			if(args.length == 0)
				return new JNINormalizer();
			else {
				throw new AlfException(
					"Invalid parameters for creating normalizer: " + args.length + ".");
			}
			
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
