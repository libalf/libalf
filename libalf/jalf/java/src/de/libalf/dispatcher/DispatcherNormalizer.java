package de.libalf.dispatcher;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import de.libalf.AlfException;

public class DispatcherNormalizer extends DispatcherObject {
	private static final long serialVersionUID = 1L;

	public DispatcherNormalizer(DispatcherFactory factory, DispatcherConstants normType) throws AlfException {
		super(factory, DispatcherConstants.OBJ_NORMALIZER);
		create(normType.id);
	}

	private void create(int normType) {
		create(new int[] { normType });
	}

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

	public int[] serialize() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.NORMALIZER_SERIALIZE);
			return this.factory.readInts();
		}
	}

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
