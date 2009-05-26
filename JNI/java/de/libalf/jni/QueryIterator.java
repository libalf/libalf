package de.libalf.jni;

public class QueryIterator extends LibALFObject {

	private Knowledgebase base;

	public QueryIterator(Knowledgebase base) {
		this.base = base;
		this.pointer = init(base.getPointer());
	}

	private native long init(long knowledgebasePointer);

	public int[] nextQuery() {
		return nextQuery(this.pointer);
	}

	private native int[] nextQuery(long pointer);

	public boolean answer(boolean answer) {
		return answer(answer, this.pointer);
	}

	private native boolean answer(boolean answer, long pointer);
}