package de.libalf.dispatcher;

import de.libalf.AlfException;

public class DispatcherProtocolException extends AlfException {
	private static final long serialVersionUID = 1L;

	public DispatcherProtocolException() {}

	public DispatcherProtocolException(String message) {
		super(message);
	}

	public DispatcherProtocolException(String message, Throwable cause) {
		super(message, cause);
	}

}
