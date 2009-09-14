package de.libalf;

import java.io.Serializable;

public interface LibALFObject extends Serializable {
	/**
	 * <p>Kills the object by freeing the memory occupied by the C++ object.</p>
	 * <p>After an object is dead, no more operations on this object can be performed.</p>
	 */
	public boolean isDestroyed();

	/**
	 * <p>Kills the object by freeing the memory occupied by the C++ object.</p>
	 * <p>After an object is dead, no more operations on this object can be performed.</p>
	 */
	public void destroy();
}
