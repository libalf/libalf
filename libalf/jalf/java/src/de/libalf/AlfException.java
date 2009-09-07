package de.libalf;

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

	public AlfException(String message, Throwable cause) {
		super(message, cause);
	}
}
