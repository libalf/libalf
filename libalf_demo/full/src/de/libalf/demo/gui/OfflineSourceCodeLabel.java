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
import de.libalf.demo.gui.OnlineSourceCodeLabel.Color;
import de.libalf.demo.gui.OnlineSourceCodeLabel.State;

class OfflineSourceCodeLabel extends JLabel {

	private static final long serialVersionUID = 1L;
	private Scenario scenario;

	private JWindow initWindow, algorithmWindow, currentlyZooming = null;
	private MouseMotionListener zoomMouseListener = new ZoomMouseListener();

	public OfflineSourceCodeLabel(Scenario scenario) {
		super();
		this.scenario = scenario;

		/*
		 * Zoom Windows
		 */
		initWindow = new InitWindow();
		algorithmWindow = new AlgorithmWindow();

		/*
		 * GUI stuff
		 */
		final JPopupMenu popupMenu = new MyPopupMenu();
		addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent e) {
				if (e.getButton() == MouseEvent.BUTTON3) {
					popupMenu.show(OfflineSourceCodeLabel.this, e.getX(), e
							.getY());
				}
			}

			public void mouseExited(MouseEvent e) {
				if (currentlyZooming != null)
					currentlyZooming.setVisible(false);
				currentlyZooming = null;
			}
		});
		setOpaque(true);
		setBackground(java.awt.Color.WHITE);

		// Advance
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

	private String color(Color c, String s) {
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
						OfflineSourceCodeLabel.this
								.removeMouseMotionListener(zoomMouseListener);
						hideAllZoomWindows(null);
					} else {
						zoomEnabled = true;
						OfflineSourceCodeLabel.this
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
			code += "Automaton " + color(Color.BLUE, "result") + " = "
					+ color(Color.BLUE, "algorithm") + ".advance();<br><br>";
			code += "return " + color(Color.BLUE, "automaton") + ";";
			code += getHTMLEnd();

			editor.setText(code);

			pack();
		}
	}
}