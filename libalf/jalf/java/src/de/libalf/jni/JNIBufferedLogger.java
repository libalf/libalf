package de.libalf.jni;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import de.libalf.Logger;

/**
 * TODO: More JavaDoc!
 * <p>
 * A <code>BufferedLogger</code> logs events that occur during a learning
 * process and the logged events can be received as a <code>String</code>. A
 * logger can be attached to any {@link LearningAlgorithm}.
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 * 
 */
public class JNIBufferedLogger extends JNIObject implements Logger, Serializable {

	private static final long serialVersionUID = 2L;

	private Integer minimalLogLevel;
	
	private Boolean logAlgorithm;
	
	/**
	 * Mapping for the JNI call.
	 */
	private static final int ERROR = 1;
	private static final int WARN = 2;
	private static final int INFO = 3;
	private static final int DEBUG = 4;

	private String unread = "";

	/**
	 * Creates a new buffered logger.
	 */
	public JNIBufferedLogger() {
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
	 * or {@link JNIBufferedLogger#DEFAULT_LOGGER_LEVEL} if
	 * <code>minimalLogLevel</code> is <code>null</code>.<br>
	 * 
	 * @param minimalLogLevel
	 *            the minimal log level, an integer between 1 (ERROR) and 4
	 *            (DEBUG)
	 * @param logAlgorithm
	 *            also log algorithm-specific information? (depends on
	 *            algorithm)
	 * 
	 */
	public JNIBufferedLogger(LoggerLevel minimalLogLevel, boolean logAlgorithm) {

		int ll;
		switch (minimalLogLevel) {
		case LOGGER_ERROR:
			ll = ERROR;
		case LOGGER_WARN:
			ll = WARN;
		case LOGGER_INFO:
			ll = INFO;
		case LOGGER_DEBUG:
			ll = DEBUG;
		default:
			ll = ERROR;
		}

		this.pointer = init(this.minimalLogLevel = ll,
				this.logAlgorithm = logAlgorithm);
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

	@Override
	public String receive_and_flush() {
		check();
		String ret = this.unread + receive_and_flush(this.pointer);
		this.unread = "";
		return ret;
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIBufferedLogger#receive_and_flush()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 * @return the result of the JNI call.
	 */
	private native String receive_and_flush(long pointer);

	@Override
	public void destroy() {
		check();
		destroy(pointer);
		isAlive = false;
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNILearningAlgorithm#destroy()}.
	 * </p>
	 * 
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void destroy(long pointer);
	
	private void writeObject(ObjectOutputStream out) throws IOException {
		check();
		this.unread = receive_and_flush();
		out.defaultWriteObject();
	}

	private void readObject(ObjectInputStream in) throws IOException,
			ClassNotFoundException {
		check();
		this.pointer = this.minimalLogLevel == null
				|| this.logAlgorithm == null ? init() : init(
				this.minimalLogLevel, this.logAlgorithm);
		receive_and_flush(); // grab first message
		in.defaultReadObject();
	}
}
