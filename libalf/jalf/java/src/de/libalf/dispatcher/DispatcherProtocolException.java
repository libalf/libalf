package de.libalf.dispatcher;

public class DispatcherProtocolException extends DispatcherException {
	private static final long serialVersionUID = 1L;

	public DispatcherProtocolException() {}

	public DispatcherProtocolException(String message) {
		super(message);
	}

	public DispatcherProtocolException(String message, Throwable cause) {
		super(message, cause);
	}

}
