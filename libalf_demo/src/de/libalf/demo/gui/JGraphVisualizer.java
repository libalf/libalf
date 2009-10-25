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
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.GeneralPath;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.Map;
import java.util.Map.Entry;

import javax.swing.DefaultBoundedRangeModel;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.SwingConstants;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.jgraph.JGraph;
import org.jgraph.graph.AttributeMap;
import org.jgraph.graph.CellViewRenderer;
import org.jgraph.graph.DefaultCellViewFactory;
import org.jgraph.graph.DefaultEdge;
import org.jgraph.graph.DefaultGraphCell;
import org.jgraph.graph.DefaultGraphModel;
import org.jgraph.graph.DefaultPort;
import org.jgraph.graph.EdgeView;
import org.jgraph.graph.GraphCell;
import org.jgraph.graph.GraphConstants;
import org.jgraph.graph.GraphLayoutCache;
import org.jgraph.graph.VertexRenderer;
import org.jgraph.graph.VertexView;
import org.jgraph.util.ParallelEdgeRouter;

import com.jgraph.layout.JGraphFacade;
import com.jgraph.layout.JGraphLayout;
import com.jgraph.layout.hierarchical.JGraphHierarchicalLayout;
import com.jgraph.layout.tree.JGraphTreeLayout;

import de.libalf.Knowledgebase;
import dk.brics.automaton.Automaton;
import dk.brics.automaton.State;
import dk.brics.automaton.Transition;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 */
public class JGraphVisualizer extends JPanel {

	private static final long serialVersionUID = 1L;

	public static final int STATE_SIZE = 40;

	private JGraph graph;

	private JGraphVisualizer(Automaton automaton) {
		super();
		setLayout(new BorderLayout());

		/*
		 * Create JGraph
		 */
		GraphAndRootNode gn = createGraph(automaton);
		graph = gn.graph;

		JGraphHierarchicalLayout layout = new JGraphHierarchicalLayout();
		layout.setOrientation(SwingConstants.WEST);
		applyLayout(graph, gn.root, layout);

		moveGraph(graph, 0, 100);
		graph.refresh();

		add(graph, BorderLayout.CENTER);
	}

	private JGraphVisualizer(Knowledgebase knowledgebase) {
		super();
		setLayout(new BorderLayout());

		/*
		 * Create JGraph
		 */
		GraphAndRootNode gn = createGraph(knowledgebase);
		graph = gn.graph;
		
		JGraphTreeLayout layout = new JGraphTreeLayout();
		layout.setOrientation(SwingConstants.WEST);
		applyLayout(graph, gn.root, layout);
		moveGraph(graph, 100, 0);

		add(graph, BorderLayout.CENTER);
	}

