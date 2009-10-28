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

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 * 
 */
public class SourceCodeLabel extends JLabel {

	private static final long serialVersionUID = 1L;

	public enum Color {
		RED, BLUE, GREEN;
	}

	public enum State {
		ADVANCE, MEMBERSHIP, EQUIVALENCE, FINISH;
	}

	Scenario scenario;

	JWindow initWindow;
	JWindow currentlyZooming = null;

	MouseMotionListener zoomMouseListener;

	public SourceCodeLabel(Scenario scenario) {
		this(scenario, false);
	}

	public SourceCodeLabel(Scenario scenario, boolean doZooming) {
		super();
		this.scenario = scenario;

		// Zoom Windows
		initWindow = new InitWindow();

		// Mouse listener
		zoomMouseListener = new ZoomMouseListener();

		/*
		 * Popup menu
		 */
		final JPopupMenu popupMenu = new MyPopupMenu(doZooming);
		addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent e) {
				if (e.getButton() == MouseEvent.BUTTON3) {
					popupMenu.show(SourceCodeLabel.this, e.getX(), e.getY());
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

		/*
		 * Set zooming
		 */
		if (doZooming)
			addMouseMotionListener(zoomMouseListener);

		/*
		 * Advance
		 */
		changeState(null);
	}

	String color(Color c, String s) {
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

	String getHTMLHeader() {
		return "<html>" + "<head>" + "<style>" + "body {"
				+ "font-family:Calibri,Arial,sans-serif; " + "padding:10px; "
				+ "font-size: large;" + "}" + "</style>" + "</head>" + "<body>";
	}

	String getHTMLEnd() {
		return "<body></html>";
	}

	public void changeState(State s) {
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

		code += "</body></html>";

		setText(code);
	}

	void hideAllZoomWindows(JWindow dontHideMe) {
		if (dontHideMe != initWindow)
			initWindow.setVisible(false);
	}

	class ZoomWindow extends JWindow {
		private static final long serialVersionUID = 1L;

		JEditorPane editor = new JEditorPane();

		public ZoomWindow() {
			editor.setContentType("text/html");
			editor.setEditable(false);
			editor.setBorder(new EtchedBorder(EtchedBorder.LOWERED));
			getContentPane().add(editor);
			setAlwaysOnTop(true);
		}
	}

	class InitWindow extends ZoomWindow {
		private static final long serialVersionUID = 1L;

		public InitWindow() {
			super();

			String code = getHTMLHeader();
			code += "LibALFFactory "
					+ color(Color.BLUE, "factory")
					+ " = new "
					+ (scenario.isJniConnection() ? "JNI" : "Dispatcher")
					+ "Factory("
					+ (scenario.isJniConnection() ? "" : scenario.getServer()
							+ ", " + scenario.getPort()) + ");<br>";

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

	public boolean isZooming() {
		return getMouseMotionListeners().length > 0;
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
			}
			hideAllZoomWindows(currentlyZooming);

			if (currentlyZooming != null) {
				currentlyZooming.setLocation(e.getXOnScreen() + 10, e
						.getYOnScreen() + 10);
				currentlyZooming.setVisible(true);
			}
		}
	}

	class MyPopupMenu extends JPopupMenu {
		private static final long serialVersionUID = 1L;

		private boolean zoomEnabled = false;

		public MyPopupMenu(boolean doZoom) {
			this.zoomEnabled = doZoom;

			final JCheckBoxMenuItem box = new JCheckBoxMenuItem("Enable zoom",
					zoomEnabled);
			add(box);
			box.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					if (zoomEnabled) {
						zoomEnabled = false;
						SourceCodeLabel.this
								.removeMouseMotionListener(zoomMouseListener);
						hideAllZoomWindows(null);
					} else {
						zoomEnabled = true;
						SourceCodeLabel.this
								.addMouseMotionListener(zoomMouseListener);
					}
					box.setSelected(zoomEnabled);
				}
			});
		}
	}
}
