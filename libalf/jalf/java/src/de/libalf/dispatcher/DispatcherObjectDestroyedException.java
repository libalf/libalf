package de.libalf.dispatcher;

import de.libalf.AlfObjectDestroyedException;

public class DispatcherObjectDestroyedException extends AlfObjectDestroyedException {
	private static final long serialVersionUID = 1L;

	public DispatcherObjectDestroyedException(String message) {
		super(message);
	}

	public DispatcherObjectDestroyedException(DispatcherCommandError cause) {
		super(cause);
	}

	@Override
	public DispatcherCommandError getCause() {
		return (DispatcherCommandError) super.getCause();
	}
}
