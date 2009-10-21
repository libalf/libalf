package de.libalf.demo.gui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.TitledBorder;

import de.libalf.demo.filter.AutomatonFilter;
import de.libalf.demo.filter.Filter;
import de.libalf.demo.filter.Filter.Result;
import dk.brics.automaton.Automaton;

public class FilterEditor extends JDialog {

	private static final long serialVersionUID = 1L;

	private Filter filter = null;

	private Automaton acceptAutomaton = null, rejectAutomaton = null;

	private int returnValue = DemoDesktop.CANCEL;

	private JDialog acceptPreviewDialog = null, rejectPreviewDialog = null;

	private final static String NoAutomatonDefined = "<html><body><p><font color=#ff0000>No automaton selected.</font></p></body></html>";
	private final static String automatonDefined = "<html><body><p><font color=#00ff00>Automaton selected.</font></p></body></html>";

	public void buildGUI(Filter f) {
		/*
		 * Stuff
		 */
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		setTitle("Filter Editor");
		setLayout(new BorderLayout());
		setModal(true);
		setPreferredSize(new Dimension(250, 350));

		/*
		 * Description
		 */
		JPanel dPanel = new JPanel();
		add(dPanel, BorderLayout.NORTH);
		dPanel.setBorder(new TitledBorder("Description"));
		// Text field
		final JTextField descriptionField = new JTextField(20);
		if (f != null)
			descriptionField.setText(f.getDescription());
		dPanel.add(descriptionField);
		descriptionField.setToolTipText("Set the filter's description here");

		/*
		 * Automata panel
		 */
		JPanel aPanel = new JPanel(new GridLayout(2, 1));
		add(aPanel, BorderLayout.CENTER);

		/*
		 * accept Automaton
		 */
		// Panel
		JPanel accPanel = new JPanel(new BorderLayout());
		aPanel.add(accPanel);
		accPanel.setBorder(new TitledBorder("Accept Automaton"));

		// Info label
		JLabel accInfoLabel = new JLabel(
				"<html><body>Words accepted by this automaton are marked as "
						+ Result.ACCEPT + " by the filter.<br></body></html>");
		accPanel.add(accInfoLabel, BorderLayout.CENTER);

		// Acc bottomPanel
		JPanel accBottomPanel = new JPanel(new BorderLayout());
		accPanel.add(accBottomPanel, BorderLayout.SOUTH);

		// Status label
		final JLabel accStatusLabel = new JLabel(
				acceptAutomaton == null ? NoAutomatonDefined : automatonDefined);
		accBottomPanel.add(accStatusLabel, BorderLayout.CENTER);

		// Buttons
		JPanel accButtonPanel = new JPanel(new GridLayout(1, 2));
		accBottomPanel.add(accButtonPanel, BorderLayout.SOUTH);

		// Choose button
		JButton accChooseButton = new JButton("Choose ...");
		accChooseButton.setToolTipText("Open the Automaton Chooser Dialog");
		accButtonPanel.add(accChooseButton);

		// Preview button
		final JButton accPreviewButton = new JButton("Preview");
		accPreviewButton.setToolTipText("Show a preview of the automaton");
		accButtonPanel.add(accPreviewButton);
		accPreviewButton.setEnabled(acceptAutomaton == null ? false : true);

		// Actions
		accChooseButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				AutomatonEditor chooser = new AutomatonEditor();
				if (chooser.showAutomatonChooser() == DemoDesktop.OK) {
					acceptAutomaton = chooser.getAutomaton();
					accPreviewButton.setEnabled(true);
					accStatusLabel.setText(automatonDefined);
					acceptPreviewDialog = null;
				}
			}
		});
		accPreviewButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if (acceptPreviewDialog == null)
					acceptPreviewDialog = JGraphVisualizer
							.createVisualizationDialog(acceptAutomaton,
									FilterEditor.this);
				acceptPreviewDialog.setVisible(true);

			}
		});

		/*
		 * reject Automaton
		 */
		// Panel
		JPanel rejPanel = new JPanel(new BorderLayout());
		aPanel.add(rejPanel);
		rejPanel.setBorder(new TitledBorder("Reject Automaton"));
		// Info label
		JLabel rejInfoLabel = new JLabel(
				"<html><body>Words accepted by this automaton are marked as "
						+ Result.REJECT + " by the filter.<br></body></html>");
		rejPanel.add(rejInfoLabel, BorderLayout.CENTER);

		// Rej bottomPanel
		JPanel rejBottomPanel = new JPanel(new BorderLayout());
		rejPanel.add(rejBottomPanel, BorderLayout.SOUTH);

		// Status label
		final JLabel rejStatusLabel = new JLabel(
				rejectAutomaton == null ? NoAutomatonDefined : automatonDefined);
		rejBottomPanel.add(rejStatusLabel, BorderLayout.CENTER);

		// Buttons
		JPanel rejButtonPanel = new JPanel(new GridLayout(1, 2));
		rejBottomPanel.add(rejButtonPanel, BorderLayout.SOUTH);

		// Choose button
		JButton rejChooseButton = new JButton("Choose ...");
		rejChooseButton.setToolTipText("Open the Automaton Chooser Dialog");
		rejButtonPanel.add(rejChooseButton);

		// Preview button
		final JButton rejPreviewButton = new JButton("Preview");
		rejPreviewButton.setToolTipText("Show a preview of the automaton");
		rejButtonPanel.add(rejPreviewButton);
		rejPreviewButton.setEnabled(rejectAutomaton == null ? false : true);

		// Actions
		rejChooseButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				AutomatonEditor chooser = new AutomatonEditor();
				if (chooser.showAutomatonChooser() == DemoDesktop.OK) {
					rejectAutomaton = chooser.getAutomaton();
					rejPreviewButton.setEnabled(true);
					rejStatusLabel.setText(automatonDefined);
					rejectPreviewDialog = null;
				}
			}
		});
		rejPreviewButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if (rejectPreviewDialog == null)
					rejectPreviewDialog = JGraphVisualizer
							.createVisualizationDialog(rejectAutomaton,
									FilterEditor.this);
				rejectPreviewDialog.setVisible(true);

			}
		});

		/*
		 * Bottom panel
		 */
		JPanel bottomPanel = new JPanel(new BorderLayout());
		add(bottomPanel, BorderLayout.SOUTH);

		/*
		 * Info Panel
		 */
		JLabel infoLabel = new JLabel("Info text goes here.");
		bottomPanel.add(infoLabel, BorderLayout.CENTER);

		/*
		 * OK Button
		 */
		JPanel okPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
		okPanel
				.setToolTipText("Accepts the configuration and closes the dialog");
		bottomPanel.add(okPanel);
		JButton okButton = new JButton("OK");
		okPanel.add(okButton);
		okButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if (acceptAutomaton == null && rejectAutomaton == null) {
					JOptionPane.showMessageDialog(FilterEditor.this,
							"Please define at least one automaton.",
							"Automaton not defined",
							JOptionPane.WARNING_MESSAGE);
				} else {
					// check description
					if (descriptionField.getText() == null
							|| descriptionField.getText().equals("")) {
						int retValue = JOptionPane
								.showConfirmDialog(
										FilterEditor.this,
										"Do you really want to continue without a filter desciption?",
										"No description given",
										JOptionPane.OK_OPTION);
						if (retValue == JOptionPane.NO_OPTION)
							return;
					}

					// create filter and dispose
					String description = descriptionField.getText();
					if (description == null || description.equals(""))
						description = "Default automaton filter ("
								+ AutomatonFilter.number + ")";
					AutomatonFilter.number++;

					filter = new AutomatonFilter(acceptAutomaton,
							rejectAutomaton, description);
					returnValue = DemoDesktop.OK;
					dispose();
				}
			}
		});

		/*
		 * More stuff
		 */
		pack();
		setVisible(true);
	}

	public int showFilterChooser() {
		buildGUI(null);

		return returnValue;
	}

	public int showFilterChooser(AutomatonFilter f) {
		acceptAutomaton = f.getAcceptAutomaton();
		rejectAutomaton = f.getRejectAutomaton();
		buildGUI(f);

		return returnValue;
	}

	public Filter getFilter() {
		return filter;
	}

}
