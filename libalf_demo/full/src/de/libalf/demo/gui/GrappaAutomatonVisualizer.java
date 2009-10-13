package de.libalf.demo.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Window;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.File;
import java.io.StringReader;
import java.net.URL;
import java.net.URLConnection;

import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import att.grappa.Graph;
import att.grappa.GrappaSupport;
import att.grappa.Parser;
import dk.brics.automaton.Automaton;

public class GrappaAutomatonVisualizer extends JPanel implements ItemListener,
		ChangeListener {

	public static final File DOT_SCRIPT = new File("dot_layout.sh");

	private static final long serialVersionUID = 1L;

	/**
	 * Stores the graph.
	 */
	private att.grappa.GrappaPanel grappaPanel;

	private JCheckBox scaleToFitBox;

	private JSlider zoomSlider = null;

	private static final int DEFAULT_ZOOM = 10;

	private GrappaAutomatonVisualizer() {
		super();
	}

	private static void layoutGraph(Graph graph) {
		/*
		 * Layout graph
		 */
		Object connector = null;
		try {
			// connector = Runtime.getRuntime().exec("/bin/bash dot_layout",
			// null, new File("/home/daniel/demo/"));
			connector = Runtime.getRuntime().exec("");
		} catch (Exception ex) {
			// System.err.println("Exception while setting up Process: "
			// + ex.getMessage() + "\nTrying URLConnection...");
			connector = null;
		}
		if (connector == null) {
			try {
				connector = (new URL(
						"http://www.research.att.com/~john/cgi-bin/format-graph"))
						.openConnection();
				URLConnection urlConn = (URLConnection) connector;
				urlConn.setDoInput(true);
				urlConn.setDoOutput(true);
				urlConn.setUseCaches(false);
				urlConn.setRequestProperty("Content-Type",
						"application/x-www-form-urlencoded");
			} catch (Exception ex) {
				System.err.println("Exception while setting up URLConnection: "
						+ ex.getMessage() + "\nLayout not performed.");
				connector = null;
			}
		}
		if (connector != null) {
			if (!GrappaSupport.filterGraph(graph, connector)) {
				System.err.println("ERROR: somewhere in filterGraph");
			}
			if (connector instanceof Process) {
				try {
					int code = ((Process) connector).waitFor();
					if (code != 0) {
						System.err.println("WARNING: proc exit code is: "
								+ code);
					}
				} catch (InterruptedException ex) {
					System.err.println("Exception while closing down proc: "
							+ ex.getMessage());
					ex.printStackTrace(System.err);
				}
			}
			connector = null;
		}
		graph.repaint();
	}

	/**
	 * <p>
	 * Creates a <em>Grappa graph</em> from a dot-<code>String</code>.
	 * </p>
	 * <p>
	 * This is a 3 step process:
	 * <ol>
	 * <li>Parse the <code>String</code></li>
	 * <li>Layout the graph</li>
	 * <li>Create the Grappa panel</li>
	 * <li></li>
	 * </ol>
	 * </p>
	 * 
	 * @param graphDescription
	 *            a <code>String</code> containing a dot representation of a
	 *            graph.
	 * @return a <em>Grappa-Panel</em> (instance of <code>JPanel</code>).
	 */
	public static att.grappa.GrappaPanel createGrappaPanel(
			String graphDescription) {

		/*
		 * 1. Parse
		 */
		Parser parser = new Parser(new StringReader(graphDescription));
		try {
			parser.parse();
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
		Graph graph = parser.getGraph();

		/*
		 * 2. Parse
		 */
		layoutGraph(graph);

		/*
		 * 3. Create Grappa panel
		 */
		att.grappa.GrappaPanel grappaPanel = new att.grappa.GrappaPanel(graph);
		grappaPanel.setScaleToFit(true);

		return grappaPanel;
	}

	public static GrappaAutomatonVisualizer createZoomableGrappaPanel(
			String graphDescription) {

		/*
		 * Prepare panel
		 */
		GrappaAutomatonVisualizer panel = new GrappaAutomatonVisualizer();
		panel.setLayout(new GridBagLayout());

		/*
		 * Zoom panel
		 */
		JPanel zoomPanel = new JPanel(new BorderLayout(15, 0));
		zoomPanel.setBorder(new TitledBorder("Zoom"));

		// Scale to fit checkbox
		panel.scaleToFitBox = new JCheckBox("Scale to fit", true);
		panel.scaleToFitBox.addItemListener(panel);
		zoomPanel.add(panel.scaleToFitBox, BorderLayout.WEST);

		// Zoom slider
		panel.zoomSlider = new JSlider(1, 20, DEFAULT_ZOOM);
		panel.zoomSlider.setEnabled(false);
		panel.zoomSlider.addChangeListener(panel);
		zoomPanel.add(panel.zoomSlider, BorderLayout.CENTER);

		/*
		 * Graph panel
		 */
		panel.grappaPanel = createGrappaPanel(graphDescription);
		JScrollPane scrollPane = new JScrollPane(panel.grappaPanel);
		scrollPane.setBackground(Color.WHITE);
		scrollPane.setOpaque(true);

		/*
		 * Add panels
		 */
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = c.gridy = 0;
		c.weightx = 1;
		c.weighty = .9;
		c.anchor = GridBagConstraints.CENTER;
		// Automaton
		panel.add(scrollPane, c);
		// Zoom
		c.gridx = 0;
		c.gridy = 1;
		c.weightx = 1;
		c.weighty = .1;
		c.fill = GridBagConstraints.NONE;
		panel.add(zoomPanel, c);

		return panel;
	}

	private static double computeZoomFactor(int value) {
		return (double) value / (double) DEFAULT_ZOOM;
	}

	public static JDialog createAutomatonDisplayDialog(Automaton a, Window owner) {
		JDialog dialog = new JDialog(owner);
		dialog.setTitle("Automaton Preview");
		dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
		dialog.setLayout(new BorderLayout());
		dialog.setSize(640, 480);
		dialog.setModal(true);
		dialog.add(GrappaAutomatonVisualizer.createZoomableGrappaPanel(a
				.toDot()), BorderLayout.CENTER);

		return dialog;
	}

	@Override
	public void itemStateChanged(ItemEvent event) {
		if (scaleToFitBox.isSelected()) {
			zoomSlider.setEnabled(false);
			zoomSlider.setValue(DEFAULT_ZOOM);
			grappaPanel.setScaleToFit(true);
			grappaPanel.repaint();
		} else {
			zoomSlider.setEnabled(true);
			grappaPanel.setScaleToFit(false);
			grappaPanel.resetZoom();
			grappaPanel.multiplyScaleFactor(computeZoomFactor(zoomSlider
					.getValue()));
			grappaPanel.repaint();
		}
	}

	@Override
	public void stateChanged(ChangeEvent event) {
		if (zoomSlider.isEnabled()) {
			grappaPanel.resetZoom();
			grappaPanel.multiplyScaleFactor(computeZoomFactor(zoomSlider
					.getValue()));
			grappaPanel.revalidate();
			grappaPanel.repaint();
		}
	}
}
