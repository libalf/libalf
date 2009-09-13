package de.libalf.dispatcher;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.net.Socket;

import de.libalf.AlfException;
import de.libalf.Knowledgebase.Acceptance;

class DispatcherSocket implements Serializable {
	private static final long serialVersionUID = 1L;

	private transient DataInputStream in;
	private transient DataOutputStream out;

	private final String host;
	private final int port;

	public DispatcherSocket(String host, int port) throws DispatcherIOException {
		this.host = host;
		this.port = port;
		init();
	}

	private void init() {
		try {
			Socket socket = new Socket(this.host, this.port);
			this.in = new DataInputStream(socket.getInputStream());
			this.out = new DataOutputStream(socket.getOutputStream());
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		init();
	}
	
	////////////////////////////////////////////////////////////////
	// SENDING

	void close() {
		try {
			this.in.close();
			this.out.close();
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	private void flush() {
		try {
			this.out.flush();
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	private void writeSendable(Sendable o) {
		writeInt(o.getInt());
	}

	private void writeBool(boolean b) {
		writeInt(b ? 1 : 0);
	}

	private void writeInt(int i) {
		try {
			this.out.writeInt(i);
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	private void writeInts(int[] is) {
		writeInt(is.length);
		for (int i : is)
			writeInt(i);
	}

	private int getSize(Object... args) {
		int size = 0;
		for (Object arg : args) {
			if (arg instanceof Sendable)
				size++;
			else if (arg instanceof Boolean)
				size++;
			else if (arg instanceof Integer)
				size++;
			else if (arg instanceof int[])
				size += 1 + ((int[]) arg).length;
			else
				throw new IllegalArgumentException();
		}
		return size;
	}

	int writeCommand(DispatcherConstants cmd, Object... args) throws DispatcherIOException {
		// send command
		writeSendable(cmd);

		// send arguments
		for (Object arg : args) {
			if (arg instanceof Sendable)
				writeSendable((Sendable) arg);
			else if (arg instanceof Boolean)
				writeBool((Boolean) arg);
			else if (arg instanceof Integer)
				writeInt((Integer) arg);
			else if (arg instanceof int[])
				writeInts((int[]) arg);
			else
				throw new IllegalArgumentException();
		}

		flush();

		// get response code
		return readInt();
	}

	int writeObjectCommand(DispatcherObject obj, DispatcherConstants objCmd, Object... args) {
		Object[] newArgs = new Object[args.length + 3];
		newArgs[0] = obj;
		newArgs[1] = objCmd;
		newArgs[2] = getSize(args);
		System.arraycopy(args, 0, newArgs, 3, args.length);

		return writeCommand(DispatcherConstants.CLCMD_OBJECT_COMMAND, newArgs);
	}

	void writeCommandThrowing(DispatcherConstants cmd, Object... args) throws AlfException {
		int code = writeCommand(cmd, args);
		if (code == 0)
			return;

		throw new DispatcherCommandError(code, cmd);
	}

	void writeObjectCommandThrowing(DispatcherObject obj, DispatcherConstants objCmd, Object... args) {
		int code = writeObjectCommand(obj, objCmd, args);
		if (code == 0)
			return;

		AlfException exception = new DispatcherCommandError(code, objCmd);
		if (code == DispatcherConstants.ERR_NO_OBJECT.id)
			exception = new DispatcherObjectDestroyedException(exception);
		throw exception;
	}

	////////////////////////////////////////////////////////////////
	// RECEIVING

	public boolean readBool() throws DispatcherIOException {
		return readInt() != 0;
	}

	public int readInt() throws DispatcherIOException {
		try {
			int i = this.in.readInt();
			//			System.out.println(">> " + DispatcherConstants.printUInt32(i));
			return i;
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	public int[] readInts() throws DispatcherIOException {
		int[] is = new int[readInt()];
		for (int i = 0; i < is.length; i++)
			is[i] = readInt();
		return is;
	}

	private byte readByte() {
		try {
			byte b = this.in.readByte();
			//			System.out.println(">> " + DispatcherConstants.printUInt8(b));
			return b;
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	@SuppressWarnings("deprecation")
	public String readString() throws DispatcherIOException {
		byte[] buf = new byte[readInt()];
		for (int i = 0; i < buf.length; i++)
			buf[i] = readByte();
		assert is7bit(buf);
		return new String(buf, 0);
	}

	private boolean is7bit(byte[] buf) {
		for (byte b : buf)
			if (b < 0)
				return true;
		return true;
	}

	public Acceptance readAcceptance() {
		if (!readBool())
			return null;

		int i = readInt();
		switch (i) {
		case 0:
			return Acceptance.REJECT;
		case 1:
			return Acceptance.UNKNOWN;
		case 2:
			return Acceptance.ACCEPT;
		}

		throw new DispatcherProtocolException("unknown acceptance type: " + i + " (" + String.format("0x%08X", i) + ")");
	}

	@Override
	protected void finalize() throws Throwable {
		close();
		super.finalize();
	}

	void printRest(int wait) throws Throwable {
		Thread.sleep(wait);
		while (this.in.available() > 0) {
			System.out.println(this.in.available() < 4 ? String.format("0x%02X", readByte()) : String.format("0x%08X", readInt()));
		}
	}
}
