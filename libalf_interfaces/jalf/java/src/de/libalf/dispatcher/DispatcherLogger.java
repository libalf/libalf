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
 * Author: Stefan Schulz
 *
 */

package de.libalf.dispatcher;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import de.libalf.AlfException;
import de.libalf.Logger;

public class DispatcherLogger extends DispatcherObject implements Logger {
	private static final long serialVersionUID = 2L;
	private String unread = "";
	private LoggerLevel level = DEFAULT_LOGGER_LEVEL;

	public DispatcherLogger(DispatcherFactory factory) throws AlfException {
		super(factory, DispatcherConstants.OBJ_LOGGER);
		create();
		set_min_loglevel(this.level);
	}

	@Override
	public String receive_and_flush() throws AlfException {
		try {
			synchronized (this.factory) {
				this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LOGGER_RECEIVE_AND_FLUSH);
				return this.unread + this.factory.readString();
			}
		} finally {
			this.unread = ""; // reset unread
		}
	}

	@Override
	public void set_min_loglevel(Logger.LoggerLevel level) throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LOGGER_SET_MIN_LOGLEVEL, DispatcherConstants.convertLoggerLevel(level));
			this.level = level;
		}
	}

	@Override
	public LoggerLevel get_min_loglevel() throws AlfException {
		return this.level;
	}

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
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		create();
		receive_and_flush(); // grab first message
		in.defaultReadObject();
		set_min_loglevel(this.level);
	}
}
