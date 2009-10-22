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
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.Map;

import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JInternalFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.ListSelectionModel;
import javax.swing.border.BevelBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.InternalFrameAdapter;
import javax.swing.event.InternalFrameEvent;

import de.libalf.AlfException;
import de.libalf.BasicAutomaton;
import de.libalf.Knowledgebase;
import de.libalf.LearningAlgorithm;
import de.libalf.LibALFFactory;
import de.libalf.demo.Statistics;
import de.libalf.demo.Scenario;
import de.libalf.demo.Tools;
import de.libalf.demo.gui.SourceCodeLabel.State;
import de.libalf.dispatcher.DispatcherFactory;
import de.libalf.jni.JNIFactory;
import dk.brics.automaton.Automaton;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 */
public class DefaultOnlineScenarioFrame extends JInternalFrame {

	private static int dialogNumber = 0;

	private static final long serialVersionUID = 1L;

	private Scenario scenario;

	private boolean knowledgebaseHasChanged = true;

	private DefaultListModel logModel;
	private JList logList;
	private JLabel statusLabel;
	private JPanel knowledgebasePanel;
	private JButton advanceButton, membershipButton, equivalenceButton;
	private JTabbedPane tabbedPane;
	private OnlineSourceCodeLabel sourceCodeLabel;
	private MembershipQueryDialog membershipDialog = null;
	private EquivalenceQueryDialog equivalenceDialog = null;

	private LearningAlgorithm algorithm;
	private Knowledgebase knowledgebase;
	private Automaton conjecture = null;

	public DefaultOnlineScenarioFrame(Scenario scenario) {
		super();
		this.scenario = scenario;
		dialogNumber++;
		buildGUI();
	}

