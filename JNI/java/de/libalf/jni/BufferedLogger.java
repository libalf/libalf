package de.libalf.jni;

/**
 * <p>
 * A <code>BufferedLogger</code> logs events that occur during a learning
 * process and the logged events can be received as a <code>String</code>. A
 * logger is given as parameter when creating a new {@link LearningAlgorithm}.
 * </p>
 * <p>
 * Each logged event is associated with a specific {@link LoggerLevel}, which
 * indicates the type of the event. The logger levels are thereby ordered:
 * <table>
 * <tr>
 * <td>1</td>
 * <td>{@link LoggerLevel#LOGGER_ERROR}</td>
 * </tr>
 * <tr>
 * <td>2</td>
 * <td>{@link LoggerLevel#LOGGER_WARN}</td>
 * </tr>
 * <tr>
 * <td>3</td>
 * <td>{@link LoggerLevel#LOGGER_INFO}</td>
 * </tr>
 * <tr>
 * <td>4</td>
 * <td>{@link LoggerLevel#LOGGER_DEBUG}</td>
 * </tr>
 * </table>
 * <br>
 * If the logger level of an event is less than the logger's
 * <code>minimalLogLevel</code>, the event is discarded. Only events with logger
 * level greater and equal to the minimal logger level are in fact logged.
 * </p>
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
	private final int ERROR = 1;
	private final int WARN = 2;
	private final int INFO = 3;
	private final int DEBUG = 4;

	/**
	 * Enumeration of all available logger levels.
	 * 
	 * @author Daniel Neider (<a
	 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
	 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
	 *         University
	 * @version 1.0
	 * 
	 */
	public static enum LoggerLevel {
		LOGGER_ERROR, LOGGER_WARN, LOGGER_INFO, LOGGER_DEBUG
	}

	/**
	 * The logger's default log level.
	 */
	public static LoggerLevel DEFAULT_LOGGER_LEVEL = LoggerLevel.LOGGER_DEBUG;

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
	 * or {@link BufferedLogger#DEFAULT_LOGGER_LEVEL} if
	 * <code>minimalLogLevel</code> is <code>null</code>.<br>
	 * TODO: What does the logAlgorithm parameter?
	 * 
	 * @param minimalLogLevel
	 *            the minimal log level, an integer between 1 (ERROR) and 4
	 *            (DEBUG)
	 * @param logAlgorithm
	 *            also log algorithm-specific information? (depends on
	 *            algorithm)
	 *
	 */
	public BufferedLogger(LoggerLevel minimalLogLevel, boolean logAlgorithm) {
		switch (minimalLogLevel) {
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
		default:
			this.pointer = init(ERROR, logAlgorithm);
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
	 *            the minimal log level, an integer between 1 (ERROR) and 4
	 *            (DEBUG)
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