	private GraphAndRootNode createGraph(Automaton automaton) {
		/*
		 * Create states
		 */
		HashMap<State, GraphCell> stateMap = new HashMap<State, GraphCell>();
		LinkedList<GraphCell> states = new LinkedList<GraphCell>();
		for (State s : automaton.getStates()) {

			DefaultGraphCell cell;
			if (s.isAccept()) {
				if (s.equals(automaton.getInitialState())) {
					cell = new InitialFinalStateCell();
				} else {
					cell = new FinalStateCell();
				}
			} else {
				if (s.equals(automaton.getInitialState())) {
					cell = new InitialStateCell();
				} else {
					cell = new StateCell();
				}
			}
			cell.add(new DefaultPort());

			if (cell instanceof InitialStateCell
					|| cell instanceof InitialFinalStateCell)
				GraphConstants.setBounds(cell.getAttributes(),
						new Rectangle2D.Double(0, 0, STATE_SIZE + 15,
								STATE_SIZE));
			else
				GraphConstants.setBounds(cell.getAttributes(),
						new Rectangle2D.Double(0, 0, STATE_SIZE, STATE_SIZE));
			GraphConstants.setSizeable(cell.getAttributes(), false);

			stateMap.put(s, cell);
			states.add(cell);
		}

		/*
		 * Create transitions
		 */
		int arrow = GraphConstants.ARROW_CLASSIC;
		for (State s : automaton.getStates()) {
			for (Transition t : s.getTransitions()) {
				DefaultEdge edge;
				if (t.getMin() != t.getMax()) {
					edge = new DefaultEdge(t.getMin() + " - " + t.getMax());
				} else {
					edge = new DefaultEdge(t.getMin());
				}
				edge.setSource(((DefaultGraphCell) stateMap.get(s))
						.getChildAt(0));
				edge.setTarget(((DefaultGraphCell) stateMap.get(t.getDest()))
						.getChildAt(0));

				AttributeMap m = edge.getAttributes();
				GraphConstants.setLabelAlongEdge(m, true);
				GraphConstants.setLineEnd(m, arrow);
				GraphConstants.setEndFill(m, true);
				GraphConstants.setRouting(m, ParallelEdgeRouter
						.getSharedInstance());
				GraphConstants.setLineStyle(m, GraphConstants.STYLE_SPLINE);
				GraphConstants.setConnectable(m, false);
				GraphConstants.setDisconnectable(m, false);
				GraphConstants.setEditable(m, false);
				edge.setAttributes(m);

				states.add(edge);
			}
		}

		DefaultGraphModel model = new DefaultGraphModel();
		GraphLayoutCache view = new GraphLayoutCache(model,
				new DefaultCellViewFactory() {
					private static final long serialVersionUID = 1L;

					protected VertexView createVertexView(Object cell) {
						if (cell instanceof FinalStateCell)
							return new FinalStateView(cell);
						else if (cell instanceof InitialFinalStateCell)
							return new InitialFinalStateView(cell);
						else if (cell instanceof InitialStateCell)
							return new InitialStateView(cell);
						else if (cell instanceof StateCell)
							return new StateView(cell);
						else
							return super.createVertexView(cell);
					}
				});

		JGraph graph = new JGraph(model, view);
		graph.setAntiAliased(true);
		graph.setMoveBelowZero(false);
		graph.getGraphLayoutCache().insert(states.toArray());

		// Construct Return value
		GraphAndRootNode gn = new GraphAndRootNode();
		gn.graph = graph;
		gn.root = stateMap.get(automaton.getInitialState());

		return gn;
	}

	private GraphAndRootNode createGraph(Knowledgebase knowledgebase) {
		LinkedList<GraphCell> cells = new LinkedList<GraphCell>();

		/*
		 * Create nodes
		 */
		HashMap<String, Node> nodes = parseKnowledgebase(knowledgebase);
		HashMap<String, GraphCell> cellMap = new HashMap<String, GraphCell>(
				nodes.size());
		for (Entry<String, Node> entry : nodes.entrySet()) {
			DefaultGraphCell cell = new DefaultGraphCell(entry.getKey());
			cell.add(new DefaultPort());

			AttributeMap map = cell.getAttributes();
			GraphConstants.setSizeable(map, false);
			GraphConstants.setBounds(map, new Rectangle2D.Double(0, 0, entry
					.getKey().length() / 2 * 15 + 25, 30));
			GraphConstants.setBorderColor(map, Color.black);
			switch (entry.getValue().marked) {
			case Node.MARKED_DONTCARE:
				GraphConstants.setGradientColor(map, Color.gray);
				break;
			case Node.MARKED_ANSWERED:
				if (entry.getValue().answer)
					GraphConstants.setGradientColor(map, Color.green);
				else
					GraphConstants.setGradientColor(map, Color.red);
				break;
			case Node.MARKED_QUERY:
				GraphConstants.setGradientColor(map, Color.orange);
				break;
			}
			GraphConstants.setOpaque(map, true);
			cell.setAttributes(map);

			cells.add(cell);
			cellMap.put(entry.getKey(), cell);
		}

		/*
		 * Create edges
		 */
		for (Entry<String, Node> entry : nodes.entrySet()) {
			if (!entry.getKey().equals(".")) {
				// Search predecessor
				DefaultGraphCell prevCell = (DefaultGraphCell) cellMap
						.get(entry.getKey().substring(0,
								entry.getKey().length() - 2));
				DefaultGraphCell thisCell = (DefaultGraphCell) cellMap
						.get(entry.getKey());

				DefaultEdge edge = new DefaultEdge();
				edge.setSource(prevCell.getChildAt(0));
				edge.setTarget(thisCell.getChildAt(0));

				AttributeMap m = edge.getAttributes();
				GraphConstants.setLineEnd(m, GraphConstants.ARROW_CLASSIC);
				GraphConstants.setEndFill(m, true);
				GraphConstants.setRouting(m, ParallelEdgeRouter
						.getSharedInstance());
				GraphConstants.setConnectable(m, false);
				GraphConstants.setDisconnectable(m, false);
				GraphConstants.setEditable(m, false);
				edge.setAttributes(m);

				cells.add(edge);
			}
		}

		DefaultGraphModel model = new DefaultGraphModel();
		GraphLayoutCache view = new GraphLayoutCache(model,
				new DefaultCellViewFactory());

		JGraph graph = new JGraph(model, view);
		graph.getGraphLayoutCache().insert(cells.toArray());

		// Construct Return value
		GraphAndRootNode gn = new GraphAndRootNode();
		gn.graph = graph;
		gn.root = cellMap.get(".");

		return gn;
	}

