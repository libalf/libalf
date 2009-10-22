/*
 * This file is part of libalf-demo.
 *
 * libalf-demo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf-demo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf-demo.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009 Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

package de.libalf.demo;

import java.io.Serializable;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 *
 */
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