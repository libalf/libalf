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
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.StringReader;
import java.util.HashMap;
import java.util.LinkedList;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JEditorPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.border.TitledBorder;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.table.TableCellRenderer;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.EditorKit;
import javax.swing.text.html.HTMLEditorKit;

import de.libalf.demo.Scenario;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 */
public class MembershipQueryDialog extends JDialog {

	private static final Color FILTERED_COLOR = new Color(Integer.parseInt(
			"afc7c7", 16));
	private static final Color CONFLICT_COLOR = new Color(Integer.parseInt(
			"f75d59", 16));
	private static final Color TEACHER_COLOR = new Color(Integer.parseInt(
			"a2ff8f", 16));

	private static final long serialVersionUID = 1L;

	private Scenario scenario;

	private LinkedList<int[]> queries;

	private MembershipQueryTableModel model;

	private int returnValue = DemoDesktop.CANCEL;

	private boolean guiIsBuild = false;

	public MembershipQueryDialog(Scenario scenario, LinkedList<int[]> queries) {
		super();
		this.scenario = scenario;
		this.queries = queries;
	}

	private void buildGUI() {
		/*
		 * Stuff
		 */
		setModal(true);
		setTitle("Membership queries");
		setSize(640, 480);
		// setResizable(false);
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		setLayout(new BorderLayout());

		/*
		 * Table
		 */
		model = new MembershipQueryTableModel(scenario, queries);
		JTable queryTable = new JTable(model);
		queryTable.getColumnModel().getColumn(0).setPreferredWidth(500);
		queryTable.getColumnModel().getColumn(0).setCellRenderer(
				new MyStringCellRenderer(queryTable
						.getDefaultRenderer(String.class)));
		queryTable.getColumnModel().getColumn(1).setCellRenderer(
				new MyStringCellRenderer(queryTable
						.getDefaultRenderer(Boolean.class)));

		/*
		 * Table Panel
		 */
		JPanel tPanel = new JPanel(new BorderLayout());
		tPanel.setBorder(new TitledBorder("Answer membership queries"));
		tPanel.add(new JScrollPane(queryTable), BorderLayout.CENTER);
		add(tPanel, BorderLayout.CENTER);

		/*
		 * Info
		 */
		// topPanel.setBorder(new TitledBorder("Info"));
		String simpleText = "<html>" + "<head>" + "<style>"
				+ "body {font-family:Calibri,Arial,sans-serif;}" + "</style>"
				+ "</head>" + "<body>";
		simpleText += "Answer the membership queries below by checking or unchecking the respective checkbox in the <b>Answer</b> column. Then proceed by clicking <b>OK</b>.<br><br>";
		simpleText += "<a href=\"more\">Read more ...</a>";
		simpleText += "</ul></body></html>";
		final JEditorPane editor = new JEditorPane();
		editor.setContentType("text/html");
		editor.setText(simpleText);
		editor.setEditable(false);
		editor.setOpaque(false);
		JScrollPane sPane = new JScrollPane(editor);
		sPane.setBorder(new TitledBorder("Info"));
		sPane
				.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		editor.addHyperlinkListener(new HyperlinkListener() {
			public void hyperlinkUpdate(HyperlinkEvent e) {
				if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
					String extendedText = "<html>" + "<head>" + "<style>"
							+ "body {font-family:Calibri,Arial,sans-serif;}"
							+ "</style>" + "</head>" + "<body>";
					extendedText += "<a name=\"top\"/>";
					extendedText += "Answer the membership queries below by checking or unchecking the respective checkbox in the <b>Answer</b> column. Then proceed by clicking <b>OK</b>.";
					extendedText += "<ul>";
					extendedText += "<li>A white background indicates <b>user queries</b>, which have to be answered by the user. By default such queries are classified as false.</li>";
					extendedText += "<li>A <font color=\"green\">green</font> background indicates queries answered by the teacher. Note that the classification of such queries can be altered, but the teacher support (in equivalence queries) will not work properly.</li>";
					extendedText += "<li>A gray background indicates <b>filtered queries</b>. Their classification cannot be changed since they would not be asked in a real application.</li>";
					extendedText += "<li>A <font color=\"#f75d59\">red</font> background indicates a filter conflict, which needs to be resolved by the user.</li>";
					extendedText += "</ul></body></html>";
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

		/*
		 * Top panel
		 */
		JPanel topPanel = new JPanel(new GridBagLayout());
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
		c.weighty = 0.5;
		c.weightx = 1.0;
		topPanel.add(tPanel, c);

		/*
		 * Buttons
		 */
		{
			// panel
			JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.TRAILING));
			add(buttonPanel, BorderLayout.SOUTH);

			// OK
			JButton ok = new JButton("OK");
			buttonPanel.add(ok);
			ok.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					returnValue = DemoDesktop.OK;
					dispose();
				}
			});
		}

		/*
		 * More stuff
		 */
		guiIsBuild = true;
		setVisible(true);
	}

	public int showMembershipQueryDialog() {
		// Show dialog
		if (!guiIsBuild)
			buildGUI();
		else
			setVisible(true);

		return returnValue;
	}

	public HashMap<int[], Boolean> getAnswers() {
		return model.getAnswers();
	}

	public int numberOfFilteredQueries() {
		return model.numberOfFilteredQueries();
	}

	public int numberOfUserQueries() {
		return model.numberOfUserQueries();
	}

	class MyStringCellRenderer implements TableCellRenderer {

		TableCellRenderer oldRenderer;

		public MyStringCellRenderer(TableCellRenderer r) {
			oldRenderer = r;
		}

		@Override
		public Component getTableCellRendererComponent(JTable table,
				Object value, boolean isSelected, boolean hasFocus, int row,
				int column) {
			Component c = oldRenderer.getTableCellRendererComponent(table,
					value, isSelected, hasFocus, row, column);

			switch (model.getResult(row)) {
			case CONFLICT:
				c.setBackground(CONFLICT_COLOR);
				break;
			case UNKNOWN:
				if (scenario.isUseTeacher() && scenario.getTeacher() != null) {
					c.setBackground(TEACHER_COLOR);
				} else {
					c.setBackground(Color.WHITE);
				}
				break;
			default:
				c.setBackground(FILTERED_COLOR);
			}
			return c;
		}
	}
}