	private HashMap<String, Node> parseKnowledgebase(Knowledgebase knowledgebase) {
		HashMap<String, Node> nodes = new HashMap<String, Node>();

		/*
		 * Parse the representation
		 */
		BufferedReader reader = new BufferedReader(new StringReader(
				knowledgebase.toString()));
		String line;
		try {
			// read header
			line = reader.readLine();
			if (!line.trim().equals("knowledgebase {")) {
				return null;
			}

			while ((line = reader.readLine()) != null) {
				// End?
				if (line.trim().equals("}")) {
					break;
				}

				// Parse entry
				line = line.trim();
				line = line.replaceFirst("node ", "");
				String[] split = line.split(" ");

				Node node = new Node();
				// node description
				node.description = split[0];
				// node marked
				if (split[2].equals("%"))
					node.marked = Node.MARKED_DONTCARE;
				else if (split[2].equals("!"))
					node.marked = Node.MARKED_ANSWERED;
				else if (split[2].equals("?"))
					node.marked = Node.MARKED_QUERY;
				else
					return null;
				// answer
				if (node.marked == Node.MARKED_ANSWERED) {
					if (split.length == 5) {
						node.answer = split[4].equals("+");
					} else {
						System.out.println("Something wrong");
						return null;
					}
				}

				nodes.put(node.description, node);
			}

			return nodes;
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}
	}

	@SuppressWarnings("unchecked")
	private void applyLayout(JGraph graph, GraphCell root, JGraphLayout layout) {
		JGraphFacade facade = new JGraphFacade(graph, new GraphCell[] { root });

		layout.run(facade);
		Map nested = facade.createNestedMap(true, true);

		graph.getGraphLayoutCache().edit(nested);
	}

	@SuppressWarnings("unchecked")
	private void moveGraph(JGraph graph, int x, int y) {
		Hashtable<GraphCell, Hashtable<AttributeMap, Object>> nested = new Hashtable<GraphCell, Hashtable<AttributeMap, Object>>();

		for (Object o : graph.getGraphLayoutCache().getCells(false, true,
				false, true)) {
			if (DefaultGraphModel.isVertex(graph.getGraphLayoutCache()
					.getModel(), o)) {
				DefaultGraphCell cell = (DefaultGraphCell) o;
				AttributeMap m = cell.getAttributes();

				m.translate(25, 50);
				nested.put(cell, m);
			}
		}
		graph.getGraphLayoutCache().edit(nested, null, null, null);
	}

