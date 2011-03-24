package de.libalf.junit;

import dk.brics.automaton.Automaton;
import dk.brics.automaton.RegExp;

/**
 * Provides example automata used in the test cases.
 * 
 * @author Daniel Neider
 * 
 */
public class AutomatonProvider {

	public static Automaton getAutomaton(int whichOne) {

		switch (whichOne) {
		case 1:
			return (new RegExp("(\u0001|\u0000)+", RegExp.ALL)).toAutomaton()
					.repeat(3);

		case 2:
			return (new RegExp("(\u0001+|\u0000\u0001\u0002)+", RegExp.ALL))
					.toAutomaton();

		case 3:
			return (new RegExp("\u0000", RegExp.ALL)).toAutomaton().repeat(10,
					10);

		case 4:
			return (new RegExp("\u0000*|((\u0001\u0000)+|(\u0001\u0000)*)",
					RegExp.ALL)).toAutomaton();

		default:
			return new Automaton();
		}
	}
}
