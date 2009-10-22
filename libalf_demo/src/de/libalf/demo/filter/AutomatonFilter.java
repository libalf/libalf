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

package de.libalf.demo.filter;

import de.libalf.demo.Tools;
import dk.brics.automaton.Automaton;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 */
public class AutomatonFilter implements Filter {

	private static final long serialVersionUID = 1L;

	private Automaton acceptAutomaton, rejectAutomaton;

	private String description;

	public static int number = 1;

	public AutomatonFilter(Automaton acceptAutomaton,
			Automaton rejectAutomaton, String description) {
		this.acceptAutomaton = acceptAutomaton;
		this.rejectAutomaton = rejectAutomaton;

		
		this.description = description;
	}

	@Override
	public Result filter(int[] input) {
		if (acceptAutomaton == null && rejectAutomaton == null)
			return Result.UNKNOWN;

		boolean r1 = false;
		if (acceptAutomaton != null)
			r1 = acceptAutomaton.run(Tools.libALFWord2String(input));
		boolean r2 = false;
		if (rejectAutomaton != null)
			r2 = rejectAutomaton.run(Tools.libALFWord2String(input));

		if ((r1 && r2) || (!r1 && !r2))
			return Result.UNKNOWN;
		else if (r1)
			return Result.ACCEPT;
		else
			return Result.REJECT;
	}

	public Automaton getAcceptAutomaton() {
		return acceptAutomaton;
	}

	public void setAcceptAutomaton(Automaton acceptAutomaton) {
		this.acceptAutomaton = acceptAutomaton;
	}

	public Automaton getRejectAutomaton() {
		return rejectAutomaton;
	}

	public void setRejectAutomaton(Automaton rejectAutomaton) {
		this.rejectAutomaton = rejectAutomaton;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	@Override
	public String getDescription() {
		return description;
	}

	@Override
	public String toString() {
		return getDescription();
	}
}