	private static JPanel createPanel(JGraphVisualizer visualization) {
		/*
		 * Visualizer
		 */
		JPanel panel = new JPanel(new BorderLayout());
		panel.setPreferredSize(new Dimension(640, 480));
		panel.add(new JScrollPane(visualization), BorderLayout.CENTER);

		/*
		 * Zoom
		 */
		JPanel zoomPanel = new JPanel();
		panel.add(zoomPanel, BorderLayout.SOUTH);
		zoomPanel.setBorder(new TitledBorder("Zoom"));
		// JSlider
		final JSlider slider = new JSlider(new DefaultBoundedRangeModel(10, 1,
				1, 100));
		final JGraphVisualizer tmp = visualization;
		slider.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				double zoomFactor = ((double) slider.getValue()) / 10d;
				tmp.graph.setScale(zoomFactor);
			}
		});
		zoomPanel.add(slider);
		// Reset button
		JButton resetButton = new JButton("Reset");
		resetButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				tmp.graph.setScale(1);
				slider.setValue(10);
			}
		});
		zoomPanel.add(resetButton);

		return panel;
	}

	public static JPanel createVisualization(Automaton automaton) {
		return createPanel(new JGraphVisualizer(automaton));
	}

	public static JPanel createVisualization(Knowledgebase knowledgebase) {
		return createPanel(new JGraphVisualizer(knowledgebase));
	}

	public static JDialog createVisualizationDialog(Automaton automaton,
			Window owner) {
		JDialog dialog = new JDialog(owner);
		dialog.setTitle("Automaton Preview");
		dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
		dialog.setLayout(new BorderLayout());
		dialog.setSize(640, 480);
		dialog.setModal(true);
		dialog.add(JGraphVisualizer.createVisualization(automaton),
				BorderLayout.CENTER);

		return dialog;
	}

	public static JDialog createVisualizationDialog(
			Knowledgebase knowledgebase, Window owner) {
		JDialog dialog = new JDialog(owner);
		dialog.setTitle("Preview");
		dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
		dialog.setLayout(new BorderLayout());
		dialog.setSize(640, 480);
		dialog.setModal(true);
		dialog.add(JGraphVisualizer.createVisualization(knowledgebase),
				BorderLayout.CENTER);

		return dialog;
	}

	// /**
	// * @param args
	// */
	// public static void main(String[] args) {
	// System.setProperty("swing.aatext", "true");
	//
	// /*
	// * create knowledgebase
	// */
	// LibALFFactory f = JNIFactory.STATIC;
	// Knowledgebase base = f.createKnowledgebase();
	// base.add_knowledge(new int[] { 0, 1 }, true);
	// base.add_knowledge(new int[] { 0, 1, 1 }, false);
	// base.resolve_or_add_query(new int[] { 1 });
	//
	// /*
	// * Create Automaton
	// */
	// @SuppressWarnings("unused")
	// Automaton a = Automaton.makeString("11").repeat(1);
	//
	// /*
	// * Create graph panel
	// */
	// JPanel graphPanel = JGraphVisualizer.createVisualization(base);
	//
	// /*
	// * Frame
	// */
	// JFrame testFrame = new JFrame("JGraph test");
	// testFrame.setLayout(new BorderLayout());
	// testFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	// testFrame.getContentPane().add(graphPanel, BorderLayout.CENTER);
	// testFrame.setLocationRelativeTo(null);
	// testFrame.setSize(640, 480);
	// testFrame.setVisible(true);
	// }

	private class GraphAndRootNode {
		JGraph graph = null;
		GraphCell root = null;
	}

	private class Node {

		public static final int MARKED_DONTCARE = 0;
		public static final int MARKED_ANSWERED = 1;
		public static final int MARKED_QUERY = 2;

		String description = null;
		Integer marked = null;
		Boolean answer = null;

		public String toString() {
			return "[" + description + ", " + marked + ", " + answer + "]";
		}
	}
}

class StateView extends VertexView {
	private static final long serialVersionUID = 1L;

	static StateRenderer renderer = new StateRenderer();

	public StateView(Object c) {
		super(c);
	}

	// Returns Perimeter Point for Ellipses
	@Override
	public Point2D getPerimeterPoint(EdgeView edge, Point2D source, Point2D p) {
		if (getRenderer() instanceof StateRenderer)
			return ((StateRenderer) getRenderer()).getPerimeterPoint(this,
					source, p);
		return super.getPerimeterPoint(edge, source, p);
	}

	// Returns the Renderer for this View
	@Override
	public CellViewRenderer getRenderer() {
		return renderer;
	}

	static class StateRenderer extends VertexRenderer {
		private static final long serialVersionUID = 1L;

		public void paint(Graphics g) {
			super.paint(g);
			// g.drawImage(image, 0, 0, getWidth(), getHeight(), this);
			g.drawOval(1, 1, getWidth() - 2, getHeight() - 2);
		}
	}
}

class FinalStateView extends VertexView {
	private static final long serialVersionUID = 1L;

