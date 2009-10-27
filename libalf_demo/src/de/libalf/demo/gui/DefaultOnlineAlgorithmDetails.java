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
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.ButtonGroup;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.border.TitledBorder;

import de.libalf.demo.Scenario;
import de.libalf.demo.filter.AutomatonFilter;
import de.libalf.demo.filter.Filter;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 */
public class DefaultOnlineAlgorithmDetails extends JPanel {

	private static final long serialVersionUID = 1L;

	private Scenario scenario;

	private JDialog previewDialog = null;

	public DefaultOnlineAlgorithmDetails(Scenario scenario) {
		super();
		this.scenario = scenario;

		buildGUI();
	}

	private void buildGUI() {
		/*
		 * Stuff
		 */
		setLayout(new BorderLayout());

		/*
		 * Teacher
		 */
		{
			JPanel teacherPanel = new JPanel(new GridLayout(3, 1));
			teacherPanel.setBorder(new TitledBorder("Teacher"));
			add(teacherPanel, BorderLayout.NORTH);
			ButtonGroup bg = new ButtonGroup();

			// None Button
			final JRadioButton noneButton = new JRadioButton(
					"None (user guided)");
			teacherPanel.add(noneButton);
			bg.add(noneButton);

			// From Automaton
			final JRadioButton fromAutomaton = new JRadioButton(
					"From Automaton");
			teacherPanel.add(fromAutomaton);
			bg.add(fromAutomaton);

			/*
			 * Automaton panel
			 */
			{
				JPanel automatonPanel = new JPanel(new BorderLayout());
				teacherPanel.add(automatonPanel);

				// Button panel
				JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.LEADING));
				automatonPanel.add(buttonPanel, BorderLayout.WEST);

				// Choose Button
				final JButton choose = new JButton("Choose");
				buttonPanel.add(choose);

				// Preview Button
				final JButton preview = new JButton("Preview");
				buttonPanel.add(preview);

				/*
				 * Actions
				 */
				if (scenario.isUseTeacher()) {
					fromAutomaton.setSelected(true);
					if (scenario.getTeacher() == null) {
						preview.setEnabled(false);
					}
				} else {
					noneButton.setSelected(true);
					preview.setEnabled(false);
					choose.setEnabled(false);
				}

				noneButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						scenario.setUseTeacher(false);
						choose.setEnabled(false);
						preview.setEnabled(false);
					}
				});

				fromAutomaton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						scenario.setUseTeacher(true);
						choose.setEnabled(true);

						// Show load dialog
						if (scenario.getTeacher() == null) {
							AutomatonEditor chooser = new AutomatonEditor(
									scenario.getAlphabetSize());
							if (chooser.showAutomatonChooser() == DemoDesktop.OK) {
								scenario.setTeacher(chooser.getAutomaton());
								previewDialog = null;
							}
						}

						if (scenario.getTeacher() != null)
							preview.setEnabled(true);
					}
				});

				choose.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						AutomatonEditor chooser = new AutomatonEditor(scenario
								.getAlphabetSize());
						if (chooser.showAutomatonChooser() == DemoDesktop.OK) {
							scenario.setTeacher(chooser.getAutomaton());
							preview.setEnabled(true);
							previewDialog = null;
						}
					}
				});

				preview.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						if (previewDialog == null) {
							previewDialog = new JDialog();
							previewDialog.setTitle("Preview");
							previewDialog
									.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
							previewDialog.setLayout(new BorderLayout());
							previewDialog
									.getContentPane()
									.add(
											JGraphVisualizer
													.createVisualization(scenario
															.getTeacher()));
							previewDialog.setModal(true);
							previewDialog.setSize(640, 480);
						}
						previewDialog.setVisible(true);
					}
				});
			}

			/*
			 * Filters
			 */
			{
				JPanel filterPanel = new JPanel(new BorderLayout());
				filterPanel.setBorder(new TitledBorder("Filters"));
				add(filterPanel, BorderLayout.CENTER);

				// List model
				final DefaultListModel model = new DefaultListModel();
				for (Filter f : scenario.getFilters())
					model.addElement(f);

				// List
				final JList list = new JList(model);
				JScrollPane scrollPane = new JScrollPane(list);
				filterPanel.add(scrollPane, BorderLayout.CENTER);
				list.addMouseListener(new MouseAdapter() {
					public void mouseClicked(MouseEvent e) {
						if (e.getClickCount() == 2) {
							if (list.getSelectedIndex() >= 0) {
								FilterEditor editor = new FilterEditor();
								AutomatonFilter f = (AutomatonFilter) model
										.getElementAt(list.getSelectedIndex());
								if (editor.showFilterChooser(f) == DemoDesktop.OK) {
									model.setElementAt(editor.getFilter(), list
											.getSelectedIndex());
								}
							}
						}
					}
				});

				// Button panel
				JPanel outerButtonPanel = new JPanel(new BorderLayout());
				filterPanel.add(outerButtonPanel, BorderLayout.SOUTH);
				JPanel innerButtonPanel = new JPanel(new GridLayout(1, 2));
				outerButtonPanel.add(innerButtonPanel, BorderLayout.EAST);

				// Add
				JButton addButton = new JButton("+");
				innerButtonPanel.add(addButton);
				addButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent arg0) {
						FilterEditor chooser = new FilterEditor();
						if (chooser.showFilterChooser() == DemoDesktop.OK) {
							Filter f = chooser.getFilter();
							model.addElement(f);
							scenario.addFilter(f);
						}
					}
				});

				// Remove
				JButton removeButton = new JButton("-");
				innerButtonPanel.add(removeButton);
				removeButton.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						if (list.getSelectedIndex() >= 0) {
							scenario.removeFilter((Filter) list
									.getSelectedValue());
							model.remove(list.getSelectedIndex());
						}
					}
				});
			}
		}
	}
}
