package de.libalf.demo.filter;

import de.libalf.demo.Tools;
import dk.brics.automaton.Automaton;

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
