package de.libalf.jni;

/**
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 * 
 */
public class BufferedLogger extends LibALFObject {

	/**
	 * Mapping for the JNI call.
	 */
	private final int INTERNAL = 0;
	private final int ERROR = 1;
	private final int WARN = 2;
	private final int INFO = 3;
	private final int DEBUG = 4;
	private final int ALGORITHM = 5;

	/**
	 * Enumeration of all available log levels.
	 * 
	 * @author Daniel Neider (<a
	 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
	 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
	 *         University
	 * @version 1.0
	 * 
	 */
	public static enum LoggerLevel {
		LOGGER_INTERNAL, LOGGER_ERROR, LOGGER_WARN, LOGGER_INFO, LOGGER_DEBUG, LOGGER_ALGORITHM
	}

	/**
	 * The logger's default log level.
	 */
	public static LoggerLevel DEFAULT_LOGGER_LEVEL = LoggerLevel.LOGGER_ALGORITHM;

	/**
	 * Creates a new buffered logger.
	 */
	public BufferedLogger() {
		this.pointer = init();
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++ buffered_logger object
	 * without any parameters and returns the pointer to this object.
	 * 
	 * @return a pointer to the memory location of the new C++ object.
	 */
	private native long init();

	/**
	 * Creates a new buffered logger. The logger has the given minimal log level
	 * and TODO: Description
	 * 
	 * @param minimalLogLevel
	 *            the minimal log level, an integer between 0 (INTERNAL) and 5
	 *            (ALGORITHM)
	 * @param logAlgorithm
	 */
	public BufferedLogger(LoggerLevel minimalLogLevel, boolean logAlgorithm) {
		switch (minimalLogLevel) {
		case LOGGER_INTERNAL:
			this.pointer = init(INTERNAL, logAlgorithm);
			break;
		case LOGGER_ERROR:
			this.pointer = init(ERROR, logAlgorithm);
			break;
		case LOGGER_WARN:
			this.pointer = init(WARN, logAlgorithm);
			break;
		case LOGGER_INFO:
			this.pointer = init(INFO, logAlgorithm);
			break;
		case LOGGER_DEBUG:
			this.pointer = init(DEBUG, logAlgorithm);
			break;
		case LOGGER_ALGORITHM:
			this.pointer = init(ALGORITHM, logAlgorithm);
			break;
		default:
			this.pointer = init(ALGORITHM, logAlgorithm);
			break;
		}
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++ buffered_logger object
	 * with the given parameters and returns the pointer to this object.
	 * 
	 * @param minimalLogLevel
	 *            the minimal log level, an integer between 0 (INTERNAL) and 5
	 *            (ALGORITHM)
	 * 
	 * @param logAlgorithm
	 * @return
	 */
	private native long init(int minimalLogLevel, boolean logAlgorithm);

	/**
	 * Flushes the logger and receives all messages logged since the last method
	 * call.
	 * 
	 * @return all messages logged since the last method call.
	 */
	public String receive_and_flush() {
		return receive_and_flush(this.pointer);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link BufferedLogger#receive_and_flush()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native String receive_and_flush(long pointer);
}
