package de.libalf.demo.gui;

import java.awt.BorderLayout;
import java.awt.Desktop;
import java.awt.Graphics;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.beans.PropertyVetoException;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;

import javax.imageio.ImageIO;
import javax.swing.JDesktopPane;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.filechooser.FileNameExtensionFilter;

import de.libalf.demo.Scenario;

public class DemoDesktop extends JFrame {

	private static final long serialVersionUID = 1L;

	public static final int OK = 1;
	public static final int CANCEL = 0;

	private JDesktopPane desktop;
	private BufferedImage backgroundImage = null;

	public DemoDesktop() {
		/*
		 * Try to load background image
		 */
		try {
			// Get current classloader
			ClassLoader cl = this.getClass().getClassLoader();

			URL imageURL = cl.getResource("res/rwth_logo.jpg");
			if (imageURL == null)
				imageURL = new File("res/rwth_logo.jpg").toURI().toURL();

			backgroundImage = ImageIO.read(imageURL);
		} catch (IOException e) {
			System.err.println("Could not load background image.");
		}

		/*
		 * Build GUI
		 */
		try {
			// UIManager
			// .setLookAndFeel(
			// "com.sun.java.swing.plaf.nimbus.NimbusLookAndFeel" );
		} catch (Exception e) {
			e.printStackTrace();
		}
		buildGUI();
	}

	private void buildGUI() {
		/*
		 * Stuff
		 */
		setTitle("libalf Demo");
		setSize(800, 600);
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setExtendedState(MAXIMIZED_BOTH);
		setLayout(new BorderLayout());

		/*
		 * Menu
		 */
		{
			JMenuBar menubar = new JMenuBar();
			setJMenuBar(menubar);

			/*
			 * Scenario Menu
			 */
			JMenu scenarioMenu = new JMenu("Scenario");
			menubar.add(scenarioMenu);

			// New Scenario
			JMenuItem newItem = new JMenuItem("New Scenario");
			scenarioMenu.add(newItem);
			newItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					addScenarioFrame(null);
				}
			});

			// Load scenarion
			JMenuItem loadItem = new JMenuItem("Load Scenario");
			scenarioMenu.add(loadItem);
			loadItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JFileChooser chooser = new JFileChooser();
					FileNameExtensionFilter filter = new FileNameExtensionFilter(
							"Scenarios", "scenario");
					chooser.setFileFilter(filter);
					int returnVal = chooser.showOpenDialog(DemoDesktop.this);
					if (returnVal == JFileChooser.APPROVE_OPTION) {
						try {
							ObjectInputStream ois = new ObjectInputStream(
									new FileInputStream(chooser
											.getSelectedFile()));
							addScenarioFrame((Scenario) ois.readObject());
						} catch (Exception exception) {
							JOptionPane.showMessageDialog(DemoDesktop.this,
									exception, "Error",
									JOptionPane.ERROR_MESSAGE);
						}
					}
				}
			});

			// Exit
			scenarioMenu.addSeparator();
			JMenuItem exitItem = new JMenuItem("Exit");
			scenarioMenu.add(exitItem);
			exitItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					System.exit(0);
				}
			});

			/*
			 * Help Menu
			 */
			JMenu helpMenu = new JMenu("Help");
			menubar.add(helpMenu);

			// Visit
			JMenuItem visitItem = new JMenuItem("Visit libalf in the web");
			helpMenu.add(visitItem);
			visitItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					if (Desktop.isDesktopSupported()
							&& Desktop.getDesktop().isSupported(
									Desktop.Action.BROWSE)) {
						try {
							Desktop
									.getDesktop()
									.browse(
											new URI(
													"http://libalf.informatik.rwth-aachen.de"));
						} catch (IOException e1) {
							e1.printStackTrace();
						} catch (URISyntaxException e1) {
							e1.printStackTrace();
						}
					} else {
						JOptionPane
								.showMessageDialog(
										DemoDesktop.this,
										new String[] {
												"Opening a browser is not supported by our system.",
												"\n",
												"Visit us at http://libalf.informatik.rwth-aachen.de" },
										"Browsing not available",
										JOptionPane.INFORMATION_MESSAGE);
					}
				}
			});

			// About
			helpMenu.addSeparator();
			JMenuItem aboutItem = new JMenuItem("About");
			helpMenu.add(aboutItem);
			aboutItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					new AboutDialog();
				}
			});
		}

		/*
		 * Desktop pane
		 */
		{
			desktop = new JDesktopPane() {
				private static final long serialVersionUID = 1L;
				private int x = Toolkit.getDefaultToolkit().getScreenSize().width - 500;
				private int y = Toolkit.getDefaultToolkit().getScreenSize().height - 250;

				public void paintComponent(Graphics g) {
					super.paintComponent(g);
					if (backgroundImage != null)
						g.drawImage(backgroundImage, x, y, this);
				}
			};
			add(desktop, BorderLayout.CENTER);
			desktop.addMouseListener(new MouseAdapter() {
				public void mouseClicked(MouseEvent e) {
					if (e.getButton() == MouseEvent.BUTTON3) {
						JPopupMenu popup = new MyPopup();
						popup
								.show(
										DemoDesktop.this,
										DemoDesktop.this.getExtendedState() == JFrame.MAXIMIZED_HORIZ ? e
												.getX()
												: e.getXOnScreen(),
										DemoDesktop.this.getExtendedState() == JFrame.MAXIMIZED_VERT ? e
												.getY()
												: e.getYOnScreen());
					}
				}
			});
		}

		/*
		 * More stuff
		 */
		setVisible(true);
	}

	private void addScenarioFrame(Scenario s) {
		ScenarioEditor editor = new ScenarioEditor(s == null ? Scenario
				.createDefaultScenario() : s);
		int returnVal = editor.showScenarioEditor();
		Scenario sce;
		switch (returnVal) {
		case ScenarioEditor.NEW_DEFAULT_ONLINE:
			sce = editor.getScenario();
			if (sce.getDescription().equals(""))
				sce.setDescription("Default online scenario");
			DefaultOnlineScenarioFrame frame1 = new DefaultOnlineScenarioFrame(
					sce);
			desktop.add(frame1);
			try {
				frame1.setSelected(true);
			} catch (PropertyVetoException e) {
			}
			break;
		case ScenarioEditor.NEW_DEFAULT_OFFLINE:
			sce = editor.getScenario();
			if (sce.getDescription().equals(""))
				sce.setDescription("Default offline scenario");
			DefaultOfflineScenarioFrame frame2 = new DefaultOfflineScenarioFrame(
					sce);
			desktop.add(frame2);
			try {
				frame2.setSelected(true);
			} catch (PropertyVetoException e) {
			}
			break;
		}
	}

	/**
	 * Starts the demo desktop.
	 * 
	 * @param args
	 *            the command line arguments
	 */
	public static void main(String[] args) {
		new DemoDesktop();
	}

	private class MyPopup extends JPopupMenu {
		private static final long serialVersionUID = 1L;

		public MyPopup() {
			JMenuItem newItem = new JMenuItem("New scenario");
			add(newItem);
			newItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent arg0) {
					addScenarioFrame(null);
				}
			});
		}
	}

}
