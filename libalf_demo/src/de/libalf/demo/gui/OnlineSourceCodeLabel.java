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

import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;

import javax.swing.JWindow;

import de.libalf.demo.Scenario;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 * 
 */
public class OnlineSourceCodeLabel extends SourceCodeLabel {

	private static final long serialVersionUID = 1L;

	JWindow algorithmWindow, membershipQueryWindow, equivalenceQueryWindow;

	public OnlineSourceCodeLabel(Scenario scenario) {
		super(scenario);

		// Zoom Windows
		algorithmWindow = new AlgorithmWindow();
		membershipQueryWindow = new MembershipQueryWindow();
		equivalenceQueryWindow = new EquivalenceQueryWindow();

		// Mouse listener
		zoomMouseListener = new ZoomMouseListener();

		/*
		 * Advance
		 */
		changeState(State.ADVANCE);
	}

	public void changeState(State s) {
		if (s == null)
			return;

		String code = "<html><head>" + "<style type=\"text/css\">"
				+ "#default { line-height: 50; }" + "</style>" + "</head>"
				+ "<body>";

		code += "<p>LibALFFactory "
				+ color(Color.BLUE, "factory")
				+ " = new "
				+ (scenario.isJniConnection() ? "JNI" : "Dispatcher")
				+ "Factory("
				+ (scenario.isJniConnection() ? "" : scenario.getServer()
						+ ", " + scenario.getPort()) + ");<br>";

		code += "Knowledgebase " + color(Color.BLUE, "knowledgebase") + " = "
				+ color(Color.BLUE, "factory") + ".createKnowledgebase();<br>";

		code += "LearningAlgorithm " + color(Color.BLUE, "algorithm") + " = "
				+ color(Color.BLUE, "factory") + ".createLearningAlgorithm("
				+ scenario.getAlgorithm() + ", "
				+ color(Color.BLUE, "knowledgebase") + ", "
				+ scenario.getAlphabetSize() + ");</p><br>";

		code += "<p>Automaton " + color(Color.BLUE, "automaton")
				+ " = null;<br><br>do {</p><br>";

		/*
		 * Advance
		 */
		code += "<p style=\"margin-left: 30\">";
		if (s.equals(State.ADVANCE)) {
			code += color(Color.RED,
					"Automaton conjecture = algorithm.advance();");
		} else {
			code += "Automaton " + color(Color.BLUE, "conjecture") + " = "
					+ color(Color.BLUE, "algorithm") + ".advance();";
		}
		code += "</p><br>";

		/*
		 * Membership queries
		 */
		code += "<p style=\"margin-left: 30\">";
		if (s.equals(State.MEMBERSHIP)) {
			code += color(Color.RED, "if (automaton == null) {") + "<br>";
			code += "<div style=\"margin-left: 60\">";
			code += color(Color.RED,
					"for(int[] query : knowledgebase.get_queries()) {")
					+ "</div>";
			code += "<div style=\"margin-left: 90\">";
			code += color(Color.RED,
					"boolean answer = answerMembership(query);")
					+ "<br>";
			code += color(Color.RED,
					"knowledgebase.add_knowledge(query, answer);")
					+ "</div>";
			code += "<div style=\"margin-left: 60\">" + color(Color.RED, "}")
					+ "</div>";
			code += "<div style=\"margin-left: 30\">" + color(Color.RED, "}")
					+ "</div>";
		} else {
			code += "if (" + color(Color.BLUE, "automaton") + " == null) {<br>";
			code += "<div style=\"margin-left: 60\">";
			code += "for(int[] " + color(Color.BLUE, "query") + " : "
					+ color(Color.BLUE, "knowledgebase")
					+ ".get_queries()) {</div>";
			code += "<div style=\"margin-left: 90\">";
			code += "boolean " + color(Color.BLUE, "answer")
					+ " = answerMembership(" + color(Color.BLUE, "query")
					+ ");<br>";
			code += color(Color.BLUE, "knowledgebase") + ".add_knowledge("
					+ color(Color.BLUE, "query") + ", "
					+ color(Color.BLUE, "answer") + ");</div>";
			code += "<div style=\"margin-left: 60\">}</div>";
			code += "<div style=\"margin-left: 30\">}</div>";
		}
		code += "</p><br>";

		/*
		 * Equivalence Queries
		 */
		code += "<p style=\"margin-left: 30\">";
		if (s.equals(State.EQUIVALENCE)) {
			code += color(Color.RED, "else {") + "<br>";
			code += "<div style=\"margin-left: 60\">";
			code += color(Color.RED,
					"if (isEquivalent(conjecture)) automaton = conjecture;")
					+ "<br>";
			code += color(Color.RED,
					"else algorithm.add_counterexample(getCE(conjecture));")
					+ "</div>";
			code += "<div style=\"margin-left: 30\">" + color(Color.RED, "}")
					+ "</div>";
		} else {
			code += "else {<br>";
			code += "<div style=\"margin-left: 60\">";
			code += "if (isEquivalent(" + color(Color.BLUE, "conjecture")
					+ ")) " + color(Color.BLUE, "automaton") + " = "
					+ color(Color.BLUE, "conjecture") + ";<br>";
			code += "else " + color(Color.BLUE, "algorithm")
					+ ".add_counterexample(getCE("
					+ color(Color.BLUE, "conjecture") + "));</div>";
			code += "<div style=\"margin-left: 30\">}</div>";
		}
		code += "</p><br>";

		code += "<p>} while(" + color(Color.BLUE, "automaton")
				+ " != null);</p><br>";

		if (s.equals(State.FINISH)) {
			code += color(Color.RED, "return automaton;");
		} else {
			code += "return " + color(Color.BLUE, "automaton");
		}

		code += "</body></html>";

		setText(code);
	}

