/*
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

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
 * @version 0.1
 * 
 */
public class JNIBufferedLogger extends JNIObject implements Logger {

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
			break;
		case LOGGER_WARN:
			ll = WARN;
			break;
		case LOGGER_INFO:
			ll = INFO;
			break;
		case LOGGER_DEBUG:
			ll = DEBUG;
			break;
		default:
			ll = ERROR;
			break;
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
		destroy(this.pointer);
		this.isAlive = false;
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
	
	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException {
		this.unread = receive_and_flush();
		out.defaultWriteObject();
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException,
			ClassNotFoundException {
		this.pointer = this.minimalLogLevel == null
				|| this.logAlgorithm == null ? init() : init(
				this.minimalLogLevel, this.logAlgorithm);
		receive_and_flush(); // grab first message
		in.defaultReadObject();
	}
}
