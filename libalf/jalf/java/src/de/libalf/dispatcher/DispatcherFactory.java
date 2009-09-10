package de.libalf.dispatcher;

import java.io.IOException;
import java.net.Socket;
import java.util.LinkedList;

import de.libalf.BasicAutomaton;
import de.libalf.LibALFFactory;
import de.libalf.Logger;
import de.libalf.Knowledgebase.Acceptance;

public class DispatcherFactory implements LibALFFactory {
	DispatcherSocket io;

	public DispatcherFactory(String host, int port) throws DispatcherIOException, DispatcherProtocolException {
		try {
			// socket
			this.io = new DispatcherSocket(new Socket(host, port));
			//this.io = new DispatcherSocket(Runtime.getRuntime().exec(new String[]{"nc",host, port+""}));

			// get init stuff
			int code = this.io.readInt();
			if (code != 0)
				throw new DispatcherProtocolException("Connection failed: " + DispatcherConstants.getErrorString(code));
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

	synchronized int dispatchDisconnect() {
		return this.io.writeCommand(DispatcherConstants.CLCMD_DISCONNECT);
	}

	synchronized int dispatchCreateObject(DispatcherConstants objType, int[] data) {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_CREATE_OBJECT, objType, data);
		return this.io.readInt();
	}

	synchronized void dispatchDeleteObject(DispatcherObject obj) {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_DELETE_OBJECT, obj); // TODO
	}

