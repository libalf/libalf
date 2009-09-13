package de.libalf.dispatcher;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import de.libalf.AlfException;
import de.libalf.Logger;

public class DispatcherLogger extends DispatcherObject implements Logger {
	private static final long serialVersionUID = 1L;
	private String unread = "";

	public DispatcherLogger(DispatcherFactory factory) throws AlfException {
		super(factory, DispatcherConstants.OBJ_LOGGER);
		create();
	}

	@Override
	public String receive_and_flush() throws AlfException {
		String ret = this.unread + this.factory.dispatchObjectCommandLoggerReceiveAndFlush(this);
		this.unread = ""; // reset unread
		return ret;
	}

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException {
		this.unread = receive_and_flush();
		out.defaultWriteObject();
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		create();
		receive_and_flush(); // grab first message
		in.defaultReadObject();
	}
}
