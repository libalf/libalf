package de.libalf.dispatcher;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.util.LinkedList;

import de.libalf.BasicAutomaton;
import de.libalf.BasicTransition;
import de.libalf.LibALFFactory;
import de.libalf.Knowledgebase.Acceptance;

public class DispatcherFactory implements LibALFFactory {
	private static final long serialVersionUID = 1L;

	private DispatcherSocket io;

	public DispatcherFactory(String host, int port) throws DispatcherIOException, DispatcherProtocolException {
		// socket
		this.io = new DispatcherSocket(host, port);

		// get init stuff
		init();
	}

	private void init() {
		int code = this.io.readInt();
		if (code != 0)
			throw new DispatcherCommandError(code, DispatcherConstants.CLCMD_REQ_CAPA);
		String capa = this.io.readString(); // TODO
		System.out.println(capa);
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		init();
	}

	public void kill() {
		int disco = dispatchDisconnect();
		if (disco != 0)
			new DispatcherCommandError(disco, DispatcherConstants.CLCMD_DISCONNECT).printStackTrace();
		this.io.close();
	}

	@Override
	protected void finalize() throws Throwable {
		kill();
		super.finalize();
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
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_DELETE_OBJECT, obj);
	}

	synchronized int dispatchGetObjectType(DispatcherObject obj) {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_GET_OBJECTTYPE, obj);
		return this.io.readInt();
	}

	synchronized void dispatchHelloCarsten(int i) throws DispatcherIOException {
		this.io.writeCommandThrowing(DispatcherConstants.CLCMD_HELLO_CARSTEN, i);
		if (this.io.readInt() != i)
			throw new DispatcherProtocolException(DispatcherConstants.CLCMD_HELLO_CARSTEN.toString());
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
		try {
			this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_DESERIALIZE, serialization);
			return true;
		} catch (DispatcherCommandError e) {
			return false;
		}
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

	synchronized String dispatchObjectCommandKnowledgebaseToString(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_TO_STRING);
		return this.io.readString();
	}

	synchronized String dispatchObjectCommandKnowledgebaseToDotFile(DispatcherKnowledgebase obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.KNOWLEDGEBASE_TO_DOTFILE);
		return this.io.readString();
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
		return this.io.readBool() ? getBA(this.io.readInts()) : null;
	}

	private static BasicAutomaton getBA(int[] ints) {
		int pos = 0;

		int alphabetSize = ints[pos++];
		int numberOfStates = ints[pos++];
		BasicAutomaton auto = new BasicAutomaton(numberOfStates, alphabetSize);
		int numberOfInitStates = ints[pos++];
		while (numberOfInitStates-- > 0)
			auto.addInitialState(ints[pos++]);
		int numberOfFinalStates = ints[pos++];
		while (numberOfFinalStates-- > 0)
			auto.addFinalState(ints[pos++]);
		int numberOfTransitions = ints[pos++];
		while (numberOfTransitions-- > 0)
			auto.addTransition(new BasicTransition(ints[pos++], ints[pos++], ints[pos++]));

		return auto;
	}

	synchronized boolean dispatchObjectCommandAlgorithmConjectureReady(DispatcherLearningAlgorithm obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_CONJECTURE_READY);
		return this.io.readBool();
	}

	synchronized boolean dispatchObjectCommandAlgorithmDeserialize(DispatcherLearningAlgorithm obj, int[] serialization) {
		try {
			this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_DESERIALIZE, serialization);
			return true;
		} catch (DispatcherCommandError e) {
			return false;
		}
	}

	synchronized int dispatchObjectCommandAlgorithmGetAlphabetSize(DispatcherLearningAlgorithm obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_GET_ALPHABET_SIZE);
		return this.io.readInt();
	}

	synchronized int dispatchObjectCommandAlgorithmGetKnowledgeSource(DispatcherLearningAlgorithm obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_GET_KNOWLEDGE_SOURCE);
		return this.io.readInt();
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
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_INCREASE_ALPHABET_SIZE, alphabet_size); // FIXME: SET not INCR
	}

	synchronized void dispatchObjectCommandAlgorithmSetKnowledgeSource(DispatcherLearningAlgorithm obj, int base) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_SET_KNOWLEDGE_SOURCE, base);
	}

	synchronized void dispatchObjectCommandAlgorithmSetLogger(DispatcherLearningAlgorithm obj, DispatcherLogger logger) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_ASSOCIATE_LOGGER, logger);
	}

	synchronized void dispatchObjectCommandAlgorithmRemoveLogger(DispatcherLearningAlgorithm obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_REMOVE_LOGGER);
	}

	synchronized boolean dispatchObjectCommandAlgorithmSupportsSync(DispatcherLearningAlgorithm obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_SUPPORTS_SYNC);
		return this.io.readBool();
	}

	synchronized boolean dispatchObjectCommandAlgorithmSyncToKnowledgebase(DispatcherLearningAlgorithm obj) {
		// TODO Auto-generated method stub
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_SYNC_TO_KNOWLEDGEBASE);
		return false;
	}

	synchronized String dispatchObjectCommandAlgorithmToString(DispatcherLearningAlgorithm obj) {
		this.io.writeObjectCommandThrowing(obj, DispatcherConstants.LEARNING_ALGORITHM_TO_STRING);
		return this.io.readString();
	}

	////////////////////////////////////////////////////////////////
	// BASIC COMMANDS

	public void sendNoOp() throws DispatcherIOException {
		dispatchHelloCarsten(0);
	}

	@Override
	public DispatcherLogger createLogger(Object... args) {
		return new DispatcherLogger(this); // FIXME: args?!
	}

	@Override
	public DispatcherKnowledgebase createKnowledgebase(Object... args) {
		return new DispatcherKnowledgebase(this); // FIXME: args?!
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

	void printRest(int wait) throws Throwable {
		this.io.printRest(wait);
	}
}
