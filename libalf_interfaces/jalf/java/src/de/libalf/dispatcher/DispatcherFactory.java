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

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.net.Socket;

import de.libalf.AlfException;
import de.libalf.Knowledgebase;
import de.libalf.LibALFFactory;
import de.libalf.Logger;
import de.libalf.Normalizer;
import de.libalf.Knowledgebase.Acceptance;

public class DispatcherFactory implements LibALFFactory {
	private static final long serialVersionUID = 1L;

	private transient DataInputStream in;
	private transient DataOutputStream out;

	private final String host;
	private final int port;

	public DispatcherFactory(String host, int port) throws DispatcherIOException, DispatcherProtocolException {
		// socket
		this.host = host;
		this.port = port;

		// get init stuff
		init();
	}

	private void init() {
		connect();

		int code = readInt();
		if (code != 0)
			throw new DispatcherCommandError(code, DispatcherConstants.CLCMD_REQ_CAPA);
		String proto = readString();
		if (!proto.toLowerCase().equals("protocol-version-1"))
			throw new DispatcherProtocolException("Dispatcher version mismatch! Need \"protocol-version-1\", got \"" + proto + "\"");
	}

	private void connect() {
		try {
			Socket socket = new Socket(this.host, this.port);
			this.in = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
			this.out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
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
	// KILLING

	@Override
	protected void finalize() throws Throwable {
		destroy();
		super.finalize();
	}

	@Override
	public void destroy() throws DispatcherCommandError, DispatcherIOException {
		if (isDestroyed())
			return;
		try {
			int disco = writeCommand(DispatcherConstants.CLCMD_DISCONNECT);
			if (disco != 0) {
				System.err.print("WARNING -- ");
				new DispatcherCommandError(disco, DispatcherConstants.CLCMD_DISCONNECT).printStackTrace();
			}
		} finally {
			try {
				this.in.close();
				this.out.close();
			} catch (IOException e) {
				throw new DispatcherIOException(e);
			} finally {
				this.in = null;
				this.out = null;
			}
		}
	}

	@Override
	public boolean isDestroyed() {
		return this.in == null || this.out == null;
	}

	////////////////////////////////////////////////////////////////
	// SENDING

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
		if (isDestroyed())
			throw new DispatcherObjectDestroyedException("Factory has been destroyed.");

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
		if (obj.isDestroyed())
			throw new DispatcherObjectDestroyedException("Object has been destroyed.");

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

		DispatcherCommandError cmdError = new DispatcherCommandError(code, objCmd);
		if (code == DispatcherConstants.ERR_NO_OBJECT.id)
			throw new DispatcherObjectDestroyedException(cmdError);
		throw cmdError;
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

	@SuppressWarnings("deprecation")
	public Acceptance readAcceptance() {
		if (!readBool())
			return Acceptance.UNKNOWN;

		int i = readInt();
		switch (i) {
		case 0:
			return Acceptance.REJECT;
		case 1:
			return Acceptance.DONTCARE;
		case 2:
			return Acceptance.ACCEPT;
		default:
			throw new DispatcherProtocolException("unknown acceptance type: " + i + " (" + String.format("0x%08X", i) + ")");
		}

	}

	void printRest(int wait) throws Throwable {
		if (isDestroyed())
			return;

		Thread.sleep(wait);
		while (this.in.available() > 0) {
			System.out.println(this.in.available() < 4 ? String.format("0x%02X", readByte()) : String.format("0x%08X", readInt()));
		}
	}

	////////////////////////////////////////////////////////////////
	// COMMUNICATION
	// has to be synchronized in order to not interleave requests of concurrent threads.

	public synchronized String requestCapa() {
		writeCommandThrowing(DispatcherConstants.CLCMD_REQ_CAPA);
		return readString();
	}

	@Override
	public synchronized String getVersion() {
		writeCommandThrowing(DispatcherConstants.CLCMD_REQ_VERSION);
		return readString();
	}

	private void dispatchHelloCarsten(int i) throws DispatcherIOException {
		writeCommandThrowing(DispatcherConstants.CLCMD_HELLO_CARSTEN, i);
		if (readInt() != i)
			throw new DispatcherProtocolException(DispatcherConstants.CLCMD_HELLO_CARSTEN.toString());
	}

	public void sendNoOp() throws DispatcherIOException {
		dispatchHelloCarsten(0xDEADBEEF);
	}

	////////////////////////////////////////////////////////////////
	// FACTORY STUFF

	@Override
	public DispatcherFactory getFactory() {
		return this;
	}

	@Override
	public DispatcherLogger createLogger(Object... args) {
		if (args.length == 0)
			return new DispatcherLogger(this);
		throw new AlfException("Invalid parameters for creating logger: " + args.length + ".");
	}

	@Override
	public DispatcherKnowledgebase createKnowledgebase(Object... args) {
		if (args.length == 0)
			return new DispatcherKnowledgebase(this);
		throw new AlfException("Invalid parameters for creating knowledgebase: " + args.length + ".");
	}

	@Override
	public DispatcherLearningAlgorithm createLearningAlgorithm(Algorithm algorithm, Object... args) {
		switch (algorithm) {

		case ANGLUIN:
			/*
			 * Create Angluin learning algorithm.
			 */
			if (args.length == 2)
				return new DispatcherAlgorithmAngluin(this, (Knowledgebase) args[0], (Integer) args[1], null);
			else if (args.length == 3)
				return new DispatcherAlgorithmAngluin(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating Angluin learning algorithm: " + args.length + ".");

		case ANGLUIN_COLUMN:
			/*
			 * Create Angluin (column) learning algorithm.
			 */
			if (args.length == 2)
				return new DispatcherAlgorithmAngluinColumn(this, (Knowledgebase) args[0], (Integer) args[1], null);
			else if (args.length == 3)
				return new DispatcherAlgorithmAngluinColumn(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating Angluin (column) learning algorithm: " + args.length + ".");

		case NL_STAR:
			/*
			 * Create NL^* learning algorithm.
			 */
			if (args.length == 2)
				return new DispatcherAlgorithmNLstar(this, (Knowledgebase) args[0], (Integer) args[1], null);
			else if (args.length == 3)
				return new DispatcherAlgorithmNLstar(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating NL^* learning algorithm: " + args.length + ".");

		case KEARNS_VAZIRANI:
			/*
			 * Create Kearns-Vazirani learning algorithm.
			 */
			if (args.length == 2)
				return new DispatcherAlgorithmKearnsVazirani(this, (Knowledgebase) args[0], (Integer) args[1], null);
			else if (args.length == 3)
				return new DispatcherAlgorithmKearnsVazirani(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2]);
			else if (args.length == 4)
				return new DispatcherAlgorithmKearnsVazirani(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2], (Boolean) args[3]);
			else
				throw new AlfException("Invalid parameters for creating Kearns-Vazirani learning algorithm: " + args.length + ".");
				
		case RIVEST_SCHAPIRE:
			/*
			 * Create Rivest and Schapire's learning algorithm.
			 */
			if (args.length == 2)
				return new DispatcherAlgorithmRivestSchapire(this, (Knowledgebase) args[0], (Integer) args[1], null);
			else if (args.length == 3)
				return new DispatcherAlgorithmRivestSchapire(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating Rivest and Schapire's learning algorithm: " + args.length + ".");
				
		case RPNI:
			/*
			 * Create RPNI learning algorithm.
			 */
			if (args.length == 2)
				return new DispatcherAlgorithmRPNI(this, (Knowledgebase) args[0], (Integer) args[1], null);
			else if (args.length == 3)
				return new DispatcherAlgorithmRPNI(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating RPNI learning algorithm: " + args.length + ".");

		case BIERMANN_MINISAT:
			/*
			 * Create Biermann (MiniSAT) learning algorithm.
			 */
			if (args.length == 2)
				return new DispatcherAlgorithmBiermannMiniSAT(this, (Knowledgebase) args[0], (Integer) args[1], null);
			else if (args.length == 3)
				return new DispatcherAlgorithmBiermannMiniSAT(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating Biermann (MiniSAT) learning algorithm: " + args.length + ".");

		case BIERMANN_ORIGINAL:
			/*
			 * Create Biermann (original) learning algorithm.
			 */
			if (args.length == 3)
				return new DispatcherAlgorithmBiermannOriginal(this, (Knowledgebase) args[0], (Integer) args[1], null, (Integer) args[2]);
			else if (args.length == 4)
				return new DispatcherAlgorithmBiermannOriginal(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2], (Integer) args[3]);
			else
				throw new AlfException("Invalid parameters for creating Biermann (original) learning algorithm: " + args.length + ".");
				
		case DELETE2:
			/*
			 * Create DeLeTe2 inference algorithm.
			 */
			if (args.length == 2)
				return new DispatcherAlgorithmDeLeTe2(this, (Knowledgebase) args[0], (Integer) args[1], null);
			else if (args.length == 3)
				return new DispatcherAlgorithmDeLeTe2(this, (Knowledgebase) args[0], (Integer) args[1], (Logger) args[2]);
			else
				throw new AlfException("Invalid parameters for creating DeLeTe2 inference algorithm: " + args.length + ".");

		default:
			/*
			 * Default switch: Should never happen.
			 */
			return null;
		}
	}

	@Override
	public Normalizer createNormalizer(Normalizer.Type normType, Object... args) {
		switch (normType) {

		case MSC:
			/*
			 * Create Angluin learning algorithm.
			 */
			if (args.length == 0)
				return new DispatcherNormalizer(this, DispatcherConstants.NORMALIZER_MSC);
			else
				throw new AlfException("Invalid parameters for creating MSC normalizer: " + args.length + ".");

		default:
			/*
			 * Default switch: Should never happen.
			 */
			return null;
		}
	}
}
