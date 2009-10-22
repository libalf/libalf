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

package de.libalf.demo.gui;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;

import javax.swing.table.AbstractTableModel;

import de.libalf.demo.Scenario;
import de.libalf.demo.Tools;
import de.libalf.demo.filter.Filter;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 *
 */
public class MembershipQueryTableModel extends AbstractTableModel {

	public enum Result {
		ACCEPT, REJECT, UNKNOWN, CONFLICT;
	}

	private static final long serialVersionUID = 1L;

	private int userQueries = 0, filteredQueries = 0;

	private ArrayList<Entry> queries;

	private Scenario scenario;

	public MembershipQueryTableModel(Scenario scenario,
			LinkedList<int[]> queries) {
		this.scenario = scenario;
		this.queries = new ArrayList<Entry>(queries.size());

		// Here we need to filter
		for (int[] word : queries) {
			Result r = filter(word);

			boolean tmp = false;
			switch (r) {
			case ACCEPT:
				filteredQueries++;
				tmp = true;
				break;
			case REJECT:
				filteredQueries++;
				tmp = false;
				break;
			default:
				userQueries++;
				if (scenario.isUseTeacher() && scenario.getTeacher() != null) {
					tmp = scenario.getTeacher().run(
							Tools.libALFWord2String(word));
				}
				break;
			}

			this.queries.add(new Entry(word, tmp, r));
		}
	}

	@SuppressWarnings("unchecked")
	public Class getColumnClass(int c) {
		return getValueAt(0, c).getClass();
	}

	@Override
	public String getColumnName(int column) {
		switch (column) {
		case 0:
			return "Query";
		case 1:
			return "Answer";
		default:
			return "Unknown";
		}
	}

	@Override
	public int getColumnCount() {
		return 2;
	}

	@Override
	public int getRowCount() {
		return queries.size();
	}

	@Override
	public Object getValueAt(int arg0, int arg1) {
		if (arg1 == 0) {
			return convertWordToString(queries.get(arg0).word);
		} else if (arg1 == 1) {
			return queries.get(arg0).value;
		} else if (arg1 == 2) {
			return queries.get(arg0).filterResult;
		} else
			return "n/a";

	}

	@Override
	public void setValueAt(Object value, int rowIndex, int columnIndex) {
		queries.get(rowIndex).value = (Boolean) value;
	}

	@Override
	public boolean isCellEditable(int rowIndex, int columnIndex) {
		if (columnIndex == 1)
			if (queries.get(rowIndex).filterResult.equals(Result.ACCEPT)
					|| queries.get(rowIndex).filterResult.equals(Result.REJECT))
				return false;
			else
				return true;
		else {
			return false;
		}
	}

	public HashMap<int[], Boolean> getAnswers() {
		HashMap<int[], Boolean> answers = new HashMap<int[], Boolean>(queries
				.size());
		for (Entry e : queries)
			answers.put(e.word, e.value);
		return answers;
	}

	private static String convertWordToString(int[] word) {
		if (word == null)
			return "";
		if (word.length == 0)
			return "Empty word";
		String ret = "";
		for (int i = 0; i < word.length; i++)
			ret += word[i] + (i == word.length - 1 ? "" : " ");
		return ret;
	}

	private Result filter(int[] input) {
		Filter.Result r = Filter.Result.UNKNOWN;

		for (Filter f : scenario.getFilters()) {
			Filter.Result tmpResult = f.filter(input);
			if (!tmpResult.equals(Filter.Result.UNKNOWN)) {
				if (r.equals(Filter.Result.UNKNOWN))
					r = tmpResult;
				else {
					if (!r.equals(tmpResult))
						return Result.CONFLICT;
				}
			}
		}

		switch (r) {
		case ACCEPT:
			return Result.ACCEPT;
		case REJECT:
			return Result.REJECT;
		case UNKNOWN:
			return Result.UNKNOWN;
		default:
			return Result.UNKNOWN;
		}
	}

	public Result getResult(int row) {
		return queries.get(row).filterResult;
	}

	public int numberOfUserQueries() {
		return userQueries;
	}

	public int numberOfFilteredQueries() {
		return filteredQueries;
	}

	class Entry {

		int[] word;
		Boolean value;
		Result filterResult;

		public Entry(int[] word, Boolean value, Result filterResult) {
			super();
			this.word = word;
			this.value = value;
			this.filterResult = filterResult;
		}
	}
}