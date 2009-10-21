package de.libalf.demo.gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.GregorianCalendar;
import java.util.concurrent.ExecutionException;

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
import javax.swing.SwingWorker;
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
import de.libalf.demo.Sample;
import de.libalf.demo.Statistics;
import de.libalf.demo.Scenario;
import de.libalf.demo.Tools;
import de.libalf.demo.gui.SourceCodeLabel.State;
import de.libalf.dispatcher.DispatcherFactory;
import de.libalf.jni.JNIFactory;
import dk.brics.automaton.Automaton;

public class DefaultOfflineScenarioFrame extends JInternalFrame {

	private static final long serialVersionUID = 1L;

	private static int dialogNumber = 0;

	private Scenario scenario;

	private DefaultListModel logModel;

	private Knowledgebase knowledgebase;
	private LearningAlgorithm algorithm;

	private OfflineSourceCodeLabel sourceCodeLabel;
	private JList logList;
	private JLabel statusLabel;

	private boolean knowledgebasePainted = false;

	public DefaultOfflineScenarioFrame(Scenario scenario) {
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
		setIconifiable(true);
		setLayout(new BorderLayout());

		/*
		 * Algorithm panel
		 */
		JPanel algorithmPanel = new JPanel(new BorderLayout());

		/*
		 * Cource Code panel
		 */
		sourceCodeLabel = new OfflineSourceCodeLabel(scenario);
		JPanel tmpPanel = new JPanel(new FlowLayout(FlowLayout.LEADING));
		tmpPanel.setOpaque(true);
		tmpPanel.setBackground(java.awt.Color.WHITE);
		tmpPanel.add(sourceCodeLabel);
		algorithmPanel.add(tmpPanel, BorderLayout.CENTER);

		/*
		 * Buttons
		 */
		JPanel p = new JPanel(new GridLayout(1, 3));
		algorithmPanel.add(p, BorderLayout.SOUTH);

		// Advance
		final JButton advanceButton = new JButton("Advance");
		p.add(advanceButton);

		/*
		 * Knowledgebase panel
		 */
		final JPanel knowledgebasePanel = new JPanel(new BorderLayout());

		/*
		 * TabbedPane
		 */

		final JTabbedPane tabbedPane = new JTabbedPane(JTabbedPane.TOP);
		add(tabbedPane, BorderLayout.CENTER);
		tabbedPane.addTab("Algorithm", algorithmPanel);
		tabbedPane.addTab("Knowledgebase", knowledgebasePanel);
		tabbedPane.addTab("Statistics", new StatisticsPanel(scenario
				.getStatistics()));
		tabbedPane.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				// paint if necessary
				if (tabbedPane.getSelectedIndex() == 1) {
					if (!knowledgebasePainted) {
						knowledgebasePainted = true;
						knowledgebasePanel.add(JGraphVisualizer
								.createVisualization(knowledgebase),
								BorderLayout.CENTER);
						knowledgebasePanel.revalidate();
						knowledgebasePanel.repaint();
					}
				}
			}
		});

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
		 * Actions
		 */
		advanceButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				log("Advancing ...");
				advanceButton.setEnabled(false);

				// Advance
				AdvanceSwingWorker worker = new AdvanceSwingWorker();
				worker.execute();
				Automaton result = null;
				try {
					result = worker.get();
				} catch (InterruptedException e) {
					e.printStackTrace();
				} catch (ExecutionException e) {
					e.printStackTrace();
				}
				log("Result computed.");

				sourceCodeLabel.changeState(State.FINISH);

				// Display result
				tabbedPane.addTab("Result", JGraphVisualizer
						.createVisualization(result));
				tabbedPane.setSelectedIndex(3);

			}
		});

		/*
		 * more stuff
		 */
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

		setVisible(true);
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
			 * Add samples
			 */
			for (Sample s : scenario.getSamples()) {
				knowledgebase.add_knowledge(s.word, s.acceptance);
			}
			log(scenario.getSamples().size()
					+ (scenario.getSamples().size() == 1 ? " sample"
							: " samples") + " added.");

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

	class AdvanceSwingWorker extends SwingWorker<Automaton, Object> {

		@Override
		protected Automaton doInBackground() throws Exception {
			try {
				Automaton result = Tools
						.basicAutomaton2bricsAutomaton((BasicAutomaton) DefaultOfflineScenarioFrame.this.algorithm
								.advance());
				return result;
			} catch (AlfException e) {
				JOptionPane.showMessageDialog(DefaultOfflineScenarioFrame.this,
						e, "libalf error occured", JOptionPane.ERROR_MESSAGE);
				DefaultOfflineScenarioFrame.this.dispose();
			}
			return null;
		}
	}
}
