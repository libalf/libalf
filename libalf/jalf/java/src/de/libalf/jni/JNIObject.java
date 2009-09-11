package de.libalf.jni;

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
public abstract class JNIObject {

	/**
	 * Load the JNI library.
	 */
	static {
		System.loadLibrary("jalf");
	}

	public static final String JNI_BINDING_VERSION = "JNI binding version 0.9";

	/**
	 * Stores the reference of the C++ object.
	 */
	transient protected long pointer;

	/**
	 * Stores whether the object has been killed.
	 */
	transient protected boolean isAlive = true;
	
	/**
	 * Returns the pointer to the C++ object.
	 * 
	 * @return the pointer to the C++ object.
	 */
	protected long getPointer() {
		return this.pointer;
	}

	/**
	 * Returns the LibALF and JNI binding versions.
	 * 
	 * @return the LibALF and JNI binding versions.
	 */
	public static String getVersion() {
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
	 * Checks whether the object is living and can perform actions.
	 *
	 * @return the status of the object.
	 */
	public boolean isAlive() {
		return isAlive;
	}
	
	/**
	 * <p>Kills the object by freeing the memory occupied by the C++ object.</p>
	 * <p>After an object is dead, no more operations on this object can be performed.</p>
	 */
	public void kill() {
		kill(pointer);
		isAlive = false;
	}
	
	/**
	 * <p>
	 * <em>JNI method call:</em> See {@link JNIObject#kill()}.
	 * </p>
	 *
	 * @param pointer
	 *            the pointer to the C++ object.
	 */
	private native void kill(long pointer);
	
	@Override
	protected void finalize() throws Throwable {
		if(isAlive) {
			isAlive = false;
			kill(pointer);
		}
	}
}
