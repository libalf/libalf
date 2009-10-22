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

package de.libalf.demo;

import java.util.GregorianCalendar;

import javax.swing.SwingWorker;

import de.libalf.demo.gui.DemoDesktop;
import de.libalf.demo.gui.SplashScreen;

/**
 * 
 * @author Daniel Neider
 * @version 0.1
 */
public class Starter {

	/**
	 * Starts the libalf demo.
	 * 
	 * @param args
	 *            paramaters are ignored
	 */
	public static void main(String[] args) {
		SwingWorker<Void, Void> w = new SwingWorker<Void, Void>() {
			@Override
			protected Void doInBackground() throws Exception {
				SplashScreen splash = new SplashScreen();
				splash.setVisible(true);

				long end = new GregorianCalendar().getTimeInMillis();
				long start = end;
				do {
					Thread.sleep(250);
					end = new GregorianCalendar().getTimeInMillis();
				} while (!isCancelled() || (end - start < 3000));

				splash.setVisible(false);
				return null;
			}
		};
		w.execute();

		long start = new GregorianCalendar().getTimeInMillis();
		DemoDesktop desktop = new DemoDesktop();
		long end = new GregorianCalendar().getTimeInMillis();
		if (end - start < DemoDesktop.SPLASH_SCREEN_DISPLAY) {
			try {
				Thread.sleep(DemoDesktop.SPLASH_SCREEN_DISPLAY - (end - start));
			} catch (InterruptedException e) {
			}
		}
		desktop.setVisible(true);
		desktop.toFront();

		w.cancel(false);

	}

}
