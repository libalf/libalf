package de.libalf.dispatcher;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import de.libalf.AlfException;
import de.libalf.BasicAutomaton;
import de.libalf.BasicTransition;
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
	public BasicAutomaton advance() throws AlfException {
		synchronized (this.factory) {
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_ADVANCE);
			return this.factory.readBool() ? getBA(this.factory.readInts()) : null;
		}
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
			// TODO Auto-generated method stub
			this.factory.writeObjectCommandThrowing(this, DispatcherConstants.LEARNING_ALGORITHM_SYNC_TO_KNOWLEDGEBASE);
			return false;
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
