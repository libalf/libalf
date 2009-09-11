package de.libalf.dispatcher;

import de.libalf.AlfObjectDestroyedException;

public class DispatcherObjectDestroyedException extends AlfObjectDestroyedException {
	private static final long serialVersionUID = 1L;

	public DispatcherObjectDestroyedException(Throwable cause) {
		super(cause);
	}
}
