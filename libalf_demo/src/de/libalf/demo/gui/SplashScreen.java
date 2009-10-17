package de.libalf.demo.gui;

import java.awt.Graphics;
import java.awt.GridBagLayout;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.URL;

import javax.imageio.ImageIO;
import javax.swing.JWindow;

public class SplashScreen extends JWindow {

	private static final long serialVersionUID = 1L;

	private BufferedImage splashImage = null;

	public SplashScreen() {

		/*
		 * Try to load slpash screen image
		 */
		try {
			// Get current classloader
			ClassLoader cl = this.getClass().getClassLoader();

			URL imageURL = cl.getResource("res/splash.jpg");
			if (imageURL == null)
				imageURL = new File("res/splash.jpg").toURI().toURL();

			splashImage = ImageIO.read(imageURL);
		} catch (IOException e) {
			System.err.println("Could not load splash screen.");
		}

		buildGUI();
	}

	private void buildGUI() {
		// Size
		setSize(splashImage.getWidth(), splashImage.getHeight());

		// Position
		setLocationRelativeTo(null);

		// Stuff
		setAlwaysOnTop(true);

		// Layout
		setLayout(new GridBagLayout());

	}

	public void paint(Graphics g) {
		g.drawImage(splashImage, 0, 0, this);
	}
}