	@Override
	public void paint(Graphics g) {
		super.paint(g);
	}

	void hideAllZoomWindows(JWindow dontHideMe) {
		if (dontHideMe != initWindow)
			initWindow.setVisible(false);
		if (dontHideMe != algorithmWindow)
			algorithmWindow.setVisible(false);
		if (dontHideMe != membershipQueryWindow)
			membershipQueryWindow.setVisible(false);
		if (dontHideMe != equivalenceQueryWindow)
			equivalenceQueryWindow.setVisible(false);
	}

	private class ZoomMouseListener extends MouseMotionAdapter {

		public void mouseDragged(MouseEvent e) {
			mouseMoved(e);
		}

		public void mouseMoved(MouseEvent e) {
			if (e.getX() > 530) {
				currentlyZooming = null;
				hideAllZoomWindows(currentlyZooming);
			}

			if (e.getY() <= 50) {
				if (currentlyZooming != initWindow)
					currentlyZooming = initWindow;
			} else if ((e.getY() >= 62 && e.getY() <= 153)
					|| (e.getY() >= 350 && e.getY() <= 398)) {
				if (currentlyZooming != algorithmWindow)
					currentlyZooming = algorithmWindow;
			} else if (e.getY() >= 159 && e.getY() <= 259) {
				if (currentlyZooming != membershipQueryWindow)
					currentlyZooming = membershipQueryWindow;
			} else if (e.getY() >= 270 && e.getY() <= 340) {
				if (currentlyZooming != equivalenceQueryWindow)
					currentlyZooming = equivalenceQueryWindow;
			} else {
				currentlyZooming = null;
			}
			hideAllZoomWindows(currentlyZooming);

			if (currentlyZooming != null) {
				currentlyZooming.setLocation(e.getXOnScreen() + 10, e
						.getYOnScreen() + 10);
				currentlyZooming.setVisible(true);
			}
		}
	}

	class AlgorithmWindow extends ZoomWindow {
		private static final long serialVersionUID = 1L;

		public AlgorithmWindow() {
			super();

			String code = getHTMLHeader();
			code += "Automaton " + color(Color.BLUE, "automaton")
					+ " = null;<br>do {";
			code += "<div style=\"margin-left: 60\">";
			code += "Automaton " + color(Color.BLUE, "conjecture") + " = "
					+ color(Color.BLUE, "algorithm") + ".advance();<br><br>";
			code += "if(" + color(Color.BLUE, "automaton") + " == null)";
			code += "</div>";
			code += "<div style=\"margin-left: 90\">";
			code += color(Color.GREEN, "// Answer membership queries");
			code += "</div><br>";
			code += "<div style=\"margin-left: 60\">";
			code += "else";
			code += "</div>";
			code += "<div style=\"margin-left: 90\">";
			code += color(Color.GREEN, "// Answer equivalence queriey");
			code += "</div><br>";
			code += "</div>";
			code += "} until(" + color(Color.BLUE, "automaton")
					+ " != null);<br><br>";
			code += "return " + color(Color.BLUE, "automaton") + ";";
			code += getHTMLEnd();

			editor.setText(code);

			pack();
		}
	}

	class MembershipQueryWindow extends ZoomWindow {
		private static final long serialVersionUID = 1L;

		public MembershipQueryWindow() {
			super();

			String code = getHTMLHeader();
			code += "if (" + color(Color.BLUE, "automaton") + " == null) {<br>";
			code += "<div style=\"margin-left: 60\">";
			code += "for(int[] " + color(Color.BLUE, "query") + " : "
					+ color(Color.BLUE, "knowledgebase") + ".get_queries()) {";
			code += "</div>";
			code += "<div style=\"margin-left: 120\">";
			code += "boolean " + color(Color.BLUE, "answer")
					+ " = answerMembership(" + color(Color.BLUE, "query")
					+ ");<br>";
			code += color(Color.BLUE, "knowledgebase") + ".add_knowledge("
					+ color(Color.BLUE, "query") + ", "
					+ color(Color.BLUE, "answer") + ");</div>";
			code += "<div style=\"margin-left: 60\">}</div>";
			code += "}";
			code += getHTMLEnd();

			editor.setText(code);

			pack();
		}
	}

	class EquivalenceQueryWindow extends ZoomWindow {
		private static final long serialVersionUID = 1L;

		public EquivalenceQueryWindow() {
			super();

			String code = getHTMLHeader();
			code += "else {<br>";
			code += "<div style=\"margin-left: 60\">";
			code += "if (isEquivalent(" + color(Color.BLUE, "conjecture")
					+ ")) " + color(Color.BLUE, "automaton") + " = "
					+ color(Color.BLUE, "conjecture") + ";<br>";
			code += "else " + color(Color.BLUE, "algorithm")
					+ ".add_counterexample(getCE("
					+ color(Color.BLUE, "conjecture") + "));";
			code += "</div>";
			code += "}";
			code += getHTMLEnd();

			editor.setText(code);

			pack();
		}
	}
}
