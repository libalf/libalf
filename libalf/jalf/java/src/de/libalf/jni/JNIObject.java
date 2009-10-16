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

import de.libalf.AlfObjectDestroyedException;
import de.libalf.LibALFObject;

/**
 * <p>
 * Root of all classes representing the JNI LibALF C++ objects.
 * </p>
 * <p>
 * Each <code>JNIObject</code> stores a 64 bit variable (a <em>pointer</em>)
 * that points to memory location of the C++ object. Each <code>native</code>
 * method call on C++ objects via the JNI interface has to provide a pointer to
 * locate the object.<br>
 * The 64 bit variable allows the memory access on both 32 bit and 64 bit
 * systems.
 * </p>
 * <p>
 * This class is only the root class and should not be initialized. Each
 * subclass has to provide an <code>init</code> method that initializes a C++
 * object via the JNI interface and returns the memory address of the object.
 * This initialization can be performed in the constructor of the subclass.
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 */
public abstract class JNIObject implements LibALFObject {
	private static final long serialVersionUID = 1L;

	/**
	 * Load the JNI library.
	 */
	static {
		System.loadLibrary("jalf");
	}

	public static final String JNI_BINDING_VERSION = "JNI binding version 0.1";

	/**
	 * Stores the reference of the C++ object.
	 */
	transient protected long pointer;

	/**
	 * Stores whether the object has been killed.
	 */
	protected boolean isAlive = true;
	
	/**
	 * Returns the pointer to the C++ object.
	 * 
	 * @return the pointer to the C++ object.
	 */
	protected long getPointer() {
		return this.pointer;
	}
	
	@Override
	public JNIFactory getFactory() {
		return JNIFactory.STATIC;
	}

	/**
	 * Returns the LibALF and JNI binding versions.
	 * 
	 * @return the LibALF and JNI binding versions.
	 */
	@Override
	public String getVersion() {
		return getVersionStatic();
	}

	public static String getVersionStatic() {
		return getLibALFVersion() + " / " + JNI_BINDING_VERSION;
	}

	/**
	 * <p>
	 * <em>JNI method call:</em> Retrieves the libalf version string.
	 * </p>
	 * 
	 * @return the libalf version string.
	 */
	private static native String getLibALFVersion();
	
	/**
	 * Checks whether the object is destroyed and can not perform actions.
	 *
	 * @return <code>true</code> iff the object is destroyed.
	 */
	@Override
	public boolean isDestroyed() {
		return !this.isAlive;
	}
	
	/**
	 * Check performed by every method before an operation is performed.
	 * The method performs the following:
	 * <ol>
	 * <li>It check whether the object is alive.</li>
	 * <li>If it is not alive, then an {@link AlfObjectDestroyedException} is thrown.</li>
	 * </ol>
	 */
	protected void check() throws AlfObjectDestroyedException {
		if(isDestroyed()) throw new AlfObjectDestroyedException("Object has been destroyed.");
	}
	
	/**
	 * <p>Kills the object by freeing the memory occupied by the C++ object.</p>
	 * <p>After an object is dead, no more operations on this object can be performed.</p>
	 */
	@Override
	public abstract void destroy();
	
	@Override
	protected void finalize() throws Throwable {
		if(this.isAlive) {
			destroy();
			this.isAlive = false;
		}
	}
}
