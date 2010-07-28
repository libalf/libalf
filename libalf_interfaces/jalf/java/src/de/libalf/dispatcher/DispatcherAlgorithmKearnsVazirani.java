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

public class DispatcherAlgorithmKearnsVazirani extends DispatcherLearningAlgorithm {
	private static final long serialVersionUID = 1L;

	public DispatcherAlgorithmKearnsVazirani(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger) {
		super(factory, DispatcherConstants.ALG_KEARNS_VAZIRANI, base, alphabet_size, logger);
	}
	
	public DispatcherAlgorithmKearnsVazirani(DispatcherFactory factory, Knowledgebase base, int alphabet_size, Logger logger, boolean use_binary_search) {
		super(factory, DispatcherConstants.ALG_KEARNS_VAZIRANI, base, alphabet_size, logger);
		
		set_binary_search(use_binary_search);
	}
	
	// Commmnd 0
	public int get_leaf_node_count() {
		int[] magic = deserialize_magic(new int[]{0});
		if (magic.length != 1)
			throw new DispatcherProtocolException("unexpected answer length");
		return magic[0];
	}
	
	// Commmnd 1
	public int get_inner_node_count() {
		int[] magic = deserialize_magic(new int[]{1});
		if (magic.length != 1)
			throw new DispatcherProtocolException("unexpected answer length");
		return magic[0];
	}
	
	// Commmnd 2
	public void set_binary_search(boolean use_binary_search) {
		int[] data = new int[2];
		data[0] = 2;
		data[1] = use_binary_search ? 1 : 0;
		
		int[] magic = deserialize_magic(data);
	}
	
	// Commmnd 3
	public boolean uses_binary_search() {
		int[] magic = deserialize_magic(new int[]{3});
		if (magic.length != 1)
			throw new DispatcherProtocolException("unexpected answer length");
		return magic[0] != 0;
	}
}
