package de.libalf.dispatcher;

import java.util.LinkedList;

import de.libalf.Knowledgebase;

public class DispatcherKnowledgebase extends DispatcherObject implements Knowledgebase {
	public DispatcherKnowledgebase(DispatcherFactory factory) throws DispatcherException {
		super(factory, DispatcherConstants.OBJ_KNOWLEDGEBASE);
	}

	@Override
	public boolean add_knowledge(int[] word, boolean acceptance) throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseAddKnowledge(this, word, acceptance);
	}

	@Override
	public void clear() throws DispatcherException {
		this.factory.dispatchObjectCommandKnowledgebaseClear(this);
	}

	@Override
	public void clear_queries() throws DispatcherException {
		this.factory.dispatchObjectCommandKnowledgebaseClearQueries(this);
	}

	@Override
	public int count_answers() throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseCountAnswers(this);
	}

	@Override
	public int count_queries() throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseCountQueries(this);
	}

	@Override
	public boolean deserialize(int[] serialization) throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseDeserialize(this, serialization);
	}

	@Override
	public String generate_dotfile() throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseGenerateDotfile(this);
	}

	@Override
	public LinkedList<int[]> get_knowledge() throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseGetKnowledge(this);
	}

	@Override
	public int get_memory_usage() throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseGetMemoryUsage(this);
	}

	@Override
	public LinkedList<int[]> get_queries() throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseGetQueries(this);
	}

	@Override
	public boolean is_answered() throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseIsAnswered(this);
	}

	@Override
	public boolean is_empty() throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseIsEmpty(this);
	}

	@Override
	public Acceptance resolve_or_add_query(int[] word) throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseResolveOrAddQuery(this, word);
	}

	@Override
	public Acceptance resolve_query(int[] word) throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseResolveQuery(this, word);
	}

	@Override
	public int[] serialize() throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseSerialize(this);
	}

	@Override
	public boolean undo(int count) throws DispatcherException {
		return this.factory.dispatchObjectCommandKnowledgebaseUndo(this, count);
	}
}
