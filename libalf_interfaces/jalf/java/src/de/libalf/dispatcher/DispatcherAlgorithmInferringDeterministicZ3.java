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
 *
 */

package de.libalf.dispatcher;

import de.libalf.Knowledgebase;
import de.libalf.Logger;

public class DispatcherAlgorithmInferringDeterministicZ3 extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmInferringDeterministicZ3(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger) {
		super(factory, DispatcherConstants.ALG_INFERRING_Z3, base, alphabet_size, logger);
	}
	
	public DispatcherAlgorithmInferringDeterministicZ3(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger, boolean use_variables, boolean use_enum) {
		super(factory, DispatcherConstants.ALG_INFERRING_Z3, base, alphabet_size, logger);
		
		set_using_variables(use_variables);
		set_using_enum(use_enum);
	}
	
	// Commmnd 0
	public boolean using_variables() {
		int[] magic = deserialize_magic(new int[]{0});
		if (magic.length != 1)
			throw new DispatcherProtocolException("unexpected answer length");
		return magic[0];
	}
	
	// Commmnd 1
	public void set_using_variables(boolean use_variables) {
		int[] data = new int[2];
		data[0] = 1;
		data[1] = use_variables ? 1 : 0;
		
		int[] magic = deserialize_magic(data);
	}

	// Commmnd 2
	public boolean using_enum() {
		int[] magic = deserialize_magic(new int[]{2});
		if (magic.length != 1)
			throw new DispatcherProtocolException("unexpected answer length");
		return magic[0];
	}
	
	// Commmnd 3
	public void set_using_enum(boolean use_enum) {
		int[] data = new int[2];
		data[0] = 3;
		data[1] = use_enum ? 1 : 0;
		
		int[] magic = deserialize_magic(data);
	}
}
