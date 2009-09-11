package de.libalf.dispatcher;

import de.libalf.AlfException;
import de.libalf.BasicAutomaton;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.Logger;

public abstract class DispatcherLearningAlgorithm extends DispatcherObject implements LearningAlgorithm {
	private DispatcherLogger logger;
	private DispatcherKnowledgebase base;

	protected DispatcherLearningAlgorithm(DispatcherFactory factory, DispatcherConstants algo, int alphabet_size) throws AlfException {
		super(factory, DispatcherConstants.OBJ_LEARNING_ALGORITHM, new int[] { algo.id, alphabet_size });
	}

	protected DispatcherLearningAlgorithm(DispatcherFactory factory, DispatcherConstants algo, int alphabet_size, DispatcherKnowledgebase base) throws AlfException {
		this(factory, algo, alphabet_size);
		if (base != null)
			set_knowledge_source(base);
	}

	@Override
	public void add_counterexample(int[] counterexample) throws AlfException {
		this.factory.dispatchObjectCommandAlgorithmAddCounterexample(this, counterexample);
	}

	@Override
	public BasicAutomaton advance() throws AlfException {
		return this.factory.dispatchObjectCommandAlgorithmAdvance(this);
	}

	@Override
	public boolean conjecture_ready() throws AlfException {
		return this.factory.dispatchObjectCommandAlgorithmConjectureReady(this);
	}

	@Override
	public boolean deserialize(int[] serialization) throws AlfException {
		return this.factory.dispatchObjectCommandAlgorithmDeserialize(this, serialization);
	}

	@Override
	public int get_alphabet_size() throws AlfException {
		return this.factory.dispatchObjectCommandAlgorithmGetAlphabetSize(this);
	}

	@Override
	public DispatcherKnowledgebase get_knowledge_source() throws AlfException {
		if (this.base != null)
			if (this.base.getInt() != this.factory.dispatchObjectCommandAlgorithmGetKnowledgeSource(this))
				throw new DispatcherProtocolException("knowledgebase changed");
		return this.base;
	}

	@Override
	public void increase_alphabet_size(int new_size) throws AlfException {
		this.factory.dispatchObjectCommandAlgorithmIncreaseAlphabetSize(this, new_size);
	}

	@Override
	public int[] serialize() throws AlfException {
		return this.factory.dispatchObjectCommandAlgorithmSerialize(this);
	}

	@Override
	public void set_alphabet_size(int alphabet_size) throws AlfException {
		this.factory.dispatchObjectCommandAlgorithmSetAlphabetSize(this, alphabet_size);
	}

	@Override
	public void set_knowledge_source(Knowledgebase base) throws AlfException {
		checkFactory(base);
		this.factory.dispatchObjectCommandAlgorithmSetKnowledgeSource(this, (DispatcherKnowledgebase) base);
		this.base = (DispatcherKnowledgebase) base; // set if above command was successful
	}

	@Override
	public void set_logger(Logger logger) throws AlfException {
		checkFactory(logger);
		this.factory.dispatchObjectCommandAlgorithmSetLogger(this, (DispatcherLogger) logger);
		this.logger = (DispatcherLogger) logger; // set if above command was successful
	}

	@Override
	public boolean supports_sync() throws AlfException {
		return this.factory.dispatchObjectCommandAlgorithmSupportsSync(this);
	}

	@Override
	public boolean sync_to_knowledgebase() throws AlfException {
		return this.factory.dispatchObjectCommandAlgorithmSyncToKnowledgebase(this);
	}

	@Override
	public String toString() throws AlfException {
		return this.factory.dispatchObjectCommandAlgorithmToString(this);
	}
}
