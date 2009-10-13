package de.libalf.demo;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;

import de.libalf.BasicAutomaton;
import de.libalf.BasicTransition;
import dk.brics.automaton.Automaton;
import dk.brics.automaton.State;
import dk.brics.automaton.StatePair;
import dk.brics.automaton.Transition;

public class Tools {

	/**
	 * <p>
	 * Converts a LibALF word (32-bit <code>integer array</code>) into a Java
	 * <code>string</code> (16-bit unicode string).
	 * </p>
	 * <p>
	 * Note that the conversion does only work correctly for alphabets smaller
	 * than 2^16 symbols.
	 * </p>
	 * 
	 * @param word
	 *            a LibALF word to convert
	 * @return the resulting brics word
	 */
	public static String libALFWord2String(int[] word) {
		if (word == null)
			return null;
		String ret = "";
		for (int i = 0; i < word.length; i++)
			ret += word[i];
		return ret;
	}

	/**
	 * Converts a Java <code>string</code> (16-bit unicode string) into a libalf
	 * word (32-bit <code>int array</code>).
	 * 
	 * @param s
	 *            the <code>string</code> to convert.
	 * @return the resukting libalf word.
	 */
	public static int[] string2libalfWord(String s) {
		if (s == null)
			return null;
		if (s.length() == 0)
			return new int[] {};

		int[] result = new int[s.length()];

		for (int i = 0; i < s.length(); i++) {
			if (s.charAt(i) < '0' || s.charAt(i) > '9') {
				return null;
			} else
				result[i] = s.charAt(i) - '0';
		}

		return result;
	}

	/**
	 * Returns a readable version of the given LibALF word.
	 * 
	 * @param word
	 *            to word to display
	 * @return a readable version of the given LibALF word.
	 */
	public static String readableLibALFWord(int[] word) {
		if (word == null)
			return null;
		String ret = "[";
		for (int i = 0; i < word.length; i++)
			ret += word[i] + (i == word.length - 1 ? "]" : ", ");
		return ret;
	}

	/**
	 * <p>
	 * Converts a LibALF Automaton into a brics automaton.
	 * </p>
	 * <p>
	 * <ul>
	 * <li>Note that the conversion does only work correctly for alphabets
	 * smaller than 2^16 symbols.</li>
	 * <li>Currently only brics automata with exactly one initial state are
	 * supported.</li>
	 * </ul>
	 * </p>
	 * 
	 * @param libalfNFA
	 *            a LibALF automaton to convert
	 * @return a brics automaton.
	 */
	public static Automaton basicAutomaton2bricsAutomaton(
			BasicAutomaton libalfNFA) {
		if (libalfNFA == null)
			return null;

		// States
		State[] s = new State[libalfNFA.getNumberOfStates()];
		for (int i = 0; i < s.length; i++)
			s[i] = new State();
		for (int i : libalfNFA.getFinalStates())
			s[i].setAccept(true);

		// Transitions
		for (BasicTransition t : libalfNFA.getTransitions())
			s[t.source].addTransition(new Transition((char) ('0' + t.label),
					s[t.destination]));

		// Automaton
		Automaton brics = new Automaton();

		// Initial states
		if (libalfNFA.getInitialStates().size() != 1) {
			State initialState = new State();
			brics.setInitialState(initialState);

			LinkedList<StatePair> epsilonTransitions = new LinkedList<StatePair>();
			for (Integer init : libalfNFA.getInitialStates()) {
				epsilonTransitions.add(new StatePair(initialState, s[init]));
			}
			brics.addEpsilons(epsilonTransitions);

		} else {
			brics.setInitialState(s[libalfNFA.getInitialStates().iterator()
					.next()]);
		}

		// Final stuff
		brics.setDeterministic(libalfNFA.isDFA());
		brics.restoreInvariant();

		return brics;
	}

	public static BasicAutomaton bricsAutomaton2BasicAutomaton(
			Automaton bricsAutomaton) {
		if (bricsAutomaton == null)
			return null;

		LinkedList<BasicTransition> transitions = new LinkedList<BasicTransition>();
		int maxChar = 0;
		int curState = 1;
		LinkedList<State> states = new LinkedList<State>();
		HashMap<State, Integer> stateMap = new HashMap<State, Integer>(
				bricsAutomaton.getNumberOfStates());
		HashSet<State> visited = new HashSet<State>(bricsAutomaton
				.getNumberOfStates());
		states.add(bricsAutomaton.getInitialState());
		stateMap.put(bricsAutomaton.getInitialState(), 0);

		while (!states.isEmpty()) {
			State s = states.poll();
			visited.add(s);
			for (Transition t : s.getTransitions()) {
				if (!stateMap.containsKey(t.getDest())) {
					stateMap.put(t.getDest(), curState);
					curState++;
				}

				if (t.getMax() > maxChar)
					maxChar = t.getMax();
				for (int i = t.getMin(); i <= t.getMax(); i++) {
					transitions.add(new BasicTransition(stateMap.get(s),
							'0' + i, stateMap.get(t.getDest())));
				}
				if (!visited.contains(t.getDest())
						&& !states.contains(t.getDest()))
					states.add(t.getDest());
			}
		}

		// create LibALF automaton
		BasicAutomaton libALFAutomaton = new BasicAutomaton(bricsAutomaton
				.isDeterministic(), bricsAutomaton.getNumberOfStates(),
				maxChar + 1);
		libALFAutomaton.addInitialState(stateMap.get(bricsAutomaton
				.getInitialState()));
		for (State s : bricsAutomaton.getAcceptStates())
			libALFAutomaton.addFinalState(stateMap.get(s));
		for (BasicTransition t : transitions)
			libALFAutomaton.addTransition(t);

		return libALFAutomaton;
	}

	public static LinkedList<Sample> parseCSV(File file) {
		LinkedList<Sample> list = new LinkedList<Sample>();

		try {
			BufferedReader reader = new BufferedReader(new FileReader(file));

			String line;
			while ((line = reader.readLine()) != null) {

				/*
				 * Parse
				 */
				String[] split = line.split(",");

				int[] word = string2libalfWord(split[0].trim());
				if (word == null)
					continue;

				if (!(split[1].trim().equals("0") || split[1].trim()
						.equals("1")))
					continue;

				list.add(new Sample(word, split[1].trim().equals("0") ? false
						: true));
			}

		} catch (FileNotFoundException e) {
		} catch (IOException e) {
		}

		return list;
	}

	public static boolean checkAlphabetSize(Automaton a, int alphabetSize) {
		// Check automaton for invalid transition labels
		for (State s : a.getStates()) {
			for (Transition t : s.getTransitions()) {
				if (t.getMin() < '0' || t.getMax() > '0' + (alphabetSize - 1)) {
					return false;
				}
			}
		}
		return true;
	}
}