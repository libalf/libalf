package de.libalf.demo.gui;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.io.StringReader;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JEditorPane;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;
import javax.swing.border.TitledBorder;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.EditorKit;
import javax.swing.text.html.HTMLEditorKit;

import de.libalf.Knowledgebase;
import de.libalf.demo.Scenario;
import de.libalf.demo.Tools;
import dk.brics.automaton.Automaton;

public class EquivalenceQueryDialog extends JDialog {

	private static final long serialVersionUID = 1L;

	private Scenario scenario;

	private Automaton conjecture;

	private Knowledgebase knowledgebase;

	private int[] counterEx = null;

	private boolean conjectureIsEquivalent = false;
	private boolean guiIsBuild = false;

	private int returnValue = DemoDesktop.CANCEL;

	public EquivalenceQueryDialog(Scenario scenario, Automaton conjecture,
			Knowledgebase knowledgebase) {
		super();
		this.scenario = scenario;
		this.knowledgebase = knowledgebase;
		this.conjecture = conjecture;
	}

	private void buildGUI() {
		/*
		 * Stuff
		 */
		setTitle("Equivalence Query");
		setSize(700, 600);
		setModal(true);
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		setLayout(new BorderLayout());

		/*
		 * Top pane (info field)
		 */
		String text = "<html>" + "<head>" + "<style>"
				+ "body {font-family:Calibri,Arial,sans-serif;}" + "</style>"
				+ "</head>" + "<body>";
		text += "Check the conjecture for equivalence:";
		text += "<ul>";
		text += "<li>If it is equivalent, then check the <b>conjecture is equivalent</b> box.";
		text += "<li>If it is not, then input a counter-example.";
		text += "</ul>";
		text += "Proceed by clicking <b>OK</b>.";
		if (scenario.getTeacher() != null)
			text += " <a href=\"more\">Read more ...</a>";
		text += "</body></html>";
		final JEditorPane editor = new JEditorPane();
		editor.setEditable(false);
		editor.setOpaque(false);
		editor.setContentType("text/html");
		editor.setText(text);
		if (scenario.getTeacher() != null)
			editor.addHyperlinkListener(new HyperlinkListener() {
				public void hyperlinkUpdate(HyperlinkEvent e) {
					if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
						String extendedText = "<html>"
								+ "<head>"
								+ "<style>"
								+ "body {font-family:Calibri,Arial,sans-serif;}"
								+ "</style>" + "</head>" + "<body>";
						extendedText += "<a name=\"top\"/>";
						extendedText += "Check the conjecture for equivalence:";
						extendedText += "<ul>";
						extendedText += "<li>If it is equivalent, then check the <b>conjecture is equivalent</b> box.";
						extendedText += "<li>If it is not, then input a counter-example.";
						extendedText += "</ul>";
						extendedText += "Proceed by clicking <b>OK</b>.<br><br>";
						extendedText += "If the learning is guided by a teacher automaton, the equivalence check is performed automatically."
								+ " Moreover, a <em>Teacher</em> and <em>Difference automaton</em> tab are added to simplify the equivalence check when performed manually.";
						EditorKit kit = new HTMLEditorKit();
						Document d = kit.createDefaultDocument();
						try {
							kit.read(new StringReader(extendedText), d, 0);
						} catch (IOException e1) {
							e1.printStackTrace();
						} catch (BadLocationException e1) {
							e1.printStackTrace();
						}
						editor.setDocument(d);
					}
				}
			});
		JScrollPane sPane = new JScrollPane(editor);
		sPane.setBorder(new TitledBorder("Info"));

		/*
		 * TabbedPane
		 */
		JTabbedPane tabbedPane = new JTabbedPane();
		// Conjecture
		tabbedPane.addTab("Conjecture", GrappaAutomatonVisualizer
				.createZoomableGrappaPanel(conjecture.toDot()));

		if (scenario.isUseTeacher() && scenario.getTeacher() != null) {
			// Teacher
			tabbedPane.addTab("Teacher", GrappaAutomatonVisualizer
					.createZoomableGrappaPanel(scenario.getTeacher().toDot()));

			// Difference
			Automaton diff = (scenario.getTeacher().minus(conjecture))
					.union(conjecture.minus(scenario.getTeacher()));
			diff.minimize();
			tabbedPane.addTab("Difference automaton", GrappaAutomatonVisualizer
					.createZoomableGrappaPanel(diff.toDot()));
		}

		/*
		 * Top panel
		 */
		JPanel topPanel = new  JPanel(new GridBagLayout());
		add(topPanel, BorderLayout.CENTER);
		// Info panel
		GridBagConstraints c = new GridBagConstraints();
		c.gridx = 0;
		c.gridy = 0;
		c.weighty = 0.25;
		c.weightx = 1.0;
		c.fill = GridBagConstraints.BOTH;
		topPanel.add(sPane, c);
		// Table
		c.gridy = 1;
		c.weighty = 0.75;
		c.weightx = 1.0;
		topPanel.add(tabbedPane, c);
		
