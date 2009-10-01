package de.libalf.dispatcher;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import de.libalf.AlfException;
import de.libalf.BasicAutomaton;
import de.libalf.BasicTransition;
import de.libalf.Conjecture;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.Logger;

public abstract class DispatcherLearningAlgorithm extends DispatcherObject implements LearningAlgorithm {
	private static final long serialVersionUID = 1L;

	private DispatcherLogger logger;
	private DispatcherKnowledgebase base;

	private DispatcherConstants algo;

	protected DispatcherLearningAlgorithm(DispatcherFactory factory, DispatcherConstants algo, int alphabet_size) throws AlfException {
		super(factory, DispatcherConstants.OBJ_LEARNING_ALGORITHM);
		this.algo = algo;
		create(alphabet_size);
	}

	private void create(int alphabet_size) {
		create(new int[] { this.algo.id, alphabet_size });
	}

	protected DispatcherLearningAlgorithm(DispatcherFactory factory, DispatcherConstants algo, Knowledgebase base, int alphabet_size, Logger logger) throws AlfException {
		this(factory, algo, alphabet_size);
		if (base != null)
			set_knowledge_source(base);
		if (logger != null)
			set_logger(logger);
	}

	@Override
	public void add_counterexample(int[] counterexample) throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_ADD_COUNTEREXAMPLE, counterexample);
		}
	}

	@Override
	public Conjecture advance() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_ADVANCE);
			return this.factory.readBool() ? getConjecture(this.factory.readInt(), this.factory.readInts()) : null;
		}
	}

	private/* static */Conjecture getConjecture(int conType, int[] ints) {
		DispatcherConstants conjType = DispatcherConstants.getConjectureType(conType);
		switch (conjType) {
		case CONJECTURE_NONE: // FIXME
		case CONJECTURE_SIMPLE_AUTOMATON:
			return getBA(ints);
			// FIXME:case CONJECTURE_NONE:
		case CONJECTURE_LAST_INVALID:
			throw new DispatcherProtocolException(String.format("Conjecture type %d (0x%08X) unsupported!", conType, conType));
		default:
			throw new DispatcherProtocolException(String.format("Conjecture type " + conjType + " (%d, 0x%08X) unknown!", conType, conType));
		}
	}

	private static BasicAutomaton getBA(int[] ints) {
		int pos = 0;

		int isDet = ints[pos++];
		assert isDet >= 0;
		assert isDet <= 1;
		int alphabetSize = ints[pos++];
		assert alphabetSize > 0;
		int numberOfStates = ints[pos++];
		assert numberOfStates >= 0;
		BasicAutomaton auto = new BasicAutomaton(numberOfStates, alphabetSize);
		int numberOfInitStates = ints[pos++];
		assert numberOfInitStates >= 0;
		assert isDet != 1 || numberOfInitStates <= 1;
		while (numberOfInitStates-- > 0) {
			int state = ints[pos++];
			assert state >= 0;
			assert state < numberOfStates;
			auto.addInitialState(state);
		}
		int numberOfFinalStates = ints[pos++];
		assert numberOfFinalStates >= 0;
		while (numberOfFinalStates-- > 0) {
			int state = ints[pos++];
			assert state >= 0;
			assert state < numberOfStates;
			auto.addFinalState(state);
		}
		int numberOfTransitions = ints[pos++];
		assert numberOfTransitions >= 0;
		while (numberOfTransitions-- > 0) {
			int source = ints[pos++];
			assert source >= 0;
			assert source < numberOfStates;
			int label = ints[pos++];
			assert label >= 0;
			assert label < alphabetSize;
			int destination = ints[pos++];
			assert destination >= 0;
			assert destination < numberOfStates;
			auto.addTransition(new BasicTransition(source, label, destination));
		}

		return auto;
	}

	@Override
	public boolean conjecture_ready() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_CONJECTURE_READY);
			return this.factory.readBool();
		}
	}

	@Override
	public boolean deserialize(int[] serialization) throws AlfException {
		synchronized (this.factory) {
			try {
				this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_DESERIALIZE, serialization);
				return true;
			} catch (DispatcherCommandError e) {
				return false;
			}
		}
	}

	@Override
	public int get_alphabet_size() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_GET_ALPHABET_SIZE);
			return this.factory.readInt();
		}
	}

	@Override
	public DispatcherKnowledgebase get_knowledge_source() throws AlfException {
		synchronized (this.factory) {
			if (this.base != null) {
				this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_GET_KNOWLEDGE_SOURCE);
				if (this.base.getInt() != this.factory.readInt())
					throw new DispatcherProtocolException("Knowledgebase changed");
			}
			return this.base;
		}
	}

	@Override
	public void increase_alphabet_size(int new_size) throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_INCREASE_ALPHABET_SIZE, new_size);
		}
	}

	@Override
	public int[] serialize() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_SERIALIZE);
			return this.factory.readInts();
		}
	}

	@Override
	public void set_alphabet_size(int alphabet_size) throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_INCREASE_ALPHABET_SIZE, alphabet_size); // FIXME: SET not INCR
		}
	}

	@Override
	public void set_knowledge_source(Knowledgebase base) throws AlfException {
		synchronized (this.factory) {
			checkFactory(base);
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_SET_KNOWLEDGE_SOURCE, ((DispatcherKnowledgebase) base).getInt());
			this.base = (DispatcherKnowledgebase) base; // set if above command was successful
		}
	}

	@Override
	public void set_logger(Logger logger) throws AlfException {
		synchronized (this.factory) {
			checkFactory(logger);
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_ASSOCIATE_LOGGER, ((DispatcherLogger) logger));
			this.logger = (DispatcherLogger) logger; // set if above command was successful
		}
	}

	public void remove_logger() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_REMOVE_LOGGER);
			this.logger = null; // set if above command was successful
		}
	}

	@Override
	public boolean supports_sync() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_SUPPORTS_SYNC);
			return this.factory.readBool();
		}
	}

	@Override
	public boolean sync_to_knowledgebase() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_SYNC_TO_KNOWLEDGEBASE);
			return false; // FIXME: return something
		}
	}

	@Override
	public int[] deserialize_magic(int[] data) {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_DESERIALIZE_MAGIC, data);
			return this.factory.readInts();
		}
	}

	@Override
	public String toString() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_TO_STRING);
			return this.factory.readString();
		}
	}

	/**
	 * @see Serializable
	 */
	private void writeObject(ObjectOutputStream out) throws IOException {
		out.defaultWriteObject();
		out.writeObject(get_alphabet_size());
		out.writeObject(serialize());
	}

	/**
	 * @see Serializable
	 */
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		create((Integer) in.readObject());
		deserialize((int[]) in.readObject());
		if (this.logger != null)
			set_logger(this.logger);
		if (this.base != null)
			set_knowledge_source(this.base);
	}
}
