package de.libalf.dispatcher;

import de.libalf.AlfException;

public class DispatcherCommandError extends AlfException {
	private static final long serialVersionUID = 1L;
	private int code;

	public DispatcherCommandError(int code, DispatcherConstants cmd) {
		super("command " + cmd + " failed: " + getErrorString(code));
		this.code = code;
	}

	public int getErrorCode() {
		return this.code;
	}

	public DispatcherConstants getError() {
		return getError(this.code);
	}

	public static DispatcherConstants getError(int code) {
		for (DispatcherConstants c : DispatcherConstants.values())
			if (c.id == code && c.toString().startsWith("ERR_"))
				return c;
		return null;
	}

	static String getErrorString(int code) {
		DispatcherConstants error = getError(code);
		return code + " (" + String.format("0x%08X", code) + ")" + (error == null ? "" : " " + error);
	}
}
