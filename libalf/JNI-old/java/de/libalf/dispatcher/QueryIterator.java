package de.libalf.dispatcher;

public class QueryIterator extends LibALFObject {

	@SuppressWarnings("unused")
	private Knowledgebase base;

	public QueryIterator(Knowledgebase base) {
		this.base = base;
		this.id = init(base.getID());
	}

	private native long init(long knowledgebasePointer);

	public int[] nextQuery() {
		return nextQuery(this.id);
	}

	private native int[] nextQuery(long pointer);

	public boolean answer(boolean answer) {
		return answer(answer, this.id);
	}

	private native boolean answer(boolean answer, long pointer);
}