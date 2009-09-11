package de.libalf.dispatcher;

public class DispatcherCommandError extends DispatcherException {
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

	static String printUInt32(int code) {
		return String.format("%08X", code);
	}

	static String printUInt8(byte code) {
		return String.format("%02X", code);
	}

	public static DispatcherConstants getError(int code) {
		for (DispatcherConstants c : DispatcherConstants.values())
			if (c.id == code && c.toString().startsWith("ERR_"))
				return c;
		return null;
	}

	static String getErrorString(int code) {
		DispatcherConstants error = getError(code);
		return code + " (0x" + printUInt32(code) + ")" + (error == null ? "" : " " + error);
	}
}
