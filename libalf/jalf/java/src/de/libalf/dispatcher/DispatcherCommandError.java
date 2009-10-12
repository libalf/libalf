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
 * (c) 2009 by Stefan Schulz, Chair of Computer Science 2 and 7, RWTH-Aachen
 *
 */

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
