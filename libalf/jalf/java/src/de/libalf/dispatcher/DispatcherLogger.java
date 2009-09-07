package de.libalf.dispatcher;

import de.libalf.Logger;

public class DispatcherLogger extends DispatcherObject implements Logger {
	public DispatcherLogger(DispatcherFactory factory) throws DispatcherException {
		super(factory, DispatcherConstants.OBJ_LOGGER);
	}

	@Override
	public String receive_and_flush() throws DispatcherException {
		return this.factory.dispatchObjectCommandLoggerReceiveAndFlush(this);
	}
}
