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

import java.awt.BorderLayout;
import java.awt.Desktop;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.net.URISyntaxException;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JEditorPane;
import javax.swing.JPanel;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 */
public class AboutDialog extends JDialog {

	private static final long serialVersionUID = 1L;

	private Desktop desktop = null;

	public AboutDialog() {
		/*
		 * Check if Desktop is supported
		 */
		if (Desktop.isDesktopSupported()) {
			desktop = Desktop.getDesktop();
		}

		/*
		 * Stuff
		 */
		setLayout(new BorderLayout());
		setTitle("About libalf Demo");
		setModal(true);
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);

		/*
		 * Text
		 */
		String text = "<html>" + "<head>" + "<style>"
				+ "body {font-family:Calibri,Arial,sans-serif; padding:10px;}"
				+ "</style>" + "</head>" + "<body>";
		text += "<h2>libalf Demo</h2>"
				+ "<p>Version: 0.1<br>"
				+ "By Daniel Neider (<a href=\"mailto:neider@automata.rwth-aachen.de?SUBJECT=libalf%20Demo\">neider@automata.rwth-aachen.de</a>)</p><br>";
		text += "<hr>";
		text += "<h2>libalf</h2>"
				+ "<p>Version: "
				+ (libalfVersion() == null ? "could not determine libalf version"
						: libalfVersion())
				+ "<br>"
				+ "By Carsten Kern, Daniel Neider, David Piegdon<br><br>"
				+ "Visit <a href=\"http://libalf.informatik.rwth-aachen.de/\">http://libalf.informatik.rwth-aachen.de/</a></p>";
		text += "</body></html>";
		JEditorPane editor = new JEditorPane();
		editor.setContentType("text/html");
		editor.setText(text);
		editor.setEditable(false);
		add(editor, BorderLayout.CENTER);
		editor.addHyperlinkListener(new HyperlinkListener() {
			public void hyperlinkUpdate(HyperlinkEvent e) {
				if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
					if (e.getURL().getProtocol().equals("mailto")) {
						if (desktop.isSupported(Desktop.Action.MAIL)) {
							try {
								desktop.mail(e.getURL().toURI());
							} catch (IOException e1) {
								e1.printStackTrace();
							} catch (URISyntaxException e1) {
								e1.printStackTrace();
							}
						}
					} else {
						if (desktop.isSupported(Desktop.Action.BROWSE)) {
							try {
								desktop.browse(e.getURL().toURI());
							} catch (IOException e1) {
								e1.printStackTrace();
							} catch (URISyntaxException e1) {
								e1.printStackTrace();
							}
						}
					}
				}
			}
		});

		/*
		 * OK button
		 */
		JPanel bPanel = new JPanel();
		add(bPanel, BorderLayout.SOUTH);
		JButton okButton = new JButton("OK");
		bPanel.add(okButton);
		okButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				dispose();
			}
		});

		/*
		 * More stuff
		 */
		pack();
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		int x = screenSize.width <= getWidth() ? 0
				: (screenSize.width - getWidth()) / 2;
		int y = screenSize.height <= getHeight() ? 0
				: (screenSize.height - getHeight()) / 2;
		setLocation(x, y);

		setVisible(true);
	}

	/**
	 * Returns the libalf and JNI binding version
	 * 
	 * @return the libalf version or <code>null</code> if it could not be
	 *         determined.
	 */
	public String libalfVersion() {
		return "libalf: 0.1, JNI binding: 0.1";
	}
}
