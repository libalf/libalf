package de.libalf.dispatcher;

import java.io.NotSerializableException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

public class DispatcherKnowledgebaseIterator extends DispatcherObject {
	private static final long serialVersionUID = -1L;

	DispatcherKnowledgebaseIterator(DispatcherFactory factory, int id) {
		super(factory, DispatcherConstants.OBJ_KNOWLEDGEBASE_ITERATOR);
		useID(id);
	}

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws NotSerializableException {
		throw new NotSerializableException(getClass().getName());
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws NotSerializableException {
		throw new NotSerializableException(getClass().getName());
	}
}
