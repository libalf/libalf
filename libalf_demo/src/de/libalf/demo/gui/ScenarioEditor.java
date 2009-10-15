package de.libalf.demo.gui;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.FileOutputStream;
import java.io.ObjectOutputStream;

import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JEditorPane;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.SpinnerNumberModel;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.filechooser.FileNameExtensionFilter;

import de.libalf.LibALFFactory.Algorithm;
import de.libalf.demo.Scenario;
import de.libalf.demo.Tools;

public class ScenarioEditor extends JDialog {

	public static final int CANCEL = 0;
	public static final int NEW_DEFAULT_ONLINE = 1;
	public static final int NEW_DEFAULT_OFFLINE = 2;

	private static final long serialVersionUID = 1L;

	private Scenario scenario;

	private int returnValue = CANCEL;

	private JPanel algorithmDetailsPanel;

	public ScenarioEditor() {
		scenario = Scenario.createDefaultScenario();
	}

	public ScenarioEditor(Scenario scenario) {
		this.scenario = scenario == null ? Scenario.createDefaultScenario()
				: scenario;
	}

	private void buildGUI() {
		/*
		 * Stuff
		 */
		setSize(300, 700);
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		setTitle("Scenario Editor");
		setLayout(new BorderLayout());
		setModal(true);

		/*
		 * Top Panel
		 */
		JPanel topPanel = new JPanel(new BorderLayout());
		add(topPanel, BorderLayout.NORTH);

		/*
		 * Connectivity
		 */
		{
			JPanel connectivityPanel = new JPanel(new GridLayout(3, 1));
			connectivityPanel
					.setBorder(new TitledBorder("libalf Connectivity"));
			topPanel.add(connectivityPanel, BorderLayout.NORTH);
			ButtonGroup bg = new ButtonGroup();

			// JNI Radiobutton
			JRadioButton jni = new JRadioButton("JNI");
			connectivityPanel.add(jni);
			bg.add(jni);

			// Dispatcher
			JRadioButton dispatcher = new JRadioButton("Dispatcher", true);
			connectivityPanel.add(dispatcher);
			bg.add(dispatcher);

			// Server textfield and port spinner
			JPanel dispatcherPanel = new JPanel(new BorderLayout());
			connectivityPanel.add(dispatcherPanel);

			// Server textfield
			final JTextField serverField = new JTextField();
			dispatcherPanel.add(serverField, BorderLayout.CENTER);
			serverField.setText(scenario.getServer());
			serverField.addKeyListener(new KeyListener() {
				public void keyTyped(KeyEvent e) {
				}

				public void keyReleased(KeyEvent e) {
					scenario.setServer(serverField.getText());
				}

				public void keyPressed(KeyEvent e) {
				}
			});

			// Port spinner
			final SpinnerNumberModel model = new SpinnerNumberModel(scenario
					.getPort(), 1, 65000, 1);
			final JSpinner portSpinner = new JSpinner(model);
			dispatcherPanel.add(portSpinner, BorderLayout.EAST);
			portSpinner.addChangeListener(new ChangeListener() {
				public void stateChanged(ChangeEvent e) {
					scenario.setPort((Integer) model.getValue());
				}
			});

			if (scenario.isJniConnection()) {
				jni.setSelected(true);
				serverField.setEnabled(false);
				portSpinner.setEnabled(false);
			} else {
				dispatcher.setSelected(true);
			}

			// Actions
			jni.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					serverField.setEnabled(false);
					portSpinner.setEnabled(false);
					scenario.setJniConnection(true);
				}
			});

			dispatcher.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					serverField.setEnabled(true);
					portSpinner.setEnabled(true);
					scenario.setJniConnection(false);
				}
			});

		}

		/*
		 * Alphabet Size
		 */
		JPanel alphabetPanel = new JPanel();
		final SpinnerNumberModel alphabetModel = new SpinnerNumberModel(
				scenario.getAlphabetSize(), 1, 10, 1);
		JSpinner alphabetSpinner = new JSpinner(alphabetModel);
		alphabetPanel.add(alphabetSpinner);
		alphabetPanel.setBorder(new TitledBorder("Alphabet size"));
		alphabetModel.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				scenario.setAlphabetSize((Integer) alphabetModel.getValue());
			}
		});

		/*
		 * Algorithm
		 */
		final JPanel algorithmPanel = new JPanel();
		algorithmPanel.setBorder(new TitledBorder("Algorithm"));
		// topPanel.add(algorithmPanel, BorderLayout.SOUTH);

		// JCombobox
		final JComboBox box = new JComboBox(new Object[] { Algorithm.ANGLUIN,
				Algorithm.ANGLUIN_COLUMN, Algorithm.BIERMANN_MINISAT,
				Algorithm.DELETE2, "KEARNS/VAZIRANI", Algorithm.NL_STAR,
				Algorithm.RPNI });
		algorithmPanel.add(box);
		box.setSelectedItem(scenario.getAlgorithm());
		box.addActionListener(new MyJComboBoxListener(box, scenario
				.getAlgorithm()));

		/*
		 * Top-Center-Panel
		 */
		JPanel tcPanel = new JPanel(new GridLayout(2, 1));
		topPanel.add(tcPanel, BorderLayout.CENTER);
		tcPanel.add(alphabetPanel);
		tcPanel.add(algorithmPanel);

		/*
		 * More options
		 */
		String text = "<html>" + "<head>" + "<style>" + "body {"
				+ "font-family:Calibri,Arial,sans-serif; padding: 5px;}"
				+ "</style>" + "</head>" + "<body>";
		text += "<a href=\"more\">More options ...</a><br></body></html>";
		JEditorPane editor = new JEditorPane();
		editor.setContentType("text/html");
		editor.setText(text);
		editor.setOpaque(false);
		editor.setEditable(false);
		editor.addHyperlinkListener(new HyperlinkListener() {
			public void hyperlinkUpdate(HyperlinkEvent arg0) {
				if (arg0.getEventType().equals(
						HyperlinkEvent.EventType.ACTIVATED)) {
					Object retVal = JOptionPane.showInputDialog(
							ScenarioEditor.this,
							"Please enter a scenario description", scenario
									.getDescription());
					if (retVal instanceof String)
						scenario.setDescription((String) retVal);
				}
			}
		});
		
		/*
		 * Init code window panel
		 */
		JCheckBox initCodeBox = new JCheckBox("Show code", false);
		
		/*
		 * More options panel
		 */
		JPanel moreOptionsPanel = new JPanel(new BorderLayout());
		topPanel.add(moreOptionsPanel, BorderLayout.SOUTH);
		moreOptionsPanel.add(editor, BorderLayout.WEST);
		moreOptionsPanel.add(initCodeBox, BorderLayout.EAST);
		
		/*
		 * Algorithm Details Panel
		 */
		{
			algorithmDetailsPanel = new JPanel(new BorderLayout());
			algorithmDetailsPanel.setBorder(new TitledBorder(
					"Algorithm Details"));
			add(algorithmDetailsPanel, BorderLayout.CENTER);

			switch (scenario.getAlgorithm()) {
			case ANGLUIN:
			case ANGLUIN_COLUMN:
			case NL_STAR:
				algorithmDetailsPanel.add(new DefaultOnlineAlgorithmDetails(
						scenario));
				break;
			case RPNI:
			case BIERMANN_MINISAT:
			case DELETE2:
				algorithmDetailsPanel.add(new DefaultOfflineAlgorithmDetails(
						scenario), BorderLayout.CENTER);
				break;
			default:
				JOptionPane.showMessageDialog(ScenarioEditor.this,
						"Unknown learning algorithm",
						"Unknown learning algorithm",
						JOptionPane.WARNING_MESSAGE);
				break;
			}
		}

		/*
		 * Buttons at the bottom
		 */
		{
			JPanel buttonPanel = new JPanel(new GridLayout(1, 3));
			add(buttonPanel, BorderLayout.SOUTH);

			// OK Button
			JButton newButton = new JButton("New");
			buttonPanel.add(newButton);
			newButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					if (scenario.getAlgorithm().equals(
							Algorithm.BIERMANN_MINISAT)
							|| scenario.getAlgorithm().equals(Algorithm.RPNI)
							|| scenario.getAlgorithm()
									.equals(Algorithm.DELETE2)) {
						returnValue = NEW_DEFAULT_OFFLINE;
						dispose();
					} else {

						// Check if teacher has invalid characters
						if (scenario.isUseTeacher()
								&& scenario.getTeacher() != null
								&& !Tools.checkAlphabetSize(scenario
										.getTeacher(), scenario
										.getAlphabetSize()))
							JOptionPane
									.showMessageDialog(
											ScenarioEditor.this,
											"The teacher automaton contains transition labels that are not in the alphabet.",
											"Invalid teacher automaton",
											JOptionPane.WARNING_MESSAGE);
						else {
							returnValue = NEW_DEFAULT_ONLINE;
							dispose();
						}
					}
				}
			});

			// Save button
			JButton saveButton = new JButton("Save");
			buttonPanel.add(saveButton);
			saveButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JFileChooser chooser = new JFileChooser();
					FileNameExtensionFilter filter = new FileNameExtensionFilter(
							"Scenarios", "scenario");
					chooser.setFileFilter(filter);
					int returnVal = chooser.showSaveDialog(ScenarioEditor.this);
					if (returnVal == JFileChooser.APPROVE_OPTION) {
						try {
							ObjectOutputStream oos = new ObjectOutputStream(
									new FileOutputStream(chooser
											.getSelectedFile()));
							oos.writeObject(scenario);
						} catch (Exception exception) {
							JOptionPane.showMessageDialog(ScenarioEditor.this,
									exception, "Error",
									JOptionPane.ERROR_MESSAGE);
						}
					}
				}
			});

			// Cancel Button
			JButton cancelButton = new JButton("Cancel");
			buttonPanel.add(cancelButton);
			cancelButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					dispose();
				}
			});
		}

		/*
		 * More stuff
		 */
		setVisible(true);
	}

	public int showScenarioEditor() {
		buildGUI();
		return this.returnValue;
	}

	public Scenario getScenario() {
		return scenario;
	}

	private class MyJComboBoxListener implements ActionListener {

		JComboBox box;
		Algorithm lastChoise;

		public MyJComboBoxListener(JComboBox box, Algorithm current) {
			this.box = box;
			lastChoise = current;
		}

		@Override
		public void actionPerformed(ActionEvent arg0) {
			if (!(box.getSelectedItem() instanceof Algorithm)) {
				JOptionPane
						.showMessageDialog(
								ScenarioEditor.this,
								"The Kearns / Vazirani learning algorithm is not yet supported by this GUI.",
								"Warning", JOptionPane.WARNING_MESSAGE);
				box.setSelectedItem(lastChoise);
				return;
			}

			scenario.setAlgorithm((Algorithm) box.getSelectedItem());
			lastChoise = scenario.getAlgorithm();
			algorithmDetailsPanel.removeAll();

			switch (scenario.getAlgorithm()) {
			case ANGLUIN:
			case ANGLUIN_COLUMN:
			case NL_STAR:
				algorithmDetailsPanel.add(new DefaultOnlineAlgorithmDetails(
						scenario), BorderLayout.CENTER);
				break;
			case RPNI:
			case BIERMANN_MINISAT:
			case DELETE2:
				algorithmDetailsPanel.add(new DefaultOfflineAlgorithmDetails(
						scenario), BorderLayout.CENTER);
				break;
			default:
				JOptionPane.showMessageDialog(ScenarioEditor.this,
						"Unknown learning algorithm",
						"Unknown learning algorithm",
						JOptionPane.WARNING_MESSAGE);
				break;
			}

			algorithmDetailsPanel.revalidate();
			algorithmDetailsPanel.repaint();

		}
	}
}