	private static FinalStateRenderer renderer = new FinalStateRenderer();

	public FinalStateView() {
		this(null);
	}

	public FinalStateView(Object c) {
		super(c);
	}

	@Override
	public Point2D getPerimeterPoint(EdgeView edge, Point2D source, Point2D p) {
		if (getRenderer() instanceof FinalStateRenderer)
			return ((FinalStateRenderer) getRenderer()).getPerimeterPoint(this,
					source, p);
		return super.getPerimeterPoint(edge, source, p);
	}

	@Override
	public CellViewRenderer getRenderer() {
		return renderer;
	}

	static class FinalStateRenderer extends VertexRenderer {
		private static final long serialVersionUID = 1L;

		public void paint(Graphics g) {
			super.paint(g);
			// g.drawImage(image, 0, 0, getWidth(), getHeight(), this);
			g.drawOval(1, 1, getWidth() - 2, getHeight() - 2);
			g.drawOval(4, 4, getWidth() - 8, getHeight() - 8);
		}
	}
}

class InitialStateView extends VertexView {
	private static final long serialVersionUID = 1L;

	private static InitialStateRenderer renderer = new InitialStateRenderer();

	public InitialStateView() {
		this(null);
	}

	public InitialStateView(Object c) {
		super(c);
	}

	// Returns Perimeter Point for Ellipses
	@Override
	public Point2D getPerimeterPoint(EdgeView edge, Point2D source, Point2D p) {
		if (getRenderer() instanceof InitialStateRenderer)
			return ((InitialStateRenderer) getRenderer()).getPerimeterPoint(
					this, source, p);
		return super.getPerimeterPoint(edge, source, p);
	}

	@Override
	public CellViewRenderer getRenderer() {
		return renderer;
	}

	static class InitialStateRenderer extends VertexRenderer {
		private static final long serialVersionUID = 1L;

		public void paint(Graphics g) {
			super.paint(g);
			Graphics2D g2 = (Graphics2D) g;

			g.drawOval(16, 1, 38, 38);
			g.drawLine(0, 20, 16, 20);
			GeneralPath p = new GeneralPath();
			p.moveTo(16, 20);
			p.lineTo(7, 15);
			p.lineTo(11, 20);
			p.lineTo(7, 25);
			p.lineTo(16, 20);
			g2.draw(p);
			g2.fill(p);

		}
	}
}

class InitialFinalStateView extends VertexView {
	private static final long serialVersionUID = 1L;

	private static InitialFinalStateRenderer renderer = new InitialFinalStateRenderer();

	public InitialFinalStateView() {
		this(null);
	}

	public InitialFinalStateView(Object c) {
		super(c);
	}

	// Returns Perimeter Point for Ellipses
	@Override
	public Point2D getPerimeterPoint(EdgeView edge, Point2D source, Point2D p) {
		if (getRenderer() instanceof InitialFinalStateRenderer)
			return ((InitialFinalStateRenderer) getRenderer())
					.getPerimeterPoint(this, source, p);
		return super.getPerimeterPoint(edge, source, p);
	}

	@Override
	public CellViewRenderer getRenderer() {
		return renderer;
	}

	static class InitialFinalStateRenderer extends VertexRenderer {
		private static final long serialVersionUID = 1L;

		public void paint(Graphics g) {
			super.paint(g);
			Graphics2D g2 = (Graphics2D) g;

			g.drawOval(16, 1, 38, 38);
			g.drawOval(19, 4, 32, 32);

			g.drawLine(0, 20, 16, 20);
			GeneralPath p = new GeneralPath();
			p.moveTo(16, 20);
			p.lineTo(7, 15);
			p.lineTo(11, 20);
			p.lineTo(7, 25);
			p.lineTo(16, 20);
			g2.draw(p);
			g2.fill(p);

		}
	}
}

class StateCell extends DefaultGraphCell {
	private static final long serialVersionUID = 1L;
}

class FinalStateCell extends DefaultGraphCell {
	private static final long serialVersionUID = 1L;
}

class InitialStateCell extends DefaultGraphCell {
	private static final long serialVersionUID = 1L;
}

class InitialFinalStateCell extends DefaultGraphCell {
	private static final long serialVersionUID = 1L;
}