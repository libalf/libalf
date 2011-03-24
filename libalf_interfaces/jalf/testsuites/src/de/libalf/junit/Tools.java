package de.libalf.junit;

import java.util.LinkedList;

import de.libalf.BasicAutomaton;
import de.libalf.BasicTransition;
import dk.brics.automaton.Automaton;
import dk.brics.automaton.State;
import dk.brics.automaton.StatePair;
import dk.brics.automaton.Transition;

public class Tools {

	/**
	 * Compares two arrays.
	 * 
	 * @param arr1
	 *            the first array
	 * @param arr2
	 *            the second array
	 * 
	 * @return whether <code>arr1==arr2</code>.
	 */
	public static boolean compare_arr(int[] arr1, int[] arr2) {
		if (arr1 == null && arr2 == null)
			return true;
		if ((arr1 == null && arr2 != null) || (arr1 != null && arr2 == null))
			return false;
		if (arr1.length != arr2.length)
			return false;

		for (int i = 0; i < arr1.length; i++) {
			if (arr1[i] != arr2[i])
				return false;
		}

		return true;
	}
	
	/**
	 * Returns a human readable string of the given array.
	 * 
	 * @param arr
	 *            the array to print
	 * @return a human readable string representing the array.
	 */
	public static String arr2readableString(int[] arr) {
		String s = new String("[");
		for (int i = 0; i < arr.length; i++) {
			s += arr[i] + (i < arr.length - 1 ? ", " : "");
		}
		return s + "]";
	}
	
	/**
	 * Converts a libalf automaton into a brics automaton.
	 * 
	 * @param a
	 *            the libalf automaton
	 * @return a the given automaton as brics automaton
	 */
	public static Automaton libalf2brics(BasicAutomaton a) {

		// Create states
		State[] states = new State[a.getNumberOfStates()];
		for (int i = 0; i < a.getNumberOfStates(); i++) {
			states[i] = new State();
			states[i].setAccept(false);
		}

		// Process transitions
		for (BasicTransition t : a.getTransitions()) {
			states[t.source].addTransition(new Transition((char) t.label,
					states[t.destination]));
		}

		// Create brics automaton
		Automaton brics = new Automaton();

		// Process initial states
		// Exactly one initial state
		if (a.getInitialStates().size() == 1) {
			brics
					.setInitialState(states[a.getInitialStates().iterator()
							.next()]);
		}
		// More initial states
		else if (a.getInitialStates().size() > 1) {
			State q = new State();
			q.setAccept(false);
			brics.setInitialState(q);
			LinkedList<StatePair> epsilon = new LinkedList<StatePair>();
			for (int i : a.getInitialStates()) {
				epsilon.add(new StatePair(q, states[i]));
				if (a.getFinalStates().contains(i)) {
					q.setAccept(true);
				}
			}
			brics.addEpsilons(epsilon);
		}
		// No initial state, return empty automaton
		else {
			System.err
					.println("Error, libalf automaton does not have an initial state.");
			return new Automaton();
		}

		// Process final states
		for (int i : a.getFinalStates()) {
			states[i].setAccept(true);
		}

		// Some final things
		brics.restoreInvariant();
		brics.setDeterministic(a.isDFA());

		return brics;
	}

	/**
	 * Converts an int array as used by libalf into a string as used by brics.
	 * 
	 * @param a
	 *            the array to convert
	 * @return the converted string.
	 */
	public static String arr2str(int[] a) {
		String out = "";
		for (int i : a) {
			out += (char) i;
		}
		return out;
	}

	/**
	 * Converts a string as used by brics automata into an int array as used by
	 * libalf.
	 * 
	 * @param s
	 *            the string to convert
	 * @return the converted int array.
	 */
	public static int[] str2arr(String s) {
		int[] out = new int[s.length()];
		for (int i = 0; i < s.length(); i++) {
			out[i] = s.charAt(i);
		}
		return out;
	}
}
