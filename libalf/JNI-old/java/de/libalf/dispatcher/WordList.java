package de.libalf.dispatcher;

import java.util.LinkedList;

/**
 * <p>
 * A {@link java.util.LinkedList} of words.
 * </p>
 * <p>
 * A <code>WordList</code> is returned by {@link Knowledgebase#getQueries()} and
 * can be used as any other {@link java.util.Collection}.
 * </p>
 * 
 * @author Daniel Neider (<a
 *         href="mailto:neider@automata.rwth-aachen.de">neider@automata.
 *         rwth-aachen.de</a>), Chair of Computer Science 7, RWTH Aachen
 *         University
 * @version 1.0
 * 
 */
public class WordList extends LinkedList<int[]> {

	/**
	 * Serial ID
	 */
	private static final long serialVersionUID = 372504528428312887L;

	/**
	 * Adds a new word to the list. This method is invoked via the JNI
	 * interface.
	 * 
	 * @param arrayToAdd
	 *            the word to add
	 */
	public void jniAdd(int[] arrayToAdd) {
		this.add(arrayToAdd);
	}

	@Override
	public String toString() {
		String ret = "[";
		for (int j = 0; j < this.size(); j++) {
			int[] arr = this.get(j);
			ret += "[";
			for (int i = 0; i < arr.length; i++)
				ret += arr[i] + (i == arr.length - 1 ? "" : ", ");
			ret += "]";
			ret += (j == this.size() - 1) ? "" : ", ";
		}
		ret += "]";
		return ret;
	}
}
