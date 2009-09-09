package de.libalf.dispatcher;

enum DispatcherError {
	// enum client_command
	SUCCESS(0), ;

	private final int id;

	private DispatcherError(int id) {
		this.id = id;
	}

	static boolean isError(int code) {
		return code == 0;
	}

	static String toString(int code) {
		DispatcherError error = getError(code);
		return code + " (0x"+Integer.toHexString(code)+")" + (error == null ? "" : " " + error);
	}

	private static DispatcherError getError(int code) {
		for (DispatcherError error : DispatcherError.values())
			if (error.id == code)
				return error;
		return null;
	}
}
