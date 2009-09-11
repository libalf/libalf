package de.libalf.dispatcher;

import java.util.LinkedList;

import de.libalf.AlfException;
import de.libalf.Knowledgebase;

public class DispatcherKnowledgebase extends DispatcherObject implements Knowledgebase {
	public DispatcherKnowledgebase(DispatcherFactory factory) throws AlfException {
		super(factory, DispatcherConstants.OBJ_KNOWLEDGEBASE);
	}

	@Override
	public boolean add_knowledge(int[] word, boolean acceptance) throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseAddKnowledge(this, word, acceptance);
	}

	@Override
	public void clear() throws AlfException {
		this.factory.dispatchObjectCommandKnowledgebaseClear(this);
	}

	@Override
	public void clear_queries() throws AlfException {
		this.factory.dispatchObjectCommandKnowledgebaseClearQueries(this);
	}

	@Override
	public int count_answers() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseCountAnswers(this);
	}

	@Override
	public int count_queries() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseCountQueries(this);
	}

	@Override
	public boolean deserialize(int[] serialization) throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseDeserialize(this, serialization);
	}

	@Override
	public String generate_dotfile() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseGenerateDotfile(this);
	}

	@Override
	public LinkedList<int[]> get_knowledge() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseGetKnowledge(this);
	}

	@Override
	public int get_memory_usage() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseGetMemoryUsage(this);
	}

	@Override
	public LinkedList<int[]> get_queries() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseGetQueries(this);
	}

	@Override
	public boolean is_answered() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseIsAnswered(this);
	}

	@Override
	public boolean is_empty() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseIsEmpty(this);
	}

	@Override
	public Acceptance resolve_or_add_query(int[] word) throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseResolveOrAddQuery(this, word);
	}

	@Override
	public Acceptance resolve_query(int[] word) throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseResolveQuery(this, word);
	}

	@Override
	public int[] serialize() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseSerialize(this);
	}

	@Override
	public boolean undo(int count) throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseUndo(this, count);
	}

	@Override
	public String toString() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseToString(this);
	}

	public String toDot() throws AlfException {
		return this.factory.dispatchObjectCommandKnowledgebaseToDotFile(this);
	}
}
