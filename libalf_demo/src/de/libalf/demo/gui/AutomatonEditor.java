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
import java.awt.Color;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;

import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.border.Border;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileNameExtensionFilter;

import de.libalf.demo.Tools;
import dk.brics.automaton.Automaton;
import dk.brics.automaton.RegExp;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 */
public class AutomatonEditor extends JDialog {

	private static final long serialVersionUID = 1L;

	private Automaton automaton = Automaton.makeEmptyString(),
			regexpAutomaton = Automaton.makeEmptyString(),
			fromFileAutomaton = null;
	private Border inputBorder;
	private JTextField input;
	private JCheckBox previewBox;
	private JPanel previewPanel;

	private int returnValue = DemoDesktop.CANCEL;
	private int alphabetSize;

	public AutomatonEditor() {
		this(10);
	}

	public AutomatonEditor(int alphabetSize) {
		super();
		this.alphabetSize = alphabetSize;
	}

	/**
	 * Creates the GUI and shows it.
	 */
	private void buildGUI() {
		/*
		 * Stuff
		 */
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		setSize(640, 480);
		setTitle("Automaton Editor");
		setModal(true);
		setLayout(new BorderLayout());

		/*
		 * Chooser
		 */
		JPanel cPanel = new JPanel(new BorderLayout());
		cPanel.setBorder(new TitledBorder("Sources"));
		add(cPanel, BorderLayout.NORTH);

		// RadioPanel
		JPanel rPanel = new JPanel(new GridLayout(2, 1));
		cPanel.add(rPanel, BorderLayout.WEST);

		// Regexp button
		ButtonGroup bg = new ButtonGroup();
		JRadioButton regButton = new JRadioButton("From regular expression",
				true);
		rPanel.add(regButton);
		bg.add(regButton);

		// From file button
		JRadioButton fileButton = new JRadioButton("From file", false);
		rPanel.add(fileButton);
		bg.add(fileButton);

		/*
		 * input panel
		 */
		JPanel iPanel = new JPanel(new GridLayout(2, 1));
		cPanel.add(iPanel, BorderLayout.CENTER);

		// regexp field
		JPanel t = new JPanel(new FlowLayout(FlowLayout.LEFT));
		iPanel.add(t);
		input = new JTextField("", 30);
		t.add(input);
		// Border
		inputBorder = new LineBorder(Color.GREEN);
		input.setBorder(inputBorder);
		input.addKeyListener(new MyKeyListener(alphabetSize));
		input.requestFocusInWindow();
		input.addKeyListener(new KeyAdapter() {
			public void keyPressed(KeyEvent e) {
				if (e.getKeyCode() == KeyEvent.VK_ENTER)
					okPressed();
			}
		});

		// From file
		t = new JPanel(new FlowLayout(FlowLayout.LEADING));
		iPanel.add(t);
		final JButton fileChooserButton = new JButton("Choose ...");
		t.add(fileChooserButton);
		fileChooserButton.setEnabled(false);
		fileChooserButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				loadAutomatonFromFile();
				automaton = fromFileAutomaton;
				showPreview();
			}
		});

		/*
		 * Action of radio buttons
		 */
		fileButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				input.setEnabled(false);
				fileChooserButton.setEnabled(true);
				if (fromFileAutomaton == null)
					loadAutomatonFromFile();
				automaton = fromFileAutomaton;
				showPreview();
			}
		});

		regButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				input.setEnabled(true);
				fileChooserButton.setEnabled(false);
				automaton = regexpAutomaton;
				showPreview();
			}
		});

		/*
		 * Preview panel
		 */
		JPanel pPanel = new JPanel(new BorderLayout());
		pPanel.setBorder(new TitledBorder("Preview"));
		add(pPanel, BorderLayout.CENTER);

		// Preview Panel
		previewPanel = new JPanel(new BorderLayout());
		pPanel.add(previewPanel, BorderLayout.CENTER);

		// Preview Checkbox
		previewBox = new JCheckBox("Show preview", true);
		pPanel.add(previewBox, BorderLayout.SOUTH);
		previewBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				showPreview();
			}
		});

		/*
		 * Button panel
		 */
		JPanel buttonPanel = new JPanel(new BorderLayout());
		add(buttonPanel, BorderLayout.SOUTH);

		// OK Button
		JButton okButton = new JButton("OK");
		buttonPanel.add(okButton, BorderLayout.EAST);
		okButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				okPressed();
			}
		});

		// Save button
		JButton saveButton = new JButton("Save");
		buttonPanel.add(saveButton, BorderLayout.WEST);
		saveButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if (automaton == null) {
					JOptionPane.showMessageDialog(AutomatonEditor.this,
							"No valid automaton selected",
							"No valid automaton", JOptionPane.WARNING_MESSAGE);
				} else {
					JFileChooser chooser = new JFileChooser();
					FileNameExtensionFilter filter = new FileNameExtensionFilter(
							"Automata", "automaton");
					chooser.setFileFilter(filter);
					int returnVal = chooser
							.showSaveDialog(AutomatonEditor.this);
					if (returnVal == JFileChooser.APPROVE_OPTION) {
						try {
							ObjectOutputStream oos = new ObjectOutputStream(
									new FileOutputStream(chooser
											.getSelectedFile()));
							oos.writeObject(automaton);
						} catch (Exception e1) {
							JOptionPane.showMessageDialog(AutomatonEditor.this,
									e, "Error occured",
									JOptionPane.ERROR_MESSAGE);
						}
					}
				}
			}
		});

		/*
		 * More stuff
		 */
		showPreview();
		setVisible(true);
	}

	public int showAutomatonChooser() {
		buildGUI();

		return returnValue;
	}

	public Automaton getAutomaton() {
		return automaton;
	}

	private void showPreview() {
		// Clear preview
		previewPanel.removeAll();

		if (previewBox.isSelected()) {

			if (automaton == null) {
				previewPanel.add(new JLabel("No preview available."));
			} else {
				previewPanel.add(new JScrollPane(new JLabel("<html><body>"
						+ automaton.toDot().replaceAll("\n", "<br>")
						+ "</body></html>")), BorderLayout.CENTER);
			}

		} else {
			previewPanel.add(new JLabel("Preview disabled."));
		}

		// show
		previewPanel.revalidate();
		previewPanel.repaint();
	}

	private void loadAutomatonFromFile() {
		JFileChooser chooser = new JFileChooser();
		FileNameExtensionFilter filter = new FileNameExtensionFilter(
				"Automata", "automaton");
		chooser.setFileFilter(filter);
		int returnVal = chooser.showOpenDialog(AutomatonEditor.this);
		if (returnVal == JFileChooser.APPROVE_OPTION) {
			try {
				ObjectInputStream ios = new ObjectInputStream(
						new FileInputStream(chooser.getSelectedFile()));
				fromFileAutomaton = (Automaton) ios.readObject();
			} catch (Exception e) {
				JOptionPane.showMessageDialog(AutomatonEditor.this, e,
						"Error occured", JOptionPane.ERROR_MESSAGE);
			}
		}
	}

	public void okPressed() {
		if (automaton == null) {
			JOptionPane.showMessageDialog(AutomatonEditor.this,
					"No automaton selected. Please select one.",
					"No automaton selected", JOptionPane.WARNING_MESSAGE);
		} else {
			returnValue = DemoDesktop.OK;
			dispose();
		}
	}

	private class MyKeyListener extends KeyAdapter {

		public ArrayList<Character> allowedChars = new ArrayList<Character>(25);

		public MyKeyListener(int alphabetSize) {
			// Add allowed chars
			char[] auxChars = new char[] { '|', '&', '(', ')', '+', '?', '*',
					',', '#', '{', '}' };
			for (char ch : auxChars)
				allowedChars.add(ch);

		}

		@Override
		public void keyReleased(KeyEvent arg0) {

			if (input.getText() == null)
				input.setText("");

			Automaton a = parseRegExp(input.getText() == null ? "" : input
					.getText());

			if (a != null) {
				if (!a.equals(regexpAutomaton)) {
					regexpAutomaton = automaton = a;
					showPreview();
				}
				input.setBorder(new LineBorder(Color.GREEN));
			} else {
				regexpAutomaton = automaton = null;
				input.setBorder(new LineBorder(Color.RED));
				showPreview();
			}
		}

		private Automaton parseRegExp(String regExp) {
			try {
				// Check for valid input characters
				for (char c : regExp.toCharArray()) {
					if (allowedChars.contains(c))
						continue;
					else if (c < '0' || c > '9') {
						return null;
					}
				}

				RegExp r = new RegExp(input.getText(), RegExp.ALL);
				Automaton a = r.toAutomaton();

				if (Tools.checkAlphabetSize(a, alphabetSize))
					return a;
				else
					return null;

			} catch (Exception e) {
				return null;
			}
		}
	}
	
}
