package de.libalf;

public class AlfObjectDestroyedException extends AlfException {
	private static final long serialVersionUID = 1L;

	public AlfObjectDestroyedException() {
		super();
	}

	public AlfObjectDestroyedException(Throwable cause) {
		super(cause);
	}

	public AlfObjectDestroyedException(String message) {
		super(message);
	}

	public AlfObjectDestroyedException(String message, Throwable cause) {
		super(message, cause);
	}
}