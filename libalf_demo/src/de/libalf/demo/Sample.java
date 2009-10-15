package de.libalf.demo;

import java.io.Serializable;

public class Sample implements Serializable {

	private static final long serialVersionUID = 1L;

	public int[] word;
	public Boolean acceptance;

	public Sample(int[] word, boolean acceptance) {
		super();
		this.word = word;
		this.acceptance = acceptance;
	}

	@Override
	public String toString() {
		String s = new String();

		if (word == null)
			s = "null";
		else {
			s = "[";
			if (word.length == 0)
				s += "]";
			for (int i = 0; i < word.length; i++)
				s += word[i] + (i == word.length - 1 ? "]" : "");
		}

		return "(" + s + ", " + acceptance + ")";
	}

	public static String word2String(int[] word) {
		if (word == null)
			return null;
		else {
			String ret = "";
			for (int i = 0; i < word.length; i++)
				ret += word[i];
			return ret;
		}
	}
}