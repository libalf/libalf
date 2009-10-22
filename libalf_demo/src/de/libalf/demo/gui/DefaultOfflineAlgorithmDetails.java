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
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.Collection;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.table.AbstractTableModel;

import de.libalf.demo.Sample;
import de.libalf.demo.Scenario;
import de.libalf.demo.Tools;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 * 
 */
public class DefaultOfflineAlgorithmDetails extends JPanel {

	private static final long serialVersionUID = 1L;

	Scenario scenario;

	public DefaultOfflineAlgorithmDetails(Scenario scenario) {
		super();
		this.scenario = scenario;
		buildGUI();
	}

	private void buildGUI() {
		setLayout(new BorderLayout());
		setBorder(new TitledBorder("Samples"));

		/*
		 * List
		 */
		// Panel
		JPanel lPanel = new JPanel(new BorderLayout());
		add(lPanel, BorderLayout.CENTER);

		// Table
		final MySampleTableModel model = new MySampleTableModel(scenario);
		final JTable table = new JTable(model);
		lPanel.add(new JScrollPane(table), BorderLayout.CENTER);
		table.getColumnModel().getColumn(0).setPreferredWidth(180);

		/*
		 * Button panel
		 */
		JPanel bPanel = new JPanel(new BorderLayout());
		add(bPanel, BorderLayout.SOUTH);

		/*
		 * Load / Save
		 */
		JPanel leftBottomPanel = new JPanel(new GridLayout(1, 2));
		bPanel.add(leftBottomPanel, BorderLayout.WEST);
		// Load
		JButton loadButton = new JButton("Load");
		loadButton.setToolTipText("Load a previously saved sample list");
		leftBottomPanel.add(loadButton);
		loadButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				JFileChooser chooser = new JFileChooser();
				FileNameExtensionFilter filter = new FileNameExtensionFilter(
						"Sample list", "csv");
				chooser.setFileFilter(filter);
				int returnVal = chooser
						.showOpenDialog(DefaultOfflineAlgorithmDetails.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					model.addSamples(Tools.parseCSV(chooser.getSelectedFile()));
				}
			}
		});

		// Save
		JButton saveButton = new JButton("Save");
		saveButton.setToolTipText("Save the sample list");
		leftBottomPanel.add(saveButton);
		saveButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				JFileChooser chooser = new JFileChooser();
				FileNameExtensionFilter filter = new FileNameExtensionFilter(
						"Sample list", "csv");
				chooser.setFileFilter(filter);
				int returnVal = chooser
						.showSaveDialog(DefaultOfflineAlgorithmDetails.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					try {
						BufferedWriter writer = new BufferedWriter(
								new FileWriter(chooser.getSelectedFile()));
						for (Sample s : scenario.getSamples()) {
							writer.write(Sample.word2String(s.word) + ","
									+ (s.acceptance ? "1" : "0"));
							writer.newLine();
						}
						writer.close();
					} catch (Exception e) {
						JOptionPane.showMessageDialog(
								DefaultOfflineAlgorithmDetails.this, e,
								"Error", JOptionPane.ERROR_MESSAGE);
					}
				}
			}
		});

		/*
		 * Add / Remove
		 */
		JPanel rightBottomPanel = new JPanel(new GridLayout(1, 2));
		bPanel.add(rightBottomPanel, BorderLayout.EAST);

		// Add
		JButton addButton = new JButton("+");
		addButton.setToolTipText("Add a new sample to the list");
		rightBottomPanel.add(addButton);
		addButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				String input = JOptionPane
						.showInputDialog(
								DefaultOfflineAlgorithmDetails.this,
								new String[] { "Please input a new sample.",
										"You can change the acceptance of the new sample afterwards." },
								"New sample", JOptionPane.QUESTION_MESSAGE);
				if (input != null) {
					int[] word = Tools.string2libalfWord(input);
					if (word != null) {
						model.addSample(new Sample(word, false));
					}
				}
			}
		});

		// Remove
		JButton removeButton = new JButton("-");
		removeButton.setToolTipText("Remove the selected sample from the list");
		rightBottomPanel.add(removeButton);
		removeButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				model.removeSamples(table.getSelectedRows());
			}
		});

	}

	class MySampleTableModel extends AbstractTableModel {

		private static final long serialVersionUID = 1L;

		private Scenario scenario;

		public MySampleTableModel(Scenario scenario) {
			super();
			this.scenario = scenario;
		}

		public boolean addSample(Sample s) {
			scenario.addSample(s);
			fireTableDataChanged();
			return true;
		}

		public void addSamples(Collection<Sample> c) {
			for (Sample s : c)
				scenario.addSample(s);
			fireTableDataChanged();
		}

		public void removeSample(int index) {
			removeSamples(new int[] { index });
		}

		public void removeSamples(int[] indexes) {
			Sample[] samples = new Sample[indexes.length];
			for (int i = 0; i < indexes.length; i++)
				samples[i] = scenario.getSamples().get(indexes[i]);

			for (Sample s : samples)
				scenario.getSamples().remove(s);

			fireTableDataChanged();
		}

		public void clearSamples() {
			scenario.getSamples().clear();
			fireTableDataChanged();
		}

		@Override
		public int getColumnCount() {
			return 2;
		}

		@Override
		public int getRowCount() {
			return scenario.getSamples().size();
		}

		@Override
		public Object getValueAt(int row, int column) {
			if (column == 0) {
				if (scenario.getSamples().get(row).word.length == 0)
					return "Empty string";

				String ret = "";
				for (int i : scenario.getSamples().get(row).word)
					ret += i;
				return ret;
			} else if (column == 1)
				return scenario.getSamples().get(row).acceptance;
			return null;
		}

		@SuppressWarnings("unchecked")
		public Class getColumnClass(int c) {
			if (c == 0)
				return String.class;
			else if (c == 1)
				return Boolean.class;
			else
				return getValueAt(0, c).getClass();
		}

		@Override
		public String getColumnName(int column) {
			switch (column) {
			case 0:
				return "Sample";
			case 1:
				return "Acceptance";
			default:
				return "Unknown";
			}
		}

		@Override
		public void setValueAt(Object value, int rowIndex, int columnIndex) {
			scenario.getSamples().get(rowIndex).acceptance = (Boolean) value;
		}

		@Override
		public boolean isCellEditable(int rowIndex, int columnIndex) {
			if (columnIndex == 1)
				return true;
			else
				return false;

		}
	}
}
