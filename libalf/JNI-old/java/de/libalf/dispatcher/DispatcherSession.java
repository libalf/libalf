package de.libalf.dispatcher;

import java.io.IOException;
import java.net.Socket;

import de.libalf.dispatcher.DispatcherSocket.Command;
import de.libalf.dispatcher.DispatcherSocket.ObjectType;

public class DispatcherSession {
	DispatcherSocket io;

	public DispatcherSession(String host, int port) throws DispatcherIOException {
		try {
			// socket
			this.io = new DispatcherSocket(new Socket(host, port));

			// get init stuff
			if (!this.io.readBool())
				throw new DispatcherProtocolException("Connection init failed");
			String capa = this.io.readString(); // TODO
			System.out.println(capa);
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	synchronized String dispatchReqCapa() {
		if (!this.io.writeCommand(Command.REQ_CAPA))
			return null;
		return this.io.readString();
	}

	synchronized String dispatchReqVersion() {
		if (!this.io.writeCommand(Command.REQ_VERSION))
			return null;
		return this.io.readString();
	}

	synchronized int dispatchCreateObject(ObjectType ot) {
		if (!this.io.writeCommand(Command.CREATE_OBJECT, ot))
			return -1;
		return this.io.readInt();
	}

	synchronized int dispatchHelloCarsten(int i) throws DispatcherIOException {
		if (!this.io.writeCommand(Command.HELLO_CARSTEN, i))
			throw new DispatcherProtocolException("HELLO_CARSTEN failed");
		return this.io.readInt();
	}

	public void sendNoOp() throws DispatcherIOException {
		if (dispatchHelloCarsten(0) != 0)
			throw new DispatcherProtocolException("HELLO_CARSTEN failed");
	}

	public BufferedLogger createLogger() {
		return new BufferedLogger(this);
	}

	public Knowledgebase createKnowledgebase() {
		return new Knowledgebase(this);
	}
}
