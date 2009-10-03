package de.libalf.jni;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import de.libalf.AlfException;
import de.libalf.Normalizer;

public class JNINormalizer extends JNIObject implements Normalizer {

	/**
	 * The serial Version UID
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Creates a new JNINormalizer.
	 */
	public JNINormalizer() {
		this.pointer = init();
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++ normalizer object
	 * without any parameters and returns the pointer to this object.
	 * 
	 * @return a pointer to the memory location of the new C++ object.
	 */
	private native long init();

	/**
	 * Serializes this normalizer. This serialization can be saved and the
	 * normalizer can be restored using the
	 * {@link JNINormalizer#deserialize(int[])} method.
	 * 
	 * @return an int array that stores the serialization of the normalizer.
	 * @throws AlfException
	 *             throws the exception if the object has already been
	 *             destroyed.
	 */
	public int[] serialize() throws AlfException {
		check();
		return serialize(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNINormalizer#serialize()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the normalizer C++ object.
	 * @return the result of the JNI call.
	 */
	private native int[] serialize(long pointer);

	/**
	 * Restores the data of an a priori serialized normalizer. All data
	 * potentially contained in the normalizer is dropped before.
	 * 
	 * @param serialization
	 *            a serialization of a normalizer
	 * @return true, if the recovery was successful and false, otherwise.
	 * @throws AlfException
	 *             throws the exception if the object has already been
	 *             destroyed.
	 */
	public boolean deserialize(int[] serialization) throws AlfException {
		check();
		return deserialize(serialization, this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNINormalizere#deserialize(int[])}.
	 * </p>
	 * 
	 * @param serialization
	 *            a serialization of a normalizer
	 * @param pointer
	 *            the pointer to the normalizer C++ object.
	 * @return the result of the JNI call.
	 */
	private native boolean deserialize(int[] serialization, long pointer);

	@Override
	public void destroy() {
		check();
		destroy(this.pointer);
		this.isAlive = false;
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNINormalizerm#destroy()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the normalizer C++ object.
	 */
	private native void destroy(long pointer);

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException,
			AlfException {
		check();
		out.defaultWriteObject();
		out.writeObject(serialize());
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException,
			ClassNotFoundException, AlfException {
		check();
		in.defaultReadObject();
		this.pointer = init();
		int[] serialization = (int[]) in.readObject();
		deserialize(serialization);
	}

}
