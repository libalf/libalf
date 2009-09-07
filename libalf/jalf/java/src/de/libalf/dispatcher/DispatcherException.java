package de.libalf.dispatcher;

import de.libalf.AlfException;

public class DispatcherException extends AlfException {
	private static final long serialVersionUID = 1L;

	public DispatcherException() {
		super();
	}

	public DispatcherException(Throwable cause) {
		super(cause);
	}

	public DispatcherException(String message) {
		super(message);
	}

	public DispatcherException(String message, Throwable cause) {
		super(message, cause);
	}
}
