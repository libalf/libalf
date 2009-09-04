package de.libalf.dispatcher;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

public class DispatcherSocket {
	static enum Command {
		REQ_CAPA(0),
		REQ_VERSION(1),
		CREATE_OBJECT(20), HELLO_CARSTEN(99);

		private final int id;

		private Command(int id) {
			this.id = id;
		}
	}

	static enum ObjectType {
		LOGGER(0), KNOWLEDGEBASE(10);

		private final int id;

		private ObjectType(int id) {
			this.id = id;
		}
	}

	private DataInputStream in;
	private DataOutputStream out;

	public DispatcherSocket(Socket socket) throws DispatcherIOException {
		try {
			this.in = new DataInputStream(socket.getInputStream());
			this.out = new DataOutputStream(socket.getOutputStream());
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	////////////////////////////////////////////////////////////////
	// SENDING

	private void writeBool(boolean b) throws DispatcherIOException {
		writeInt(b ? 1 : 0);
	}

	private void writeInt(int i) throws DispatcherIOException {
		try {
			this.out.writeInt(i);
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	private void writeInts(int[] is) throws DispatcherIOException {
		writeInt(is.length);
		for (int i : is)
			writeInt(i);
	}

	private void writeCommand(Command cmd) throws DispatcherIOException {
		writeInt(cmd.id);
	}

	private void writeObjectType(ObjectType ot) throws DispatcherIOException {
		writeInt(ot.id);
	}

	boolean writeCommand(Command cmd, Object... args) throws DispatcherIOException {
		// send command
		writeCommand(cmd);

		// send arguments
		for (Object arg : args) {
			if (arg instanceof Boolean)
				writeBool((Boolean) arg);
			else if (arg instanceof Integer)
				writeInt((Integer) arg);
			else if (arg instanceof int[])
				writeInts((int[]) arg);
			else if (arg instanceof ObjectType)
				writeObjectType((ObjectType) arg);
			else
				throw new IllegalArgumentException();
			//else if (arg instanceof Integer[])
			//	writeInts((Integer[]) arg);
			//else if (arg instanceof String)
			//	writeString((String) arg);
		}

		// get response code
		return readBool();
	}

	////////////////////////////////////////////////////////////////
	// RECEIVING

	public boolean readBool() throws DispatcherIOException {
		return readInt() != 0;
	}

	public int readInt() throws DispatcherIOException {
		try {
			return this.in.readInt();
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

	private byte readByte() throws DispatcherIOException {
		try {
			return this.in.readByte();
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
}
