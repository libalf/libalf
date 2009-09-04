package de.libalf.dispatcher;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;


/**
 * <p>
 * A <code>BufferedLogger</code> logs events that occur during a learning
 * process and the logged events can be received as a <code>String</code>. A
 * logger can be attached to any {@link LearningAlgorithm}.
 * </p>
 * <p>
 * Each logged event is associated with a specific {@link LoggerLevel}, which
 * indicates the type of the event. The logger levels are thereby ordered:
 * <ol>
 * <li>{@link LoggerLevel#LOGGER_ERROR}</li>
 * <li>{@link LoggerLevel#LOGGER_WARN}</li>
 * <li>{@link LoggerLevel#LOGGER_INFO}</li>
 * <li>{@link LoggerLevel#LOGGER_DEBUG}</li>
 * </ol>
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
 */
public class BufferedLogger extends LibALFObject implements Serializable {
	private static final long serialVersionUID = 2L;

	/**
	 * Mapping for the JNI call.
	 */
	private static final int ERROR = 1;
	private static final int WARN = 2;
	private static final int INFO = 3;
	private static final int DEBUG = 4;

	private String unread = "";

	/**
	 * Enumeration of all available logger levels.
	 * 
	 * @author Daniel Neider (<a
	 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
	 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
	 *         University
	 * @version 1.0
	 */
	public static enum LoggerLevel {

		/**
		 * All log messages that describe a non-recoverable error are marked
		 * with <code>LOGGER_ERROR</code>.
		 */
		LOGGER_ERROR,

		/**
		 * Messages describing a state or command that is erroneous but may be
		 * ignored under most conditions are marked with
		 * <code>LOGGER_WARN</code>.
		 */
		LOGGER_WARN,

		/**
		 * Any information not describing an erroneous condition are marked with
		 * <code>LOGGER_INFO</code>.
		 */
		LOGGER_INFO,

		/**
		 * Messages that may help debugging of LibALF are marked with
		 * <code>LOGGER_DEBUG</code>. <code>LOGGER_DEBUG</code> is usually only
		 * used during development of libALF and removed afterwards, as these
		 * can be <b>very</b> verbose and thus CPU consuming. Still, some debug
		 * messages may be left in a release version.
		 */
		LOGGER_DEBUG;

		public int intValue() {
			switch (this) {
			case LOGGER_ERROR:
				return ERROR;
			case LOGGER_WARN:
				return WARN;
			case LOGGER_INFO:
				return INFO;
			case LOGGER_DEBUG:
				return DEBUG;
			default:
				return ERROR;
			}
		}
	}

	/**
	 * The logger's default log level.
	 */
	public static LoggerLevel DEFAULT_LOGGER_LEVEL = LoggerLevel.LOGGER_DEBUG;

	/**
	 * Creates a new buffered logger.
	 */
	BufferedLogger(DispatcherSession session) {
		super(session, ObjectType.LOGGER);
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

	@Override
	public long getID() {
		return this.id;
	}

	/**
	 * Creates a new buffered logger. The logger has the given minimal log level
	 * or {@link BufferedLogger#DEFAULT_LOGGER_LEVEL} if
	 * <code>minimalLogLevel</code> is <code>null</code>.<br>
	 * 
	 * @param minimalLogLevel the minimal log level, an integer between 1
	 *            (ERROR) and 4 (DEBUG)
	 * @param logAlgorithm also log algorithm-specific information? (depends on
	 *            algorithm)
	 */
	public BufferedLogger(LoggerLevel minimalLogLevel, boolean logAlgorithm) {
		this.id = init(this.minimalLogLevel = minimalLogLevel.intValue(), this.logAlgorithm = logAlgorithm);
	}

	/**
	 * <p>
	 * <em>JNI method call:</em>
	 * </p>
	 * Invokes the JNI interface to initialize a new C++ buffered_logger object
	 * with the given parameters and returns the pointer to this object.
	 * 
	 * @param minimalLogLevel the minimal log level, an integer between 1
	 *            (ERROR) and 4 (DEBUG)
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
		String ret = this.unread + receive_and_flush(this.id);
		this.unread = "";
		return ret;
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link BufferedLogger#receive_and_flush()}.
	 * </p>
	 * 
	 * @param pointer the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native String receive_and_flush(long pointer);

	private Integer minimalLogLevel;
	private Boolean logAlgorithm;

	private void writeObject(ObjectOutputStream out) throws IOException {
		this.unread = receive_and_flush();
		out.defaultWriteObject();
	}

	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		this.id = this.minimalLogLevel == null || this.logAlgorithm == null ? init() : init(this.minimalLogLevel, this.logAlgorithm);
		receive_and_flush(); // grab first message
		in.defaultReadObject();
	}
}
