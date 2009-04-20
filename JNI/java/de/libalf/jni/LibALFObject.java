package de.libalf.jni;
/**
 * Parent class of all LibALF JNI objects.
 * 
 * @author Daniel Neider (Chair of Computer Science 7, RWTH Aachen University)
 * @version 1.0
 */
public abstract class LibALFObject {

	static {
		System.loadLibrary("alf_jni");
	}
	
	/**
	 * Stores the reference of the C++ object.
	 */
	long pointer;

	/**
	 * Returns the pointer to the C++ object.
	 * @return the pointer to the C++ object.
	 */
	public long getPointer() {
		return this.pointer;
	}
	
}
