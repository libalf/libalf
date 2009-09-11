package de.libalf.dispatcher;

import java.io.IOException;

import de.libalf.AlfException;

public class DispatcherIOException extends AlfException {
	private static final long serialVersionUID = 1L;

	public DispatcherIOException(IOException e) {
		super(e);
	}

	@Override
	public IOException getCause() {
		return (IOException) super.getCause();
	}
}
