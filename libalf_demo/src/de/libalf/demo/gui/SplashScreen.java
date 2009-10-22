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

import java.awt.Graphics;
import java.awt.GridBagLayout;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.URL;

import javax.imageio.ImageIO;
import javax.swing.JWindow;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 *
 */
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
