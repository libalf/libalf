package de.libalf.demo.gui;

import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import java.awt.event.MouseMotionListener;

import javax.swing.JCheckBoxMenuItem;
import javax.swing.JEditorPane;
import javax.swing.JLabel;
import javax.swing.JPopupMenu;
import javax.swing.JWindow;
import javax.swing.border.EtchedBorder;

import de.libalf.demo.Scenario;

public class OnlineSourceCodeLabel extends JLabel {

	public enum Color {
		RED, BLUE, GREEN;
	}

	public enum State {
		ADVANCE, MEMBERSHIP, EQUIVALENCE, FINISH;
	}

	private static final long serialVersionUID = 1L;

	private Scenario scenario;

	private MouseMotionListener zoomMouseListener = new ZoomMouseListener();

	private JWindow currentlyZooming = null;
	private JWindow initWindow, algorithmWindow, membershipQueryWindow,
			equivalenceQueryWindow;

	public OnlineSourceCodeLabel(Scenario scenario) {
		super();
		this.scenario = scenario;

		/*
		 * Initialize zoom
		 */
		initWindow = new InitWindow();
		algorithmWindow = new AlgorithmWindow();
		membershipQueryWindow = new MembershipQueryWindow();
		equivalenceQueryWindow = new EquivalenceQueryWindow();

		/*
		 * GUI stuff
		 */
		setOpaque(true);
		setBackground(java.awt.Color.WHITE);
		final JPopupMenu popupMenu = new MyPopupMenu();
		addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent e) {
				if (e.getButton() == MouseEvent.BUTTON3) {
					popupMenu.show(OnlineSourceCodeLabel.this, e.getX(), e
							.getY());
				}
			}

			public void mouseExited(MouseEvent e) {
				if (currentlyZooming != null)
					currentlyZooming.setVisible(false);
				currentlyZooming = null;
			}
		});

		/*
		 * Advance
		 */
		changeState(State.ADVANCE);
	}

	public void changeState(State s) {
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

	public String color(Color c, String s) {
		switch (c) {
		case RED:
			return "<span style=\"color:red\">" + s + "</span>";
		case BLUE:
			return "<span style=\"color:blue\">" + s + "</span>";
		case GREEN:
			return "<span style=\"color:green\">" + s + "</span>";
		default:
			return s;
		}
	}

	private void hideAllZoomWindows(JWindow dontHideMe) {
		if (dontHideMe != initWindow)
			initWindow.setVisible(false);
		if (dontHideMe != algorithmWindow)
			algorithmWindow.setVisible(false);
		if (dontHideMe != membershipQueryWindow)
			membershipQueryWindow.setVisible(false);
		if (dontHideMe != equivalenceQueryWindow)
			equivalenceQueryWindow.setVisible(false);
	}

	private class MyPopupMenu extends JPopupMenu {
		private static final long serialVersionUID = 1L;

		private boolean zoomEnabled = false;

		public MyPopupMenu() {

			final JCheckBoxMenuItem box = new JCheckBoxMenuItem("Enable zoom",
					zoomEnabled);
			add(box);
			box.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					if (zoomEnabled) {
						zoomEnabled = false;
						OnlineSourceCodeLabel.this
								.removeMouseMotionListener(zoomMouseListener);
						hideAllZoomWindows(null);
					} else {
						zoomEnabled = true;
						OnlineSourceCodeLabel.this
								.addMouseMotionListener(zoomMouseListener);
					}
					box.setSelected(zoomEnabled);
				}
			});
		}
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

	class ZoomWindow extends JWindow {
		private static final long serialVersionUID = 1L;

		JEditorPane editor = new JEditorPane();

		public ZoomWindow() {
			editor.setContentType("text/html");
			editor.setEditable(false);
			editor.setBorder(new EtchedBorder(EtchedBorder.LOWERED));
			getContentPane().add(editor);
		}

		String getHTMLHeader() {
			return "<html>" + "<head>" + "<style>" + "body {"
					+ "font-family:Calibri,Arial,sans-serif; "
					+ "padding:10px; " + "font-size:x-large;" + "}"
					+ "</style>" + "</head>" + "<body>";
		}

		String getHTMLEnd() {
			return "<body></html>";
		}
	}

	class InitWindow extends ZoomWindow {
		private static final long serialVersionUID = 1L;

		public InitWindow() {
			super();

			String code = getHTMLHeader();
			code += "LibALFFactory " + color(Color.BLUE, "factory") + " = new "
					+ (scenario.isJniConnection() ? "JNI" : "Dispatcher")
					+ "Factory();<br>";

			code += "Knowledgebase " + color(Color.BLUE, "knowledgebase")
					+ " = " + color(Color.BLUE, "factory")
					+ ".createKnowledgebase();<br>";

			code += "LearningAlgorithm " + color(Color.BLUE, "algorithm")
					+ " = " + color(Color.BLUE, "factory")
					+ ".createLearningAlgorithm(" + scenario.getAlgorithm()
					+ ", " + color(Color.BLUE, "knowledgebase") + ", "
					+ scenario.getAlphabetSize() + ");";
			code += getHTMLEnd();

			editor.setText(code);

			pack();
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
