package de.libalf.dispatcher;

import de.libalf.BasicAutomaton;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.Logger;
import de.libalf.LibALFFactory.Algorithm;

public abstract class DispatcherLearningAlgorithm extends DispatcherObject implements LearningAlgorithm {
	private DispatcherLogger logger;
	private DispatcherKnowledgebase base;

	protected DispatcherLearningAlgorithm(DispatcherFactory factory, DispatcherConstants algo, int alphabet_size) throws DispatcherException {
		super(factory, DispatcherConstants.OBJ_LEARNING_ALGORITHM, new int[] { algo.id, alphabet_size });
	}

	@Override
	public void add_counterexample(int[] counterexample) throws DispatcherException {
		this.factory.dispatchObjectCommandAlgorithmAddCounterexample(this, counterexample);
	}

	@Override
	public BasicAutomaton advance() throws DispatcherException {
		return this.factory.dispatchObjectCommandAlgorithmAdvance(this);
	}

	@Override
	public boolean conjecture_ready() throws DispatcherException {
		return this.factory.dispatchObjectCommandAlgorithmConjectureReady(this);
	}

	@Override
	public boolean deserialize(int[] serialization) throws DispatcherException {
		return this.factory.dispatchObjectCommandAlgorithmDeserialize(this, serialization);
	}

	@Override
	public int get_alphabet_size() throws DispatcherException {
		return this.factory.dispatchObjectCommandAlgorithmGetAlphabetSize(this);
	}

	@Override
	public DispatcherKnowledgebase get_knowledge_source() throws DispatcherException {
		if (this.base.getInt() != this.factory.dispatchObjectCommandAlgorithmGetKnowledgeSource(this))
			throw new DispatcherProtocolException("knowledgebase changed");
		return this.base;
	}

	@Override
	public void increase_alphabet_size(int new_size) throws DispatcherException {
		this.factory.dispatchObjectCommandAlgorithmIncreaseAlphabetSize(this, new_size);
	}

	@Override
	public int[] serialize() throws DispatcherException {
		return this.factory.dispatchObjectCommandAlgorithmSerialize(this);
	}

	@Override
	public void set_alphabet_size(int alphabet_size) throws DispatcherException {
		this.factory.dispatchObjectCommandAlgorithmSetAlphabetSize(this, alphabet_size);
	}

	@Override
	public void set_knowledge_source(Knowledgebase base) throws DispatcherException {
		checkFactory(base);
		this.factory.dispatchObjectCommandAlgorithmSetKnowledgeSource(this, this.base = (DispatcherKnowledgebase) base);
	}

	@Override
	public void set_logger(Logger logger) throws DispatcherException {
		checkFactory(logger);
		this.factory.dispatchObjectCommandAlgorithmSetLogger(this, this.logger = (DispatcherLogger) logger);
	}

	@Override
	public boolean supports_sync() throws DispatcherException {
		return this.factory.dispatchObjectCommandAlgorithmSupportsSync(this);
	}

	@Override
	public boolean sync_to_knowledgebase() throws DispatcherException {
		return this.factory.dispatchObjectCommandAlgorithmSyncToKnowledgebase(this);
	}
}
