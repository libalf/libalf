package de.libalf.demo.gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.GridLayout;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import de.libalf.demo.Statistics;

public class StatisticsPanel extends JPanel implements ChangeListener {

	private static final long serialVersionUID = 1L;

	private Statistics statistics;

	private JLabel queryLabel, knowledgebaseLabel;

	public StatisticsPanel(Statistics statistics) {
		super();
		this.statistics = statistics;
		statistics.addChangeListener(this);

		buildGUI();
	}

	private void buildGUI() {
		/*
		 * Stuff
		 */
		setLayout(new BorderLayout());

		// main Panel
		JPanel mainPanel = new JPanel(new GridLayout(2, 1));
		add(mainPanel, BorderLayout.NORTH);

		/*
		 * Query Panel
		 */
		JPanel qPanel = new JPanel(new FlowLayout(FlowLayout.LEADING));
		qPanel.setBorder(new TitledBorder("Queries (accumulated)"));
		mainPanel.add(qPanel);
		queryLabel = new JLabel();
		qPanel.add(queryLabel, BorderLayout.CENTER);

		/*
		 * Knowledgebase
		 */
		JPanel kbPanel = new JPanel(new FlowLayout(FlowLayout.LEADING));
		kbPanel.setBorder(new TitledBorder("Knowledgebase"));
		mainPanel.add(kbPanel);
		knowledgebaseLabel = new JLabel();
		kbPanel.add(knowledgebaseLabel);

		/*
		 * Final
		 */
		updateQuerySatistics();
		updateKnowledgebaseStatistics();
	}

	private void updateQuerySatistics() {
		String text = "<html><body><ul>";

		text += "<li>Total queries: <b>"
				+ statistics.getIntValue("total_queries") + "</b>";
		if (!statistics.valueIsNull("total_queries")
				&& !statistics.valueIsNull("filtered_queries")
				&& !statistics.valueIsNull("user_queries")) {
			int pendingQueries = statistics.getIntValue("total_queries")
					- (statistics.getIntValue("filtered_queries") + statistics
							.getIntValue("user_queries"));
			text += (pendingQueries > 0 ? " (" + (pendingQueries)
					+ (pendingQueries == 1 ? " query" : " queries")
					+ " pending)" : "");
		}
		text += "</li><br>";

		text += "<li>Filtered queries: <b>"
				+ statistics.getIntValue("filtered_queries") + "</b></li><br>";

		text += "<li>User queries: <b>"
				+ statistics.getIntValue("user_queries") + "</li><br>";

		text += "</ul></body></html>";

		queryLabel.setText(text);
	}

	private void updateKnowledgebaseStatistics() {
		String text = "<html><body><ul>";

		text += "<li>Knowledge stored: <b>"
				+ statistics.getIntValue("knowledge_count")
				+ "</b> entries</li><br>";

		text += "<li>Queries stored: <b>"
				+ statistics.getIntValue("queries_count")
				+ "</b> queries</li><br>";

		text += "<li>Memory consumed: <b>"
				+ statistics.getIntValue("kb_memory_usage") + "</b> Byte</li>";

		text += "</ul></body></html>";

		knowledgebaseLabel.setText(text);
	}

	@Override
	public void stateChanged(ChangeEvent e) {
		updateQuerySatistics();
		updateKnowledgebaseStatistics();
	}

}
