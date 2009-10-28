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

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import de.libalf.AlfException;
import de.libalf.Normalizer;

public class DispatcherNormalizer extends DispatcherObject implements Normalizer {
	private static final long serialVersionUID = 1L;

	public DispatcherNormalizer(DispatcherFactory factory, DispatcherConstants normType) throws AlfException {
		super(factory, DispatcherConstants.OBJ_NORMALIZER);
		create(normType.id);
	}

	private void create(int normType) {
		create(new int[] { normType });
	}

	@Override
	public boolean deserialize(int[] serialization) throws AlfException {
		synchronized (this.factory) {
			try {
				this.factory.writeObjectCommandThrowing(this, DispatcherConstants.NORMALIZER_DESERIALIZE, serialization);
				return true;
			} catch (DispatcherCommandError e) {
				return false;
			}
		}
	}

	@Override
	public int[] serialize() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.NORMALIZER_SERIALIZE);
			return this.factory.readInts();
		}
	}

	@Override
	public int get_type() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.NORMALIZER_GET_TYPE);
			return this.factory.readInt();
		}
	}

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException {
		out.defaultWriteObject();
		out.writeObject(get_type());
		out.writeObject(serialize());
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		create((Integer) in.readObject());
		deserialize((int[]) in.readObject());
	}
}
