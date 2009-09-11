package de.libalf.dispatcher;

import de.libalf.AlfException;
import de.libalf.Logger;

public class DispatcherLogger extends DispatcherObject implements Logger {
	public DispatcherLogger(DispatcherFactory factory) throws AlfException {
		super(factory, DispatcherConstants.OBJ_LOGGER);
	}

	@Override
	public String receive_and_flush() throws AlfException {
		return this.factory.dispatchObjectCommandLoggerReceiveAndFlush(this);
	}
}
