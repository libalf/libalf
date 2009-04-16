package de.libalf.jni;
import java.util.LinkedList;

public class WordList extends LinkedList<int[]> {

	/**
	 * Serial ID
	 */
	private static final long serialVersionUID = 372504528428312887L;

	public void jniAdd(int[] arrayToAdd) {
		this.add(arrayToAdd);
	}

	public String toString() {
		String ret = "[";
		for (int j=0; j<this.size(); j++) {
			int[] arr = this.get(j);
			ret += "[";
			for (int i = 0; i < arr.length; i++)
				ret += arr[i] + (i == arr.length - 1 ? "" : ", ");
			ret += "]";
			ret += (j==this.size()-1) ? "" : ", ";
		}
		ret += "]";
		return ret;
	}
}