	synchronized int dispatchGetObjectType(DispatcherObject obj) {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_GET_OBJECTTYPE, obj);
		return this.io.readInt();
	}

	synchronized void dispatchHelloCarsten(int i) throws DispatcherIOException {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_HELLO_CARSTEN, i);
		if (this.io.readInt() != i)
			throw DispatcherConstants.CLCMD_HELLO_CARSTEN.getException();
	}

	synchronized String dispatchObjectCommandLoggerReceiveAndFlush(DispatcherObject obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LOGGER_RECEIVE_AND_FLUSH);
		return this.io.readString();
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseAddKnowledge(DispatcherKnowledgebase obj, int[] word, boolean acceptance) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_ADD_KNOWLEDGE, word, acceptance);
		return this.io.readBool();
	}

	synchronized void dispatchObjectCommandKnowledgebaseClear(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_CLEAR);
	}

	synchronized void dispatchObjectCommandKnowledgebaseClearQueries(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_CLEAR_QUERIES);
	}

	synchronized int dispatchObjectCommandKnowledgebaseCountAnswers(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_COUNT_ANSWERS);
		return this.io.readInt();
	}

	synchronized int dispatchObjectCommandKnowledgebaseCountQueries(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_COUNT_QUERIES);
		return this.io.readInt();
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseDeserialize(DispatcherKnowledgebase obj, int[] serialization) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_DESERIALIZE, serialization);
		return this.io.readBool();
	}

	synchronized String dispatchObjectCommandKnowledgebaseGenerateDotfile(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_TO_DOTFILE);
		return this.io.readString();
	}

	private DispatcherKnowledgebaseIterator dispatchObjectCommandKnowledgebaseBegin(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_BEGIN);
		return new DispatcherKnowledgebaseIterator(this, this.io.readInt());
	}

	private DispatcherKnowledgebaseIterator dispatchObjectCommandKnowledgebaseEnd(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_END);
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
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_GET_MEMORY_USAGE);
		return this.io.readInt();
	}

	private DispatcherKnowledgebaseIterator dispatchObjectCommandKnowledgebaseQBegin(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_QBEGIN);
		return new DispatcherKnowledgebaseIterator(this, this.io.readInt());
	}

	private DispatcherKnowledgebaseIterator dispatchObjectCommandKnowledgebaseQEnd(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_QEND);
		return new DispatcherKnowledgebaseIterator(this, this.io.readInt());
	}

	synchronized LinkedList<int[]> dispatchObjectCommandKnowledgebaseGetQueries(DispatcherKnowledgebase obj) {
		return iterateToList(dispatchObjectCommandKnowledgebaseQBegin(obj), dispatchObjectCommandKnowledgebaseQEnd(obj));
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseIsAnswered(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_IS_ANSWERED);
		return this.io.readBool();
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseIsEmpty(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_IS_EMPTY);
		return this.io.readBool();
	}

	synchronized Acceptance dispatchObjectCommandKnowledgebaseResolveOrAddQuery(DispatcherKnowledgebase obj, int[] word) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_RESOLVE_OR_ADD_QUERY, word);
		return this.io.readAcceptance();
	}

	synchronized Acceptance dispatchObjectCommandKnowledgebaseResolveQuery(DispatcherKnowledgebase obj, int[] word) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_RESOLVE_QUERY, word);
		return this.io.readAcceptance();
	}

	synchronized int[] dispatchObjectCommandKnowledgebaseSerialize(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_SERIALIZE);
		return this.io.readInts();
	}

	synchronized boolean dispatchObjectCommandKnowledgebaseUndo(DispatcherKnowledgebase obj, int count) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_UNDO, count);
		return this.io.readBool();
	}

	synchronized boolean dispatchObjectCommandKIteratorCompare(DispatcherKnowledgebaseIterator obj1, DispatcherKnowledgebaseIterator obj2) {
		this.io.writeObjectCommandThrowing(obj1, DispatcherConstants.KITERATOR_COMPARE, obj2);
		return this.io.readBool();
	}

	synchronized void dispatchObjectCommandKIteratorNext(DispatcherKnowledgebaseIterator obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KITERATOR_NEXT);
	}

	synchronized int[] dispatchObjectCommandKIteratorGetWord(DispatcherKnowledgebaseIterator obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KITERATOR_GET_WORD);
		return this.io.readInts();
	}

	synchronized void dispatchObjectCommandAlgorithmAddCounterexample(DispatcherLearningAlgorithm obj, int[] counterexample) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_ADD_COUNTEREXAMPLE, counterexample);
	}

	synchronized BasicAutomaton dispatchObjectCommandAlgorithmAdvance(DispatcherLearningAlgorithm obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_ADVANCE);
		if (!this.io.readBool())
			return null;

		// TODO basic automata
		return null;
	}

	synchronized boolean dispatchObjectCommandAlgorithmConjectureReady(DispatcherLearningAlgorithm obj) {
		// TODO Auto-generated method stub
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_CONJECTURE_READY);
		return false;
	}

	synchronized boolean dispatchObjectCommandAlgorithmDeserialize(DispatcherLearningAlgorithm obj, int[] serialization) {
		// TODO Auto-generated method stub
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_DESERIALIZE, serialization);
		return false;
	}

	synchronized int dispatchObjectCommandAlgorithmGetAlphabetSize(DispatcherLearningAlgorithm obj) {
		// TODO Auto-generated method stub
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_GET_ALPHABET_SIZE);
		return 0;
	}

	synchronized int dispatchObjectCommandAlgorithmGetKnowledgeSource(DispatcherLearningAlgorithm obj) {
		// TODO Auto-generated method stub
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_GET_KNOWLEDGE_SOURCE);
		return 0;
	}

	synchronized void dispatchObjectCommandAlgorithmIncreaseAlphabetSize(DispatcherLearningAlgorithm obj, int new_size) {
		// TODO Auto-generated method stub
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_INCREASE_ALPHABET_SIZE, new_size);
	}

	synchronized int[] dispatchObjectCommandAlgorithmSerialize(DispatcherLearningAlgorithm obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_SERIALIZE);
		return this.io.readInts();
	}

	synchronized void dispatchObjectCommandAlgorithmSetAlphabetSize(DispatcherLearningAlgorithm obj, int alphabet_size) {
	// TODO Auto-generated method stub
	//		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_);
	}

	synchronized void dispatchObjectCommandAlgorithmSetKnowledgeSource(DispatcherLearningAlgorithm obj, DispatcherKnowledgebase base) {
		// TODO Auto-generated method stub
		obj.checkFactory(base);
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_SET_KNOWLEDGE_SOURCE, base);
	}

	synchronized void dispatchObjectCommandAlgorithmSetLogger(DispatcherLearningAlgorithm obj, DispatcherLogger logger) {
	// TODO Auto-generated method stub
			this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_ASSOCIATE_LOGGER, logger);
	}

	synchronized boolean dispatchObjectCommandAlgorithmSupportsSync(DispatcherLearningAlgorithm obj) {
		// TODO Auto-generated method stub
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_SUPPORTS_SYNC);
		return false;
	}

	synchronized boolean dispatchObjectCommandAlgorithmSyncToKnowledgebase(DispatcherLearningAlgorithm obj) {
		// TODO Auto-generated method stub
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_SYNC_TO_KNOWLEDGEBASE);
		return false;
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
	public DispatcherLearningAlgorithm createLearningAlgorithm(Algorithm algorithm, Object... args) {
		switch (algorithm) {
		case ANGLUIN:
			return new DispatcherAlgorithmAngluin(this, (DispatcherKnowledgebase) args[0], (Integer) args[1]);
		case ANGLUIN_COLUMN:
			return new DispatcherAlgorithmAngluinColumn(this, (DispatcherKnowledgebase) args[0], (Integer) args[1]);
		case NL_STAR:
			return new DispatcherAlgorithmNLstar(this, (DispatcherKnowledgebase) args[0], (Integer) args[1]);
		case RPNI:
			return new DispatcherAlgorithmRPNI(this, (DispatcherKnowledgebase) args[0], (Integer) args[1]);
		case BIERMANN_MINISAT:
			return new DispatcherAlgorithmBiermannMiniSAT(this, (DispatcherKnowledgebase) args[0], (Integer) args[1]);
		default:
			return null;
		}
	}
}
