package de.libalf.dispatcher;

import java.io.IOException;
import java.net.Socket;
import java.util.LinkedList;

import de.libalf.LibALFFactory;
import de.libalf.Knowledgebase.Acceptance;

public class DispatcherFactory implements LibALFFactory {
	DispatcherSocket io;

	public DispatcherFactory(String host, int port) throws DispatcherIOException, DispatcherProtocolException {
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

	////////////////////////////////////////////////////////////////
	// COMMUNICATION
	// has to be synchronized in order to not interleave requests of concurrent threads.

	synchronized String dispatchReqCapa() {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_REQ_CAPA);
		return this.io.readString();
	}

	synchronized String dispatchReqVersion() {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_REQ_VERSION);
		return this.io.readString();
	}

	synchronized int dispatchCreateObject(DispatcherConstants objType) {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_CREATE_OBJECT, objType);
		return this.io.readInt();
	}

	synchronized void dispatchDeleteObject(DispatcherObject obj) {
		this.io.writeCommand(DispatcherConstants.CLCMD_DELETE_OBJECT, obj);
	}

	synchronized void dispatchHelloCarsten(int i) throws DispatcherIOException {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_HELLO_CARSTEN, i);
		if (this.io.readInt() != i)
			throw DispatcherConstants.CLCMD_HELLO_CARSTEN.getException();
	}

	private void dispatchObjectCommandThrowing(DispatcherObject obj, DispatcherConstants objCmd, Object... args) {
		Object[] newArgs = new Object[args.length + 2];
		newArgs[0] = obj;
		newArgs[1] = objCmd;
		System.arraycopy(args, 0, newArgs, 2, args.length);

		try {
			this.io.writeCommandThrowing(DispatcherConstants.CLCMD_OBJECT_COMMAND, newArgs);
		} catch (DispatcherProtocolException e) {
			throw objCmd.getException(e);
		}
	}

	private boolean dispatchObjectCommand(DispatcherObject obj, DispatcherConstants objCmd, Object... args) {
		try {
			dispatchObjectCommandThrowing(obj, objCmd, args);
		} catch (DispatcherProtocolException e) {
			return false;
		}
		return true;
	}

	synchronized String dispatchObjectCommandLoggerReceiveAndFlush(DispatcherObject obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.LOGGER_RECEIVE_AND_FLUSH);
		return this.io.readString();
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseAddKnowledge(DispatcherKnowledgebase obj, int[] word, boolean acceptance) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_ADD_KNOWLEDGE, word, acceptance);
		return this.io.readBool();
	}

	synchronized void dispatchObjectCommandKnowledgebaseClear(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_CLEAR);
	}

	synchronized void dispatchObjectCommandKnowledgebaseClearQueries(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_CLEAR_QUERIES);
	}

	synchronized int dispatchObjectCommandKnowledgebaseCountAnswers(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_COUNT_ANSWERS);
		return this.io.readInt();
	}

	synchronized int dispatchObjectCommandKnowledgebaseCountQueries(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_COUNT_QUERIES);
		return this.io.readInt();
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseDeserialize(DispatcherKnowledgebase obj, int[] serialization) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_DESERIALIZE, serialization);
		return this.io.readBool();
	}

	synchronized String dispatchObjectCommandKnowledgebaseGenerateDotfile(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_TO_DOTFILE);
		return this.io.readString();
	}

	synchronized DispatcherKnowledgebaseIterator dispatchObjectCommandKnowledgebaseBegin(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_BEGIN);
		return new DispatcherKnowledgebaseIterator(this, this.io.readInt());
	}

	synchronized DispatcherKnowledgebaseIterator dispatchObjectCommandKnowledgebaseEnd(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_END);
		return new DispatcherKnowledgebaseIterator(this, this.io.readInt());
	}

	private LinkedList<int[]> iterateToList(DispatcherKnowledgebaseIterator begin, DispatcherKnowledgebaseIterator end) {
		LinkedList<int[]> list = new LinkedList<int[]>();
		while (!dispatchObjectCommandKIteratorCompare(begin, end)) {
			list.add(dispatchObjectCommandKIteratorGetWord(begin));
			dispatchObjectCommandKIteratorNext(begin);
		}
		return list;
	}

	synchronized LinkedList<int[]> dispatchObjectCommandKnowledgebaseGetKnowledge(DispatcherKnowledgebase obj) {
		return iterateToList(dispatchObjectCommandKnowledgebaseBegin(obj), dispatchObjectCommandKnowledgebaseEnd(obj));
	}

	synchronized int dispatchObjectCommandKnowledgebaseGetMemoryUsage(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_GET_MEMORY_USAGE);
		return this.io.readInt();
	}

	synchronized DispatcherKnowledgebaseIterator dispatchObjectCommandKnowledgebaseQBegin(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_QBEGIN);
		return new DispatcherKnowledgebaseIterator(this, this.io.readInt());
	}

	synchronized DispatcherKnowledgebaseIterator dispatchObjectCommandKnowledgebaseQEnd(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_QEND);
		return new DispatcherKnowledgebaseIterator(this, this.io.readInt());
	}

	synchronized LinkedList<int[]> dispatchObjectCommandKnowledgebaseGetQueries(DispatcherKnowledgebase obj) {
		return iterateToList(dispatchObjectCommandKnowledgebaseQBegin(obj), dispatchObjectCommandKnowledgebaseQEnd(obj));
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseIsAnswered(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_IS_ANSWERED);
		return this.io.readBool();
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseIsEmpty(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_IS_EMPTY);
		return this.io.readBool();
	}

	synchronized Acceptance dispatchObjectCommandKnowledgebaseResolveOrAddQuery(DispatcherKnowledgebase obj, int[] word) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_RESOLVE_OR_ADD_QUERY, word);
		return this.io.readAcceptance();
	}

	synchronized Acceptance dispatchObjectCommandKnowledgebaseResolveQuery(DispatcherKnowledgebase obj, int[] word) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_RESOLVE_QUERY, word);
		return this.io.readAcceptance();
	}

	synchronized int[] dispatchObjectCommandKnowledgebaseSerialize(DispatcherKnowledgebase obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_SERIALIZE);
		return this.io.readInts();
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseUndo(DispatcherKnowledgebase obj, int count) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_UNDO, count);
		return this.io.readBool();
	}

	synchronized boolean dispatchObjectCommandKIteratorCompare(DispatcherKnowledgebaseIterator obj1, DispatcherKnowledgebaseIterator obj2) {
		dispatchObjectCommandThrowing(obj1, DispatcherConstants.KITERATOR_COMPARE, obj2);
		return this.io.readBool();
	}

	synchronized void dispatchObjectCommandKIteratorNext(DispatcherKnowledgebaseIterator obj) {
		dispatchObjectCommandThrowing(obj, DispatcherConstants.KITERATOR_NEXT);
	}

	synchronized int[] dispatchObjectCommandKIteratorGetWord(DispatcherKnowledgebaseIterator obj) {
		return dispatchObjectCommand(obj, DispatcherConstants.KITERATOR_GET_WORD) ? this.io.readInts() : null;
	}

	////////////////////////////////////////////////////////////////
	// BASIC COMMANDS

	public void sendNoOp() throws DispatcherIOException {
		dispatchHelloCarsten(0);
	}

	@Override
	public DispatcherLogger createLogger() {
		return new DispatcherLogger(this);
	}

	@Override
	public DispatcherKnowledgebase createKnowledgebase() {
		return new DispatcherKnowledgebase(this);
	}

	@Override
	public DispatcherLearningAlgorithm createLearningAlgorithm(Algorithm algorithm, Object[] parameter) {
		return null;
	}
}
