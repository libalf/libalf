package de.libalf.dispatcher;

import java.io.Serializable;

import de.libalf.AlfException;

/**
 * TODO: check the following text
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
 * @author Stefan Schulz
 * @version 1.0
 */
public abstract class DispatcherObject implements Sendable, Serializable {
	private static final long serialVersionUID = 1L;

	/**
	 * Stores the reference of the dispatcher object id.
	 */
	transient private int id;

	/**
	 * Object's factory.
	 */
	protected final DispatcherFactory factory;

	private final DispatcherConstants objType;

	protected DispatcherObject(DispatcherFactory factory, DispatcherConstants objType) throws DispatcherProtocolException {
		this.factory = factory;
		this.objType = objType;
	}

	protected void useID(int id) throws DispatcherProtocolException {
		this.id = id;
		if (id < 0)
			throw new DispatcherProtocolException("Negative object identifier!");
	}

	protected void create(int[] data) throws DispatcherProtocolException {
		useID(this.factory.dispatchCreateObject(this.objType, data));
	}

	protected void create() throws DispatcherProtocolException {
		create(new int[0]);
	}

	@Override
	public int getInt() {
		return this.id;
	}

	@Override
	protected void finalize() throws Throwable {
		kill();
		super.finalize();
	}

	public void kill() throws AlfException {
		if (this.id < 0)
			return;

		this.factory.dispatchDeleteObject(this);
		this.id = -1;
	}

	public boolean isKilled() {
		return this.id < 0;
	}

	void checkFactory(Object obj) {
		if (obj instanceof DispatcherObject && ((DispatcherObject) obj).factory == this.factory)
			return;
		throw obj == null ? new NullPointerException() : new IllegalArgumentException("object has to be from the same factory!");
	}

	public DispatcherConstants getObjType() {
		return this.objType;
	}

	void checkObjType() {
		if (this.objType.getInt() != this.factory.dispatchGetObjectType(this))
			throw new DispatcherProtocolException("object type mismatch");
	}

	@Override
	public String toString() {
		return getClass().getCanonicalName() + " (type=" + this.objType + ", id=" + this.id + ")";
	}
}
