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
 * Author: Stefan Schulz
 *
 */

package de.libalf.dispatcher;

import de.libalf.Knowledgebase;
import de.libalf.Logger;

public class DispatcherAlgorithmBiermannOriginal extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;
	
	public DispatcherAlgorithmBiermannOriginal(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger, int nondeterminism) {
		super(factory, DispatcherConstants.ALG_BIERMANN_ORIGINAL, base, alphabet_size, logger);
		
		if (deserialize_magic(new int[]{1, nondeterminism}).length != 1)
			throw new DispatcherProtocolException("unexpected answer length");
	}

	public int get_nondeterminism() {
		int[] magic = deserialize_magic(new int[]{0});
		if (magic.length != 1)
			throw new DispatcherProtocolException("unexpected answer length");
		return magic[0];
	}
}