		/*
		 * BottomPanel
		 */
		{
			JPanel bottomPanel = new JPanel(new BorderLayout());
			add(bottomPanel, BorderLayout.SOUTH);

			// Panel
			JPanel l = new JPanel(new GridLayout(2, 1));
			l.setBorder(new TitledBorder("Counter-example"));
			bottomPanel.add(l, BorderLayout.CENTER);

			// Checkbox
			final JCheckBox box = new JCheckBox("Conjecture is equivalent",
					false);
			l.add(box);

			// Counter example input field
			JPanel textPanel = new JPanel(new BorderLayout(10, 0));
			l.add(textPanel);
			JLabel lLabel = new JLabel("Counter-example:");
			textPanel.add(lLabel, BorderLayout.WEST);
			final JTextField counterExampleField = new JTextField();
			textPanel.add(counterExampleField, BorderLayout.CENTER);
			counterExampleField.addKeyListener(new KeyAdapter() {
				public void keyPressed(KeyEvent arg0) {
					if (arg0.getKeyCode() == KeyEvent.VK_ENTER) {
						int[] tmp = parseCounterExample(scenario
								.getAlphabetSize(), counterExampleField
								.getText());
						if (validateCounterExample(tmp)) {
							counterEx = tmp;
							EquivalenceQueryDialog.this.returnValue = DemoDesktop.OK;
							dispose();
						} else {
							JOptionPane.showMessageDialog(
									EquivalenceQueryDialog.this,
									"The counter-example is not valid",
									"Invalid counter-example",
									JOptionPane.WARNING_MESSAGE);
						}
					}
				}
			});

			// OK button
			final JButton ok = new JButton("OK");
			bottomPanel.add(ok, BorderLayout.EAST);
			ok.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					// Is the automaton equivalent?
					if (conjectureIsEquivalent) {
						EquivalenceQueryDialog.this.returnValue = DemoDesktop.OK;
						dispose();
					} else {
						// 
						int[] tmp = parseCounterExample(scenario
								.getAlphabetSize(), counterExampleField
								.getText());
						if (validateCounterExample(tmp)) {
							counterEx = tmp;
							EquivalenceQueryDialog.this.returnValue = DemoDesktop.OK;

							dispose();
						} else {
							JOptionPane.showMessageDialog(
									EquivalenceQueryDialog.this,
									"The counter-example is not valid",
									"Invalid counter-example",
									JOptionPane.WARNING_MESSAGE);
						}
					}
				}
			});

			/*
			 * Actions
			 */
			box.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					if (box.isSelected()) {
						counterExampleField.setEnabled(false);
						conjectureIsEquivalent = true;
					} else {
						counterExampleField.setEnabled(true);
						conjectureIsEquivalent = false;
					}
				}
			});

			if (scenario.isUseTeacher() && scenario.getTeacher() != null) {
				if (scenario.getTeacher().equals(conjecture)) {
					conjectureIsEquivalent = true;
					box.setSelected(true);
					counterExampleField.setEnabled(false);
				} else {
					Automaton diff = (scenario.getTeacher().minus(conjecture))
							.union(conjecture.minus(scenario.getTeacher()));
					counterExampleField.setText(diff.getShortestExample(true));
					counterExampleField.setSelectionEnd(counterExampleField
							.getText().length() - 1);
					counterExampleField.setSelectionStart(0);
				}
			}
			counterExampleField.requestFocus();
		}

		/*
		 * More stuff
		 */
		guiIsBuild = true;
		setVisible(true);
	}

	private int[] parseCounterExample(int alphabetSize, String ce) {
		if (ce == null)
			ce = "";

		int[] word = Tools.string2libalfWord(ce);
		if (word == null)
			return null;
		for (int i : word)
			if (i >= alphabetSize)
				return null;

		return word;
	}

	private boolean validateCounterExample(int[] ce) {
		if (ce != null
				&& knowledgebase.resolve_query(ce) == Knowledgebase.Acceptance.UNKNOWN)
			return true;
		return false;
	}

	public int showEquivalenceQueryDialog() {
		// Show dialog
		if (!guiIsBuild)
			buildGUI();
		else
			setVisible(true);

		// return return value;
		return returnValue;
	}

	public int[] getCounterExample() {
		return counterEx;
	}

	public boolean conjectureIsEquivalent() {
		return conjectureIsEquivalent;
	}

	public static void printWord(int[] word) {
		if (word == null)
			System.out.println(word);

		System.out.print("'");
		for (int i : word)
			System.out.print(i + " ");
		System.out.println("'");
	}

}
