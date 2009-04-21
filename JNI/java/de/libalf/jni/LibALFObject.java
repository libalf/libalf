package de.libalf.jni;

/**
 * <p>
 * Root of all classes representing LibALF C++ objects.
 * </p>
 * <p>
 * Each <code>LibALFObject</code> stores a 64 bit variable (a <em>pointer</em>)
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
public abstract class LibALFObject {

	/**
	 * Load the JNI library.
	 */
	static {
		System.loadLibrary("alf_jni");
	}

	/**
	 * Stores the reference of the C++ object.
	 */
	long pointer;

	/**
	 * Returns the pointer to the C++ object.
	 * 
	 * @return the pointer to the C++ object.
	 */
	public long getPointer() {
		return this.pointer;
	}

}