	private void buildGUI() {
		/*
		 * Stuff
		 */
		setTitle("libalf - " + scenario.getDescription() + " (" + dialogNumber
				+ ")");
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		setSize(800, 700);
		setResizable(true);
		setClosable(true);
		setMaximizable(true);
		setLayout(new BorderLayout());
		setIconifiable(true);

		/*
		 * Algorithm panel
		 */
		JPanel algorithmPanel = new JPanel(new BorderLayout());
		{
			/*
			 * Cource Code panel
			 */
			sourceCodeLabel = new OnlineSourceCodeLabel(scenario);
			JPanel tmpPanel = new JPanel(new FlowLayout(FlowLayout.LEADING));
			tmpPanel.setOpaque(true);
			tmpPanel.setBackground(Color.WHITE);
			tmpPanel.add(sourceCodeLabel);
			algorithmPanel.add(tmpPanel, BorderLayout.CENTER);

			/*
			 * Buttons
			 */
			JPanel p = new JPanel(new GridLayout(1, 3));
			algorithmPanel.add(p, BorderLayout.SOUTH);

			// Advance
			advanceButton = new JButton("Advance");
			p.add(advanceButton);
			advanceButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					DefaultOnlineScenarioFrame.this.advance();
				}
			});

			// Membership
			membershipButton = new JButton("Answer membership queries");
			p.add(membershipButton);
			membershipButton.setEnabled(false);
			membershipButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					DefaultOnlineScenarioFrame.this.answerMembershipQueries();
				}
			});

			// Equivalence
			equivalenceButton = new JButton("Answer equivalence query");
			p.add(equivalenceButton);
			equivalenceButton.setEnabled(false);
			equivalenceButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					DefaultOnlineScenarioFrame.this.answerEquivalenceQueries();
				}
			});
		}

		/*
		 * Knowledgebase panel
		 */
		knowledgebasePanel = new JPanel(new BorderLayout());

		/*
		 * TabbedPane
		 */
		{
			tabbedPane = new JTabbedPane(JTabbedPane.TOP);
			add(tabbedPane, BorderLayout.CENTER);
			tabbedPane.addTab("Algorithm", algorithmPanel);
			tabbedPane.addTab("Knowledgebase", knowledgebasePanel);
			tabbedPane.addTab("Statistics", new StatisticsPanel(scenario
					.getStatistics()));
			tabbedPane.addChangeListener(new ChangeListener() {
				public void stateChanged(ChangeEvent e) {
					// paint new if necessary
					if (tabbedPane.getSelectedIndex() == 1) {
						if (knowledgebaseHasChanged) {
							knowledgebaseHasChanged = false;
							knowledgebasePanel.removeAll();

							knowledgebasePanel
									.add(
											JGraphVisualizer
													.createVisualization(knowledgebase),
											BorderLayout.CENTER);
							knowledgebasePanel.revalidate();
							knowledgebasePanel.repaint();
						}
					}
				}
			});
		}

		/*
		 * Bottom panel
		 */
		JPanel bottomPanel = new JPanel(new BorderLayout());
		add(bottomPanel, BorderLayout.SOUTH);

		/*
		 * Log list
		 */
		logModel = new DefaultListModel();
		logList = new JList(logModel);
		logList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		bottomPanel.add(new JScrollPane(logList), BorderLayout.CENTER);

		/*
		 * Status bar
		 */
		statusLabel = new JLabel("OK");
		bottomPanel.add(statusLabel, BorderLayout.SOUTH);
		statusLabel.setBorder(new BevelBorder(1));

		/*
		 * More stuff
		 */
		setVisible(true);
		addInternalFrameListener(new InternalFrameAdapter() {
			public void internalFrameClosed(InternalFrameEvent e) {
				try {
					// Destroy libalf
					if (algorithm != null)
						algorithm.destroy();
					if (knowledgebase != null)
						knowledgebase.destroy();
				} catch (AlfException e1) {

				}
			}
		});

		initializeLibALF();
	}

	private void initializeLibALF() {
		try {
			// Create factory
			LibALFFactory factory;
			if (scenario.isJniConnection()) {

				factory = JNIFactory.STATIC;
				log("JNI factory created,");
			} else {
				factory = new DispatcherFactory(scenario.getServer(), scenario
						.getPort());
				log("Dispatcher factory created,");
			}

			// create knowledgebase
			knowledgebase = factory.createKnowledgebase();
			log("Knowledgebase created.");

			// create learning algorithm
			algorithm = factory.createLearningAlgorithm(
					scenario.getAlgorithm(), knowledgebase, scenario
							.getAlphabetSize());
			log(scenario.getAlgorithm() + " learning algorithm created.");

			/*
			 * Set statistics
			 */
			Statistics s = scenario.getStatistics();
			s.setIntValue("total_queries", 0);
			s.setIntValue("filtered_queries", 0);
			s.setIntValue("user_queries", 0);
			s.setIntValue("knowledge_count", knowledgebase.count_answers());
			s.setIntValue("queries_count", knowledgebase.count_queries());
			s.setIntValue("kb_memory_usage", knowledgebase.get_memory_usage());

		} catch (AlfException e) {
			JOptionPane.showMessageDialog(this, e, "libalf error occured",
					JOptionPane.ERROR_MESSAGE);
			dispose();
		} catch (UnsatisfiedLinkError e) {
			JOptionPane.showMessageDialog(this, e, "libalf error occured",
					JOptionPane.ERROR_MESSAGE);
			dispose();
		}
	}

	private void log(Object message) {
		logModel.addElement(GregorianCalendar.getInstance().getTime() + ": "
				+ message);
		logList.ensureIndexIsVisible(logModel.getSize() - 1);
	}

	public void advance() {
		try {
			BasicAutomaton c = (BasicAutomaton) algorithm.advance();
			log("Advancing ...");

			if (c != null) {
				conjecture = Tools.basicAutomaton2bricsAutomaton(c);
				equivalenceButton.setEnabled(true);
				sourceCodeLabel.changeState(State.EQUIVALENCE);
				log("Conjecture computed.");
			}
			if (knowledgebase.count_queries() > 0) {
				conjecture = null;
				membershipButton.setEnabled(true);
				sourceCodeLabel.changeState(State.MEMBERSHIP);
				log(knowledgebase.count_queries()
						+ " membership "
						+ (knowledgebase.count_queries() == 1 ? "query needs"
								: "queries need") + " to be answered.");

				// Statistics
				Statistics s = scenario.getStatistics();
				s.setIntValue("total_queries", s.getIntValue("total_queries")
						+ knowledgebase.count_queries());
			}

			/*
			 * Set statistics
			 */
			Statistics s = scenario.getStatistics();
			s.setIntValue("knowledge_count", knowledgebase.count_answers());
			s.setIntValue("queries_count", knowledgebase.count_queries());
			s.setIntValue("kb_memory_usage", knowledgebase.get_memory_usage());

			advanceButton.setEnabled(false);
			knowledgebaseHasChanged = true;

		} catch (AlfException e) {
			JOptionPane.showMessageDialog(this, e, "libalf error occured",
					JOptionPane.ERROR_MESSAGE);
			dispose();
		}
	}

	/**
	 * Performs the GUI (and libalf) actions to answer membership queries.
	 */
	private void answerMembershipQueries() {
		try {
			if (membershipDialog == null)
				membershipDialog = new MembershipQueryDialog(scenario,
						knowledgebase.get_queries());

			// Show dialog
			if (membershipDialog.showMembershipQueryDialog() == DemoDesktop.OK) {
				HashMap<int[], Boolean> resolvedQueries = membershipDialog
						.getAnswers();

				// Add answers to libalf
				for (Map.Entry<int[], Boolean> answers : resolvedQueries
						.entrySet())
					knowledgebase.add_knowledge(answers.getKey(), answers
							.getValue());

				membershipButton.setEnabled(false);
				advanceButton.setEnabled(true);
				sourceCodeLabel.changeState(State.ADVANCE);
				log(resolvedQueries.size() + " membership "
						+ (resolvedQueries.size() == 1 ? "query" : "queries")
						+ " answered.");
				knowledgebaseHasChanged = true;

				/*
				 * Set statistics
				 */
				Statistics s = scenario.getStatistics();
				s.setIntValue("filtered_queries", s
						.getIntValue("filtered_queries")
						+ membershipDialog.numberOfFilteredQueries());
				s.setIntValue("user_queries", s.getIntValue("user_queries")
						+ membershipDialog.numberOfUserQueries());
				s.setIntValue("knowledge_count", knowledgebase.count_answers());
				s.setIntValue("queries_count", knowledgebase.count_queries());
				s.setIntValue("kb_memory_usage", knowledgebase
						.get_memory_usage());

				membershipDialog = null;
			}

		} catch (AlfException e) {
			JOptionPane.showMessageDialog(this, e, "libalf error occured",
					JOptionPane.ERROR_MESSAGE);
			dispose();
		}

	}

	private void answerEquivalenceQueries() {
		try {
			if (equivalenceDialog == null)
				equivalenceDialog = new EquivalenceQueryDialog(scenario,
						conjecture, knowledgebase);

			// Show dialog
			if (equivalenceDialog.showEquivalenceQueryDialog() == DemoDesktop.OK) {
				// Add counter example to libalf
				if (!equivalenceDialog.conjectureIsEquivalent()) {
					algorithm.add_counterexample(equivalenceDialog
							.getCounterExample());
					advanceButton.setEnabled(true);
					sourceCodeLabel.changeState(State.ADVANCE);
					log("Counter-example added.");

				} else {
					// Show result
					advanceButton.setEnabled(false);
					log("Found automaton.");
					sourceCodeLabel.changeState(State.FINISH);
					tabbedPane.addTab("Result", JGraphVisualizer
							.createVisualization(conjecture));
					tabbedPane.setSelectedIndex(3);
				}

				conjecture = null;
				equivalenceButton.setEnabled(false);
				knowledgebaseHasChanged = true;
				equivalenceDialog = null;
			}

		} catch (AlfException e) {
			JOptionPane.showMessageDialog(this, e, "libalf error occured",
					JOptionPane.ERROR_MESSAGE);
			dispose();
		}
	}

}
