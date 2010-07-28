package de.libalf.testsuits;

import de.libalf.BasicAutomaton;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.LibALFFactory;
import de.libalf.LibALFFactory.Algorithm;
import de.libalf.BasicTransition;
import de.libalf.jni.JNIFactory;
import dk.brics.automaton.Automaton;
import dk.brics.automaton.RegExp;
import dk.brics.automaton.State;
import dk.brics.automaton.Transition;

public class Test {

	public static Automaton libALFAutomaton2bricsAutomaton(
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
			s[t.source].addTransition(new Transition((char) t.label,
					s[t.destination]));

		// Automaton
		Automaton brics = new Automaton();

		// Initial states
		if (libalfNFA.getInitialStates().size() != 1) {
			System.err
					.println("Encountered "
							+ libalfNFA.getInitialStates().size()
							+ " initial states while converting from LibALF to brics automaton! Exiting ...");
			System.exit(1);
		}
		brics
				.setInitialState(s[libalfNFA.getInitialStates().iterator()
						.next()]);

		// Final stuff
		brics.setDeterministic(libalfNFA.isDFA());
		brics.restoreInvariant();

		return brics;
	}

	public static boolean membershipQuery(int[] word, Automaton teacher) {
		String w = "";
		for (int i : word)
			w += (char) i;
		return teacher.run(w);
	}

	public static int[] equivalenceQuery(BasicAutomaton hypothesis,
			Automaton teacher) {
		Automaton h = libALFAutomaton2bricsAutomaton(hypothesis);

		Automaton tmp = h.minus(teacher);
		if (!tmp.isEmpty())
			return String2intArray(tmp.getShortestExample(true));
		tmp = teacher.minus(h);
		if (!tmp.isEmpty())
			return String2intArray(tmp.getShortestExample(true));
		return null;
	}

	public static String intArray2String(int[] array) {
		if (array == null)
			return null;
		String res = "[";
		for (int i = 0; i < array.length; i++)
			res += array[i] + (i < array.length - 1 ? ", " : "");
		res += "]";
		return res;
	}

	public static int[] String2intArray(String word) {
		if (word == null)
			return null;
		int[] res = new int[word.length()];
		for (int i = 0; i < word.length(); i++)
			res[i] = word.charAt(i);
		return res;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		Automaton teacher = new RegExp("(\u0000|(\u0001{2})){4}", RegExp.ALL).toAutomaton();

		LibALFFactory factory = new JNIFactory();
		Knowledgebase base = factory.createKnowledgebase();
		LearningAlgorithm alg = factory.createLearningAlgorithm(
				Algorithm.ANGLUIN, new Object[] { base, 2 });

		Automaton learned = null;
		do {
			BasicAutomaton aut = (BasicAutomaton)alg.advance();

			if (aut == null) {
				for (int[] query : base.get_queries())
					base.add_knowledge(query, membershipQuery(query, teacher));
			} else {
				int[] ce = equivalenceQuery(aut, teacher);
				if (ce == null) {
					learned = libALFAutomaton2bricsAutomaton(aut);
					break;
				} else {
					alg.add_counterexample(ce);
				}
			}

		} while (learned == null);

		System.out.println(learned.toDot());
	}
}
