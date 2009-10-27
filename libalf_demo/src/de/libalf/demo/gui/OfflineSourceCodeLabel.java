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
class OfflineSourceCodeLabel extends SourceCodeLabel {

	private static final long serialVersionUID = 1L;

	JWindow algorithmWindow;

	public OfflineSourceCodeLabel(Scenario scenario) {
		super(scenario);

		// Zoom Windows
		algorithmWindow = new AlgorithmWindow();

		// Mouse listener
		zoomMouseListener = new ZoomMouseListener();

		// Advance
		changeState(State.ADVANCE);
	}

	public void changeState(State s) {
		if (s == null)
			return;

		String code = "<html><head>" + "<style type=\"text/css\">"
				+ "#default { line-height: 50; }" + "</style>" + "</head>"
				+ "<body>";

		code += "<p>LibALFFactory " + color(Color.BLUE, "factory") + " = new "
				+ (scenario.isJniConnection() ? "JNI" : "Dispatcher")
				+ "Factory();<br>";

		code += "Knowledgebase " + color(Color.BLUE, "knowledgebase") + " = "
				+ color(Color.BLUE, "factory") + ".createKnowledgebase();<br>";

		code += "LearningAlgorithm " + color(Color.BLUE, "algorithm") + " = "
				+ color(Color.BLUE, "factory") + ".createLearningAlgorithm("
				+ scenario.getAlgorithm() + ", "
				+ color(Color.BLUE, "knowledgebase") + ", "
				+ scenario.getAlphabetSize() + ");</p><br>";

		if (s.equals(State.ADVANCE)) {
			code += "<p>"
					+ color(Color.RED,
							"Automaton result = algorithm.advance();")
					+ "</p><br>";
		} else {
			code += "<p>Automaton " + color(Color.BLUE, "result") + " = "
					+ color(Color.BLUE, "algorithm") + ".advance();</p><br>";
		}

		if (s.equals(State.FINISH)) {
			code += "<p>" + color(Color.RED, "return result;") + "</p>";
		} else {
			code += "<p>return " + color(Color.BLUE, "result") + ";</p>";
		}

		code += "</body></html>";

		this.setText(code);
	}

	@Override
	public void paint(Graphics g) {
		super.paint(g);
	}

	@Override
	void hideAllZoomWindows(JWindow dontHideMe) {
		if (dontHideMe != initWindow)
			initWindow.setVisible(false);
		if (dontHideMe != algorithmWindow)
			algorithmWindow.setVisible(false);
	}

	private class ZoomMouseListener extends MouseMotionAdapter {

		public void mouseDragged(MouseEvent e) {
			mouseMoved(e);
		}

		public void mouseMoved(MouseEvent e) {
			if (e.getX() > 600) {
				currentlyZooming = null;
				hideAllZoomWindows(currentlyZooming);
			}

			if (e.getY() <= 50) {
				if (currentlyZooming != initWindow)
					currentlyZooming = initWindow;
			} else if (e.getY() >= 62 && e.getY() <= 110) {
				if (currentlyZooming != algorithmWindow)
					currentlyZooming = algorithmWindow;
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
			code += "Automaton " + color(Color.BLUE, "result") + " = "
					+ color(Color.BLUE, "algorithm") + ".advance();<br><br>";
			code += "return " + color(Color.BLUE, "automaton") + ";";
			code += getHTMLEnd();

			editor.setText(code);

			pack();
		}
	}
}