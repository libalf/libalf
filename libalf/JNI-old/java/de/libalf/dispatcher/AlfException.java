package de.libalf.dispatcher;

public class AlfException extends RuntimeException {
	private static final long serialVersionUID = 1L;

	public AlfException() {
		super();
	}

	public AlfException(Throwable cause) {
		super(cause);
	}

	public AlfException(String message) {
		super(message);
	}

	public AlfException(Throwable cause, String message) {
		super(message, cause);
	}
}
