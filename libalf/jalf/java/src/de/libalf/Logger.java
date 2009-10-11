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
 * (c) 2009 by Daniel Neider, i7 Informatik RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 *
 */

package de.libalf;

/**
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
 * @version 0.1
 */
public interface Logger extends LibALFObject {

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
	}

	/**
	 * The logger's default log level.
	 */
	public static final LoggerLevel DEFAULT_LOGGER_LEVEL = LoggerLevel.LOGGER_DEBUG;

	/**
	 * Flushes the logger and receives all messages logged since the last method
	 * call.
	 * 
	 * @return all messages logged since the last method call.
	 */
	public abstract String receive_and_flush();

}
